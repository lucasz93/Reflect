#pragma once

#include "Core/Core.h"
#include "ReflectStructs.h"
#include "CodeGenerate/CodeGenerate.h"
#include<fstream>

namespace Reflect
{
	class CodeGenerateHeader
	{
	public:
		CodeGenerateHeader() { }
		~CodeGenerateHeader() { }

		void GenerateHeader(const FileParsedData& data, std::ostream& file, const CodeGenerateAddtionalOptions& addtionalOptions);

	private:
		void WriteMacros(const FileParsedData& data, std::ostream& file, const CodeGenerateAddtionalOptions& addtionalOptions);
		void WriteStaticClass(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);
		void WriteMemberProperties(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);
		void WriteMemberPropertiesOffsets(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);
		void WriteMemberGet(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);

		void WriteFunctions(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);
		void WriteFunctionGet(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);

		void WriteDataDictionary(const std::vector<Reflect::ReflectMemberData>& serialiseFields, const Reflect::ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);
		void WriteSerialiseMethods(const std::vector<Reflect::ReflectMemberData>& serialiseFields, const Reflect::ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions);
	};
}
