#ifndef FILE_READER_H
#define FILE_READER_H


#include <fstream>
#include <iostream>
#include <string>


inline std::string ReadFile(const char* filePath)
{
	std::fstream file(filePath, std::ios::in);
	if (!file)
	{
		std::cerr << "FILE_READER::ERROR::ReadFile\n";
		return "";
	}

	std::string result;
	std::string line;

	while (std::getline(file, line))
	{
		result += line + "\n";
	}
	
	file.close();

	return result;
}


#endif // !FILE_READER_H
