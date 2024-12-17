#include "pch.h"
#include "FileManager.h"
#include "ShaderStructures.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshAsset.h"

std::wstring FileManager::GetAbsoluteFolderPathofFile(const std::wstring& localFilePath)
{
	return GetAbsoluteExePath() + GetLocalFolderPathofFile(localFilePath);
}
std::wstring FileManager::GetLocalFolderPathofFile(const std::wstring& localFilePath)
{
	const size_t last_slash_idx = localFilePath.rfind(L'\\');
	if (std::wstring::npos != last_slash_idx)
	{
		return localFilePath.substr(0, last_slash_idx);
	}
	else 
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed getting LocalFolderPathofFile."));
		return L"";
	}
}
std::wstring FileManager::GetAbsoluteExePath()
{
	//Get the full path
	WCHAR fullPath[MAX_PATH];
	if (!GetModuleFileNameW(NULL, fullPath, sizeof(fullPath) / 2))
	{
		return L"";
	}

	//Regex search for the full path withouth the .exe
	std::wstring str(fullPath);
	std::wregex reg(L"(.+\\\\).+\\.exe$");
	std::wsmatch matches;
	std::regex_match(str, matches, reg);

	if (!matches.size())
	{
		return L"";
	}

	return matches[1].str();
}
std::wstring FileManager::GetFileName(const std::wstring& filePath)
{
	std::wstring fullFileName = filePath;

	//Regex search
	std::wregex reg(L".+[\\/\\\\](.+)[.].+$");
	std::wsmatch matches;
	std::regex_match(fullFileName, matches, reg);

	if (!matches.size())
	{
		return L"";
	}

	return matches[1].str();
}
std::wstring FileManager::GetFullFilePath(const std::wstring& filePath)
{
	//Regex search for the full path without the filetype
	std::wregex reg(L"(.+\\\\.+)[.].+$");
	std::wsmatch matches;
	std::regex_match(filePath, matches, reg);

	if (!matches.size())
	{
		return L"";
	}

	return matches[1].str();
}

