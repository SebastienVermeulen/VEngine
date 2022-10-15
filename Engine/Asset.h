#pragma once
class Asset
{
public:
	Asset();
	~Asset();

	Asset(Asset& other) = delete;
	Asset(Asset&& other) = delete;
	Asset operator=(Asset& other) = delete;
	Asset& operator=(Asset&& other) = delete;

	void SetFilePath(const std::wstring& filePath);

protected:
	virtual void WriteToVAsset() const;

	std::wstring m_FilePath; //TO-DO: use the Id instead of this string to denote stuff

private:
	int m_AssetID;
};