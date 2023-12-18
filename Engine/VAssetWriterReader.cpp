#include "pch.h"
#include "VAssetWriterReader.h"
#include "FileManager.h"

VAssetWriterReader::VAssetWriterReader()
	:Singleton<VAssetWriterReader>()
{
}
VAssetWriterReader::~VAssetWriterReader()
{
}

void VAssetWriterReader::WriteAsset(const std::wstring& localPath, const std::wstring& data)
{
    std::wstring absoluteFilePath = FileManager::GetAbsoluteFolderPathofFile(localPath);
    std::ofstream binOutStream(absoluteFilePath, std::ios::binary);
    std::string writeData = UTF8Encode(data);
    binOutStream.write(writeData.c_str(), sizeof(writeData.size()));
}
void VAssetWriterReader::ReadAsset(const std::wstring& localPath, std::wstring& data)
{
    std::wstring absoluteFilePath = FileManager::GetAbsoluteFolderPathofFile(localPath);
    std::ifstream binInStream(absoluteFilePath, std::ios::binary);
    if (binInStream.fail())
    {
        V_LOG(LogVerbosity::Warning, V_WTEXT("VAssetWriterReader: Failed to find file from path: " + V_TEXT(absoluteFilePath) + "."));
        return;
    }
    std::stringstream buffer;
    buffer << binInStream.rdbuf();
    data = UTF8Decode(buffer.str().c_str());
}
