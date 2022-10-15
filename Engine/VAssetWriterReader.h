#pragma once
#include "Singleton.h"

class VAssetWriterReader : public Singleton<VAssetWriterReader>
{
public:
	VAssetWriterReader();
	~VAssetWriterReader();

	VAssetWriterReader(VAssetWriterReader& other) = delete;
	VAssetWriterReader(VAssetWriterReader&& other) = delete;
	VAssetWriterReader operator=(VAssetWriterReader& other) = delete;
	VAssetWriterReader& operator=(VAssetWriterReader&& other) = delete;

	void WriteAsset(const std::wstring& localPath, const std::wstring& data);
	void ReadAsset(const std::wstring& localPath, std::wstring& data);

private:

};
