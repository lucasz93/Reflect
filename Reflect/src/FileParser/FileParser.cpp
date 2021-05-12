#include "FileParser/FileParser.h"
#include "Instrumentor.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <filesystem>
#include <stack>
#include <assert.h>

namespace Reflect
{
	constexpr int DEFAULT_TYPE_SIZE = 0;

	FileParser::FileParser()
	{ }

	FileParser::~FileParser()
	{ }

	void FileParser::ParseDirectory(const std::string& directory)
	{
		REFLECT_PROFILE_FUNCTION();

		m_filesParsed.clear();
		m_filesToRemove.clear();

		std::filesystem::path dirPath(directory);
		std::error_code err;
		if (!std::filesystem::is_directory(dirPath, err))
		{
			std::cout << err.message() << '\n';
			return;
		}

		for (const auto& f : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string filePath = f.path().u8string();

			if ((f.is_regular_file() || f.is_character_file()) &&
				CheckExtension(filePath, { ".h", ".hpp" }) &&
				!CheckIfAutoGeneratedFile(filePath))
			{
				// TODO thread this. We could load files on more than one thread to speed
				// this up.
				std::cout << "Parsing: " << filePath << std::endl;
				std::ifstream file = OpenFile(filePath);
				FileParsedData data = LoadFile(file);
				data.FileName = f.path().filename().u8string().substr(0, f.path().filename().u8string().find_last_of('.'));
				data.FilePath = f.path().parent_path().u8string();
				m_filesParsed.push_back(data);
				CloseFile(file);
			}
		}


		// All files have been loaded.
		// Now we need to parse them to find all the information we want from them.
		// TODO this could also be threaded.
		for (auto& file : m_filesParsed)
		{
			if (!ParseFile(file))
			{
				m_filesToRemove.push_back(file.FileName);
			}
		}

		for (auto const& fileToRemove : m_filesToRemove)
		{
			auto itr = std::find_if(m_filesParsed.begin(), m_filesParsed.end(), [fileToRemove](FileParsedData const& data)
			{
				return fileToRemove == data.FileName;
			});
			assert(itr != m_filesParsed.end() && "[FileParser::ParseDirectory] Remove file to parse dose not exists.");
			m_filesParsed.erase(itr);
		}
	}

	std::ifstream FileParser::OpenFile(const std::string& filePath)
	{
		std::ifstream file = std::ifstream(filePath);
		assert(file.is_open() && "[FileParser::OpenFile] File is not open.");
		return file;
	}

	void FileParser::CloseFile(std::ifstream& file)
	{
		if (file.is_open())
		{
			file.close();
		}
	}

	bool FileParser::CheckExtension(const std::string& filePath, std::vector<const char*> extensions)
	{
		std::string extension = filePath.substr(filePath.find_last_of('.'));
		for (auto& e : extensions)
		{
			if (e == extension)
			{
				return true;
			}
		}
		return false;
	}

	bool FileParser::CheckIfAutoGeneratedFile(const std::string& filePath)
	{
		return filePath.find(ReflectFileGeneratePrefix) != std::string::npos;
	}

	FileParsedData FileParser::LoadFile(std::ifstream& file)
	{
		FileParsedData data = {};

		file.seekg(0, std::ios::end);
		int fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);
		data.Data = std::string(fileSize, '\0');
		data.Cursor = 0;
		file.read(&data.Data[0], fileSize);

