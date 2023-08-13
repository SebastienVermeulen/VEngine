#include "pch.h"
#include "FileWriterReader.h"

FileWriterReader::FileWriterReader()
{
}
FileWriterReader::~FileWriterReader()
{
}

void FileWriterReader::WriteToPath(const std::string& path, const std::string& data, std::ios::openmode mode)
{
	std::fstream(path, std::ios::out | mode) << data;
}
bool FileWriterReader::ReadFromPath(const std::string& path, std::string& data, std::ios::openmode mode)
{
	if (!std::filesystem::exists(path))
	{
		return false;
	}
	std::fstream(path, std::ios::in | mode) >> data;
	return true;
}