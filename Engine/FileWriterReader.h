#pragma once

class FileWriterReader
{
public:
	FileWriterReader();
	~FileWriterReader();

	FileWriterReader(FileWriterReader& other) = delete;
	FileWriterReader(FileWriterReader&& other) = delete;
	FileWriterReader operator=(FileWriterReader& other) = delete;
	FileWriterReader& operator=(FileWriterReader&& other) = delete;

	static void WriteToPath(const std::string& path, const std::string& data, std::ios::openmode mode);
	static bool ReadFromPath(const std::string& path, std::string& data, std::ios::openmode mode);
};
