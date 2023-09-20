#pragma once
#include "pch.h"
#include "Renderable.h"

// TO-DO: Simplify the way in which boilerplate is handled for rendering

#define QUAD_INDICES 6 

static void CreateNDCQuad(ID3D11Buffer** pScreenQuadVertexBuffer, ID3D11Buffer** pScreenQuadIndexBuffer, UINT& screenQuadNrVerticies, UINT& screenQuadNrIndicies)
{
	ID3D11Device* pDevice = EngineManager::Instance()->GetDevice()->GetDevice();

	//***********************************************************************************
	// 	   Quad
	//***********************************************************************************
	D3D11_SUBRESOURCE_DATA vertexMS, indexMS;
	D3D11_BUFFER_DESC vertexBD, indexBD;
	HRESULT result;

	std::vector<QuadVertex> vertices{};
	std::vector<unsigned int> indices{};

	//Create screen quad for deferred
	vertices.resize(4);
	indices.resize(QUAD_INDICES);

	//Position coordinates specified in NDC space.
	vertices[0] = { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
	vertices[1] = { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } };
	vertices[2] = { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } };
	vertices[3] = { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	screenQuadNrVerticies = (UINT)vertices.size();
	screenQuadNrIndicies = (UINT)indices.size();

	//Vertex
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.ByteWidth = sizeof(QuadVertex) * screenQuadNrVerticies;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.CPUAccessFlags = 0;
	vertexBD.MiscFlags = 0;
	vertexBD.StructureByteStride = sizeof(QuadVertex);

	//Give the subresource structure a pointer to the index data.
	vertexMS.pSysMem = &vertices[0];
	vertexMS.SysMemPitch = 0;
	vertexMS.SysMemSlicePitch = 0;
	result = pDevice->CreateBuffer(&vertexBD, &vertexMS, pScreenQuadVertexBuffer);
	if (FAILED(result))
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Renderer: Failed to create vertexbuffer for defered render quad."));
		return;
	}

	//Index
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.ByteWidth = sizeof(unsigned int) * screenQuadNrIndicies;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.CPUAccessFlags = 0;
	indexBD.MiscFlags = 0;
	indexBD.StructureByteStride = sizeof(unsigned int);

	//Give the subresource structure a pointer to the index data.
	indexMS.pSysMem = &indices[0];
	indexMS.SysMemPitch = 0;
	indexMS.SysMemSlicePitch = 0;
	result = pDevice->CreateBuffer(&indexBD, &indexMS, pScreenQuadIndexBuffer);
	if (FAILED(result))
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Renderer: Failed to create indexbuffer for defered render quad."));
		return;
	}
}

static bool IsRenderTypeInVector(RenderType type, std::vector<RenderType>& vector)
{
	auto determineVectorRenderCapable = [](RenderType type, std::vector<RenderType> vector)
	{
		for (RenderType vectorType : vector)
		{
			if (vectorType == type)
				return true;
		}
		return false;
	};
	return determineVectorRenderCapable(type, vector);
}
static std::vector<RenderType> FilterRenderTypesFromVectors(std::vector<RenderType>& types, std::vector<RenderType>& vector)
{
	// Reserve maximum capacity to avoid multiple allocations
	std::vector<RenderType> output;
	std::vector<RenderType>& vectorToCheck = vector;
	output.reserve(vectorToCheck.size());

	for (RenderType type : types)
	{
		if (IsRenderTypeInVector(type, vectorToCheck))
		{
			output.push_back(type);
		}
	}
	return output;
}

