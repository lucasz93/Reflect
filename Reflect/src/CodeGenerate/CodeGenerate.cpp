#include "CodeGenerate/CodeGenerate.h"
#include "CodeGenerate/CodeGenerateHeader.h"
#include "CodeGenerate/CodeGenerateSource.h"
#include "Instrumentor.h"
#include <assert.h>
#include <filesystem>

namespace Reflect
{
	constexpr const char* ContainerPrefix = "ReflectObject";

	CodeGenerate::CodeGenerate()
	{ }

	CodeGenerate::~CodeGenerate()
	{ }

	void CodeGenerate::Reflect(const FileParsedData& data, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		REFLECT_PROFILE_FUNCTION();

		CodeGenerateHeader header;
		CodeGenerateSource source;

		const auto headerPath = data.FilePath + "/" + data.FileName + ReflectFileGeneratePrefix + ".h";
		const auto sourcePath = addtionalOptions.OutputCPPDir + data.SubPath + "/" + data.FileName + ReflectFileGeneratePrefix + ".cpp";

		std::ostringstream sout;

		header.GenerateHeader(data, sout, addtionalOptions);
		WriteIfDifferent(headerPath, sout.str());
		sout.str("");

		source.GenerateSource(data, sout, addtionalOptions);
		WriteIfDifferent(sourcePath, sout.str());
		sout.str("");
	}

	void CodeGenerate::WriteIfDifferent(const std::string& filePath, const std::string& str)
	{
		std::ifstream fin(filePath);
		const std::string existing((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

		if (existing == str)
		{
			// They're the same.
			// Don't write the updated file - changes the timestamp and makes compilers want to recompile them!
			return;
		}

		std::cout << "Writing: " << filePath << std::endl;

		fin.close();
		std::ofstream fout(filePath);
		fout.write(str.data(), str.length());
	}

	void CodeGenerate::IncludeHeader(const std::string& headerToInclude, std::ostream& file, bool windowsInclude)
	{
		if (windowsInclude)
		{
			file << "#include <" + headerToInclude + ">\n";
		}
		else
		{
			file << "#include \"" + headerToInclude + "\"\n";
		}
	}

	std::string CodeGenerate::GetMemberProps(const std::vector<std::string>& flags)
	{
		if (flags.size() == 0)
		{
			return "{ }";
		}

		std::string value;
		value += "{";
		for (auto const& flag : flags)
		{
			if (flag != flags.back())
			{
				value += "\"" + flag + "\"" + ", ";
			}
		}
		value += "\"" + flags.back() + "\"" + "}";
		return value;
	}

	std::string CodeGenerate::GetCustomSerialiser(const Reflect::ReflectMemberData& data)
	{
		const auto it = std::find_if(data.ContainerProps.begin(), data.ContainerProps.end(), [](const auto& p) { return p.find("CustomSerialiser=") == 0; });
		if (it != data.ContainerProps.end())
		{
			return it->substr(it->find('=') + 1);
		}

		return "";
	}

	bool CodeGenerate::IsSerialised(const Reflect::ReflectMemberData& data)
	{
		const auto it = std::find_if(data.ContainerProps.begin(), data.ContainerProps.end(), [](const auto& p) { return p.find("Serialise") == 0; });
		return it != data.ContainerProps.end();
	}
}
