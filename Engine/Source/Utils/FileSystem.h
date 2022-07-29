#pragma once

#include <string>
#include <vector>

class FileSystem
{
public:

	// reads a text file by it's path
	static std::string ReadTextFromFile(std::string path);

	// writes a buffer text to a file
	static void WriteTextToFile(std::string path, std::string buffer);

	// reads a binary file by it's path
	static std::vector<char> ReadBinaryFromFile(std::string path);

	// writes a buffer binary to a file
	static void WriteBinaryToFile(std::string path, std::vector<uint32_t> bytecode);
};