#pragma region Meshes
	#pragma region OBJ
		HRESULT FileManager::LoadOBJ(std::wstring localFileDir, MeshAsset* pMeshAsset)
		{
			// Read the file from disk
			const std::string absoluteFilePath = V_TEXT(GetAbsoluteExePath() + localFileDir);
			std::ifstream fileRead{ absoluteFilePath };
		
			if (!fileRead.is_open())
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed opening file for LoadOBJ \"" + absoluteFilePath + "\". With error: " + strerror(errno)));
				return E_FAIL;
			}
		
			std::string objFileContent{};
			{
				std::stringstream buffer;
				buffer << fileRead.rdbuf();
				objFileContent = buffer.str();
			}
			fileRead.close();
		
			// Regex parse
			// Basic data
			int vertexCount, faceCount;
			if (FAILED(FileManager::OBJParseBasicData(objFileContent, vertexCount, faceCount)))
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed Regex for the basic data."));
				return E_FAIL;
			}
		
			// Vertex data
			std::vector<DirectX::XMFLOAT3>& position = pMeshAsset->GetPositions();
			std::vector<std::vector<DirectX::XMFLOAT2>>& uvs = pMeshAsset->GetUVs();
			std::vector<DirectX::XMFLOAT3>& normals = pMeshAsset->GetNormals();
			if (FAILED(FileManager::OBJParseVertexData(objFileContent, vertexCount, position, uvs, normals)))
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed Regex for vertices."));
				return E_FAIL;
			}
		
			// Index data
			std::vector<DirectX::XMINT3>& indices = pMeshAsset->GetUnpackedIndicies();
			if (FAILED(FileManager::OBJParseIndices(objFileContent, faceCount, indices)))
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed Regex for indices."));
				return E_FAIL;
			}
		
			// Pack into the mesh
			pMeshAsset->RefillVertexVector();
		
			return S_OK;
		}
		HRESULT FileManager::OBJParseBasicData(std::string& objFileContent, int& outVertexCount, int& outFaceCount)
{
	std::regex reg{ "# vertex count = (\\d+)" };
	std::smatch matches{};
	std::regex_search(objFileContent, matches, reg);

	if (!matches.size())
	{
		return E_FAIL;
	}

	outVertexCount = std::stoi(matches[1]);
	objFileContent = matches.suffix().str();

	reg = { "# face count = (\\d+)" };
	matches = {};
	std::regex_search(objFileContent, matches, reg);

	if (!matches.size())
	{
		return E_FAIL;
	}

	outFaceCount = std::stoi(matches[1]);
	objFileContent = matches.suffix().str();

	return S_OK;
}
		HRESULT FileManager::OBJParseVertexData(std::string& objFileContent, const int vertexCount, 
			std::vector<DirectX::XMFLOAT3>& positions,
			std::vector<std::vector<DirectX::XMFLOAT2>>& uvs, 
			std::vector<DirectX::XMFLOAT3>& normals)
{
	// Vertex data
	// Pos
	std::regex reg{ "v (-?\\d*\\.?\\d*e?-?\\d{0,3}) (-?\\d*\\.?\\d*e?-?\\d{0,3}) (-?\\d*\\.?\\d*e?-?\\d{0,3})\\n" };
	std::smatch matches{};
	positions.reserve(vertexCount);
	for (int idx = 0; idx < vertexCount; ++idx)
	{
		std::regex_search(objFileContent, matches, reg);

		if (!matches.size())
		{
			return E_FAIL;
		}

		positions.emplace_back(std::stof(matches[1]), std::stof(matches[2]), std::stof(matches[3]));
		objFileContent = matches.suffix().str();
	}

	// UVs
	reg = { "vt (-?\\d+.?\\d?) (-?\\d+.?\\d?) \\[(\\d)\\]\\n" };
	matches = {};
	{
		int nrUVMaps = 1;
		uvs.push_back({});
		int uvCount = 0;
		while (true)
		{
			std::regex_search(objFileContent, matches, reg);

			if (!matches.size())
			{
				if (uvCount == 0)
				{
					return S_OK;
				}
				else
				{
					break;
				}
			}

			// First confirm the number of maps that need to be accounted for
			// Idx math, every 4th match (count starting at 0) is the map idx
			const int uvMapIdx = std::stoi(matches[3]);
			if (nrUVMaps < uvMapIdx)
			{
				nrUVMaps = uvMapIdx;
				uvs.resize(nrUVMaps);
			}

			// Collect UVs
			uvs[uvMapIdx].emplace_back(std::stof(matches[1]), std::stof(matches[2]));

			objFileContent = matches.suffix().str();
			++uvCount;
		}
	}

	// Normals
	reg = { "vn (-?\\d*\\.?\\d*e?-?\\d{0,3}) (-?\\d*\\.?\\d*e?-?\\d{0,3}) (-?\\d*\\.?\\d*e?-?\\d{0,3})\\n" };
	matches = {};
	{
		int normalCount = 0;
		while (true)
		{
			std::regex_search(objFileContent, matches, reg);

			if (!matches.size())
			{
				if (normalCount == 0)
				{
					V_LOG(LogVerbosity::Log, V_WTEXT("No Normal data present in obj."));
					return S_OK;
				}
				else
				{
					break;
				}
			}

			normals.emplace_back(std::stof(matches[1]), std::stof(matches[2]), std::stof(matches[3]));

			objFileContent = matches.suffix().str();
			++normalCount;
		}
	}

	return S_OK;
}
		HRESULT FileManager::OBJParseIndices(std::string& objFileContent, const int faceCount, std::vector<DirectX::XMINT3>& indices)
{
	enum IndexGroup 
	{
		P,
		PUV,
		PUVN,
		PN
	};

	// Parse a face to see what type of index structure it holds
	IndexGroup indexGroup;
	{
		// Check if there is a comment and if so remove it from the string
		size_t commentCheck = (size_t)objFileContent.find_first_of("#");
		if (commentCheck != std::string::npos)
		{
			commentCheck = (size_t)objFileContent.find_first_of("\n");
			objFileContent = objFileContent.substr(commentCheck + 1, objFileContent.size() - (commentCheck + 1));
		}

		// Locate an example of an indexgroup
		size_t firstFacePos = (size_t)objFileContent.find_first_of("f");
		if (firstFacePos == std::string::npos)
		{
			return E_FAIL;
		}
		size_t indexStartPos = (size_t)objFileContent.find_first_of(" ", firstFacePos);
		size_t indexEndPos = (size_t)objFileContent.find_first_of(" ", indexStartPos + 1);
		std::string firstIndexGroup = objFileContent.substr(indexStartPos, indexEndPos - indexStartPos);
	
		// Check what type of group it is with some checks
		size_t firstSlash = (size_t)firstIndexGroup.find("/");
		size_t secondSlash = (size_t)firstIndexGroup.find("/", firstSlash + 1);
		if (firstSlash == std::wstring::npos) // "X"
		{
			indexGroup = IndexGroup::P;
		}
		else if (secondSlash == std::wstring::npos) // "X/Y"
		{
			indexGroup = IndexGroup::PUV;
		}
		else if (secondSlash - firstSlash == 1) // "X//Z"
		{
			indexGroup = IndexGroup::PN;
		}
		else  // "X/Y/Z"
		{
			indexGroup = IndexGroup::PUVN;
		}
	}

	// Match our data
	std::regex reg{};
	switch (indexGroup)
	{
	case IndexGroup::P:
		reg = { "f (\\d+) (\\d+) (\\d+)" };
		break;
	case IndexGroup::PUV:
		reg = { "f (\\d+)\\/(\\d+) (\\d+)\\/(\\d+) (\\d+)\\/(\\d+)" };
		break;
	case IndexGroup::PUVN:
		reg = { "f (\\d+)\\/(\\d+)\\/(\\d+) (\\d+)\\/(\\d+)\\/(\\d+) (\\d+)\\/(\\d+)\\/(\\d+)" };
		break;
	case IndexGroup::PN:
		reg = { "f (\\d+)\\/\\/(\\d+) (\\d+)\\/\\/(\\d+) (\\d+)\\/\\/(\\d+)" };
		break;
	}
	std::smatch matches{};

	indices.reserve(faceCount);
	int indexCount = 0;
	while (true)
	{
		std::regex_search(objFileContent, matches, reg);

		if (!matches.size())
		{
			if (indexCount == 0)
			{
				return E_FAIL;
			}
			else
			{
				break;
			}
		}

		switch (indexGroup)
		{
		case IndexGroup::P:
			indices.emplace_back(std::stoi(matches[1]) - 1, -1, -1);
			indices.emplace_back(std::stoi(matches[2]) - 1, -1, -1);
			indices.emplace_back(std::stoi(matches[3]) - 1, -1, -1);
			break;
		case IndexGroup::PUV:
			indices.emplace_back(std::stoi(matches[1]) - 1, std::stoi(matches[2]) - 1, -1);
			indices.emplace_back(std::stoi(matches[3]) - 1, std::stoi(matches[4]) - 1, -1);
			indices.emplace_back(std::stoi(matches[5]) - 1, std::stoi(matches[6]) - 1, -1);
			break;
		case IndexGroup::PUVN:
			indices.emplace_back(std::stoi(matches[1]) - 1, std::stoi(matches[2]) - 1, std::stoi(matches[3]) - 1);
			indices.emplace_back(std::stoi(matches[4]) - 1, std::stoi(matches[5]) - 1, std::stoi(matches[6]) - 1);
			indices.emplace_back(std::stoi(matches[7]) - 1, std::stoi(matches[8]) - 1, std::stoi(matches[9]) - 1);
			break;
		case IndexGroup::PN:
			indices.emplace_back(std::stoi(matches[1]) - 1, -1, std::stoi(matches[2]) - 1);
			indices.emplace_back(std::stoi(matches[3]) - 1, -1, std::stoi(matches[4]) - 1);
			indices.emplace_back(std::stoi(matches[5]) - 1, -1, std::stoi(matches[6]) - 1);
			break;
		}

		objFileContent = matches.suffix().str();
		++indexCount;
	}

	return S_OK;
}
	#pragma endregion

	#pragma region glTF
		#define TINYGLTF_IMPLEMENTATION
		#define STB_IMAGE_IMPLEMENTATION
		#define STB_IMAGE_WRITE_IMPLEMENTATION
		// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
		#pragma warning (push, 0) //Ignore all warnings
		#include "../AdditionalDependencies/Includes/tinyglTF/tiny_gltf.h"
		#pragma warning (pop)
		
		HRESULT FileManager::LoadglTF(std::wstring localFileDir, MeshAsset* pMeshAsset)
		{
			const std::string absoluteFilePath = V_TEXT(GetAbsoluteExePath() + localFileDir);
		
			tinygltf::Model model;
			tinygltf::TinyGLTF loader;
			std::string err;
			std::string warn;
		
			bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, absoluteFilePath);
			//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)
		
			if (!warn.empty()) 
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Warning glTF: " + warn));
			}
			if (!err.empty()) 
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Error glTF: " + err));
			}
			if (!ret) 
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed to parse glTF."));
				return E_FAIL;
			}
		
			return S_OK;
		}
	#pragma endregion
#pragma endregion

#pragma region Textures
Texture* FileManager::LoadDDS(ID3D11Device* pDevice, std::wstring localFileDir)
{
	HRESULT hr;

	const std::wstring absoluteFilePath = GetAbsoluteExePath() + localFileDir;

	//Create DX11 variables
	ID3D11Resource* pResource;
	ID3D11ShaderResourceView* pResourceView;
	hr = DirectX::CreateDDSTextureFromFile(pDevice, absoluteFilePath.c_str(), &pResource, &pResourceView);
	if (hr != S_OK)
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed creating a texture from a DDS."));
		return nullptr;
	}

	return new Texture(pResource, pResourceView);
}
#pragma endregion