		return data;
	}

	bool FileParser::ParseFile(FileParsedData& fileData)
	{
		REFLECT_PROFILE_FUNCTION();

		bool reflectItem = false;
		while (ReflectContainerHeader(fileData, RefectStructKey, ReflectType::Struct) || ReflectContainerHeader(fileData, RefectClassKey, ReflectType::Class))
		{
			ReflectContainer(fileData);
			reflectItem = true;
		}
		return reflectItem;
	}

	bool FileParser::ReflectContainerHeader(FileParsedData& fileData, const std::string& keyword, const ReflectType type)
	{
		// Check if we can reflect this class/struct. 
		int reflectStart = static_cast<int>(fileData.Data.find(keyword, fileData.Cursor));
		if (reflectStart == std::string::npos)
		{
			// Can't reflect this class/struct. Return.
			return false;
		}

		ReflectContainerData containerData = {};

		containerData.ReflectType = type;
		fileData.Cursor = reflectStart + static_cast<int>(keyword.length()) + 1;
		containerData.ContainerProps = ReflectFlags(fileData);

		if (containerData.ReflectType == ReflectType::Class)
		{
			int newPos = fileData.Data.find("class", fileData.Cursor);
			if (newPos != std::string::npos)
			{
				fileData.Cursor = newPos;
				fileData.Cursor += 5;
			}
			else
			{
				return false;
			}
		}
		else if (containerData.ReflectType == ReflectType::Struct)
		{
			int newPos = fileData.Data.find("struct", fileData.Cursor);
			if (newPos != std::string::npos)
			{
				fileData.Cursor = newPos;
				fileData.Cursor += 6;
			}
			else
			{
				return false;
			}
		}

		// Get the flags passed though the REFLECT macro.
		std::string containerName;
		while (fileData.Data.at(fileData.Cursor) != ':' && fileData.Data.at(fileData.Cursor) != '{' && fileData.Data.at(fileData.Cursor) != '\n')
		{
			if (fileData.Data.at(fileData.Cursor) != ' ')
			{
				containerName += fileData.Data.at(fileData.Cursor);
			}
			++fileData.Cursor;
		}
		containerData.Name = containerName;
		containerData.Type = containerName;
		containerData.TypeSize = DEFAULT_TYPE_SIZE;
		fileData.ReflectData.push_back(containerData);

		return true;
	}

	void FileParser::ReflectContainer(FileParsedData& fileData)
	{
		int endOfContainerCursor = FindEndOfConatiner(fileData);

		// Good, we have a reflected container class/struct.
		// First find out which it is and verify that we are inheriting from "ReflectObject".
		std::stack<char> bracketStack;
		ReflectContainerData& conatinerData = fileData.ReflectData.back();

		int generatedBodyLine = static_cast<int>(fileData.Data.find(ReflectGeneratedBodykey, fileData.GeneratedBodyLineOffset));
		assert(generatedBodyLine != -1 && "[FileParser::ReflectContainer] 'REFLECT_GENERATED_BODY()' is missing from a container.");
		fileData.GeneratedBodyLineOffset = generatedBodyLine + static_cast<int>(strlen(ReflectGeneratedBodykey));
		conatinerData.ReflectGenerateBodyLine = CountNumberOfSinceTop(fileData, generatedBodyLine, '\n') + 1;

		while (true)
		{
			int reflectStart = static_cast<int>(fileData.Data.find(PropertyKey, fileData.Cursor));
			if (reflectStart == static_cast<int>(std::string::npos) || reflectStart > endOfContainerCursor)
			{
				// There are no more properties to reflect or we have found a new container to reflect.
				break;
			}
			fileData.Cursor = reflectStart + static_cast<int>(strlen(PropertyKey));

			// Get the reflect flags.
			auto propFlags = ReflectFlags(fileData);

			// Get the type and name of the property to reflect.
			auto [type, name, isConst] = ReflectTypeAndName(fileData, {});

			char c = FindNextChar(fileData, { ' ', '=' });
			while (c != ';' && c != '(' && c != '\n')
			{
				++fileData.Cursor;
				c = FindNextChar(fileData, { ' ', '=' });
			}

			// Find out if the property is a function or member variable.
			if (c == ';')
			{
				// Member
				// We have found a member variable 
				ReflectMemberData memberData = {};
				memberData.Type = type;
				memberData.Name = name;
				memberData.ReflectMemberType = type.back() == '*' || type.back() == '&' ? (type.back() == '*' ? ReflectMemberType::Pointer : ReflectMemberType::Reference) : ReflectMemberType::Value;
				memberData.TypeSize = DEFAULT_TYPE_SIZE;
				memberData.ContainerProps = propFlags;
				memberData.IsConst = isConst;
				conatinerData.Members.push_back(memberData);
			}
			else if (c == '(')
			{
				ReflectFunctionData funcData = {};
				funcData.Type = type;
				funcData.Name = name;
				funcData.TypeSize = DEFAULT_TYPE_SIZE;
				funcData.ContainerProps = propFlags;
				conatinerData.Functions.push_back(funcData);

				// Function
				ReflectGetFunctionParameters(fileData);
			}
			else if (c == '\n')
			{
				assert(false && "[FileParser::ParseFile] Unknown reflect type. This must be a member variable or function. Make sure ')' or ';' is used before a new line.");
			}

			++fileData.Cursor;
		}
	}

	int FileParser::FindEndOfConatiner(const FileParsedData& fileData)
	{
		int cursor = fileData.Cursor;
		char lastCharacter = '\0';
		char c = '\0';
		while (true)
		{
			if (lastCharacter == '}' && c == ';')
			{
				break;
			}

			if (c != '\t' && c != '\n')
			{
				lastCharacter = c;
			}
			++cursor;
			c = fileData.Data[cursor];
		}
		return cursor;
	}

	std::vector<std::string> FileParser::ReflectFlags(FileParsedData& fileData)
	{
		// Get the flags passed though the REFLECT macro.
		std::string flag;
		std::vector<std::string> flags;

		if (fileData.Data[fileData.Cursor] == '(')
		{
			++fileData.Cursor;
		}

		while (fileData.Data[fileData.Cursor] != ')')
		{
			char c = fileData.Data[fileData.Cursor];
			if (c == ',')
			{
				if (!flag.empty())
				{
					flags.push_back(flag);
				}
				flag = "";
			}
			else
			{
				if (c != ' ' && c != '\t')
				{
					flag += c;
				}
			}
			++fileData.Cursor;
		}
		++fileData.Cursor;
		if (!flag.empty())
		{
			flags.push_back(flag);
		}

		return flags;
	}

	char FileParser::FindNextChar(FileParsedData& fileData, const std::vector<char>& ingoreChars)
	{
		while (std::find(ingoreChars.begin(), ingoreChars.end(), fileData.Data[fileData.Cursor]) != ingoreChars.end())
		{
			++fileData.Cursor;
		}
		return fileData.Data[fileData.Cursor];
	}

	bool FileParser::RefectCheckForEndOfLine(const FileParsedData& fileData)
	{
		char c = fileData.Data[fileData.Cursor];
		if (c == ' ' || c == '(' || c == ';')
		{
			return true;
		}

		return false;
	}

	bool FileParser::ReflectTypeCheck(const std::string& type)
	{
		if (type == "const")
		{
			return true;
		}

		return false;
	}

	void FileParser::ReflectGetFunctionParameters(FileParsedData& fileData)
	{
		fileData.Cursor = static_cast<int>(fileData.Data.find('(', fileData.Cursor));
		++fileData.Cursor;

		ReflectFunctionData& funcData = fileData.ReflectData.back().Functions.back();

		while (fileData.Data[fileData.Cursor] != ')')
		{
			auto [type, name, isConst] = ReflectTypeAndName(fileData, { ',', ')' });
			funcData.Parameters.push_back(
				{
					type,
					name,
					DEFAULT_TYPE_SIZE,
					type.back() == '*' || type.back() == '&' ? (type.back() == '*' ? ReflectMemberType::Pointer : ReflectMemberType::Reference) : ReflectMemberType::Value,
					isConst
				});
		}
	}

	std::tuple<std::string, std::string, bool> FileParser::ReflectTypeAndName(FileParsedData& fileData, const std::vector<char>& endOfLineCharacters)
	{
		std::string type;
		bool typeFound = false;
		std::string name;
		bool nameFound = false;
		bool isConst = false;

		while (true)
		{
			char c = fileData.Data[fileData.Cursor];
			if (c == '}')
			{
				// Break here if we have finished
				break;
			}

			if (RefectCheckForEndOfLine(fileData) || (std::find(endOfLineCharacters.begin(), endOfLineCharacters.end(), c) != endOfLineCharacters.end()))
			{
				if (!typeFound)
				{
					if (!type.empty())
					{
						typeFound = true;
						CheckForConst(fileData, type, typeFound, isConst);
					}
				}
				else if (!nameFound)
				{
					if (!name.empty())
					{
						nameFound = true;
					}
				}
			}
			else if (c != '\n' && c != '\t') /*if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')*/
			{
				if (!typeFound)
				{
					type += c;
				}
				else if (!nameFound)
				{
					name += c;
				}
			}

			if ((typeFound && nameFound))
			{
				break;
			}
			++fileData.Cursor;
		}

		return std::make_tuple<std::string, std::string>(type.c_str(), name.c_str(), isConst);
	}

	void FileParser::CheckForConst(FileParsedData& fileData, std::string& type, bool& typeFound, bool& isConst)
	{
		const int len = 6;
		std::string tmp;
		if (isConst)
		{
			return;
		}

		for (int i = fileData.Cursor - (len - 1); i < fileData.Cursor; ++i)
		{
			if (i < 0)
			{
				break;
			}
			tmp += fileData.Data[i];
		}
		if (tmp == "const")
		{
			type += ' ';
			typeFound = false;
			isConst = true;
			return;
		}

		tmp = "";
		for (int i = fileData.Cursor + 1; i < fileData.Cursor + len; ++i)
		{
			if (i > static_cast<int>(fileData.Data.size()))
			{
				break;
			}
			tmp += fileData.Data[i];
		}
		if (tmp == "const")
		{
			type += ' ' + tmp;
			fileData.Cursor += 5;
			typeFound = false;
			isConst = true;
		}
	}

	int FileParser::CountNumberOfSinceTop(const FileParsedData& fileData, int cursorStart, const char& character)
	{
		int count = 0;
		while (cursorStart > 0)
		{
			//TODO Out of bounds checks.
			if (fileData.Data[cursorStart] == character)
			{
				++count;
			}
			--cursorStart;
		}
		return count;
	}
}
