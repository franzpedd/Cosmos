#include "enpch.h"
#include "FileSystem.h"

std::string FileSystem::ReadTextFromFile(std::string path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		ENGINE_ASSERT(false, "Failed to open %s", path);
	}

	std::ostringstream buffer{};
	file >> buffer.rdbuf();

	file.close();

	return buffer.str();
}

void FileSystem::WriteTextToFile(std::string path, std::string buffer)
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		ENGINE_ASSERT(false, "Failed to open %s", path);
	}

	file << buffer;
	file.close();
}

std::vector<char> FileSystem::ReadBinaryFromFile(std::string path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		ENGINE_ASSERT(false, "Failed to open %s", path);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void FileSystem::WriteBinaryToFile(std::string path, std::vector<uint32_t> bytecode)
{
	std::ofstream file(path, std::ios::out | std::ios::binary);

	if (!file.is_open())
	{
		ENGINE_ASSERT(false, "Failed to open %s", path);
	}

	if (!bytecode.empty())
	{
		file.write(reinterpret_cast<char*>(&bytecode[0]), bytecode.size() * sizeof(bytecode[0]));
	}

	file.close();
}
