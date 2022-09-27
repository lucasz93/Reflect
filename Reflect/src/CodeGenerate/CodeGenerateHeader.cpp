#include "CodeGenerate/CodeGenerateHeader.h"
#include "CodeGenerate/CodeGenerate.h"
#include "Instrumentor.h"
#include <assert.h>
#include <filesystem>

namespace Reflect
{
	std::string GetCurrentFileID(const CodeGenerateAddtionalOptions& addtionalOptions, const std::string& fileName)
	{
		auto relative = addtionalOptions.RelativeFilePath;
		std::replace(relative.begin(), relative.end(), '/',  '_');
		std::replace(relative.begin(), relative.end(), '\\', '_');

		return relative + "_" + fileName + "_Source_h";
	}

#define WRITE_CURRENT_FILE_ID(FileName) file << "#define " + GetCurrentFileID(addtionalOptions, FileName)
#define WRITE_CLOSE() file << "\n\n"

#define WRITE_PUBLIC() file << "public:\\\n"
#define WRITE_PRIVATE() file << "private:\\\n"

	void CodeGenerateHeader::GenerateHeader(const FileParsedData& data, std::ostream& file, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		REFLECT_PROFILE_FUNCTION();

		file << " // This file is auto generated please don't modify.\n";

		CodeGenerate::IncludeHeader("ReflectStructs.h", file);
		CodeGenerate::IncludeHeader("Core/Core.h", file);
		CodeGenerate::IncludeHeader("Core/Util.h", file);
		CodeGenerate::IncludeHeader("array", file, true);

		const auto relativePath = data.FilePath + "\\" + std::string(data.FileName + "." + data.FileExtension);
		std::string reflectGuard = relativePath + ReflectFileHeaderGuard;
		std::replace(reflectGuard.begin(), reflectGuard.end(), '/', '_');
		std::replace(reflectGuard.begin(), reflectGuard.end(), '\\', '_');
		std::replace(reflectGuard.begin(), reflectGuard.end(), '.', '_');
		std::replace(reflectGuard.begin(), reflectGuard.end(), ':', '_');
		std::replace(reflectGuard.begin(), reflectGuard.end(), '-', '_');

		file << "\n";
		file << "#ifdef " + reflectGuard + "_h\n";
		file << "#error \"" + reflectGuard + ".h" + " already included, missing 'pragma once' in " + data.FileName + ".h\"\n";
		file << "#endif //" + reflectGuard + "_h\n";
		file << "#define " + reflectGuard + "_h\n\n";

		WriteMacros(data, file, addtionalOptions);
		WriteEnums(data, file, addtionalOptions);
	}

	void CodeGenerateHeader::WriteMacros(const FileParsedData& data, std::ostream& file, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		for (const auto& reflectData : data.ReflectData)
		{
			if (reflectData.ReflectType == ReflectType::Enum)
			{
				continue;
			}

			const std::string CurrentFileId = GetCurrentFileID(addtionalOptions, data.FileName) + "_" + std::to_string(reflectData.ReflectGenerateBodyLine);

			WriteMemberPropertiesOffsets(reflectData, file, CurrentFileId, addtionalOptions);
			WriteStaticClass(reflectData, file, CurrentFileId, addtionalOptions);
			WriteMemberProperties(reflectData, file, CurrentFileId, addtionalOptions);
			WriteFunctions(reflectData, file, CurrentFileId, addtionalOptions);
			WriteFunctionGet(reflectData, file, CurrentFileId, addtionalOptions);
			WriteMemberGet(reflectData, file, CurrentFileId, addtionalOptions);

			std::vector<Reflect::ReflectMemberData> serialiseFields;
			for (const auto& member : reflectData.Members)
			{
				const auto it = std::find_if(member.ContainerProps.begin(), member.ContainerProps.end(), [](const auto& p) { return p == "Serialise"; });
				if (it != member.ContainerProps.end())
				{
					serialiseFields.push_back(member);
				}
			}

			WRITE_CURRENT_FILE_ID(data.FileName) + "_" + std::to_string(reflectData.ReflectGenerateBodyLine) + "_GENERATED_BODY \\\n";
			file << CurrentFileId + "_PROPERTIES_OFFSET \\\n";
			file << CurrentFileId + "_STATIC_CLASS \\\n";
			file << CurrentFileId + "_PROPERTIES \\\n";
			file << CurrentFileId + "_FUNCTION_DECLARE \\\n";
			file << CurrentFileId + "_FUNCTION_GET \\\n";
			file << CurrentFileId + "_PROPERTIES_GET \\\n";

			WRITE_CLOSE();
		}

		file << "#undef CURRENT_FILE_ID\n";
		file << "#define CURRENT_FILE_ID " + GetCurrentFileID(addtionalOptions, data.FileName) + "\n";
	}

	void CodeGenerateHeader::WriteStaticClass(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		file << "#define " + currentFileId + "_STATIC_CLASS \\\n";
		WRITE_PUBLIC();
		if (data.SuperName.length())
			file << "\tusing SuperClass = " + data.SuperName + ";\\\n";
		file << "\tstatic const Reflect::Class StaticClass;\\\n";
		WRITE_CLOSE();
	}

	void CodeGenerateHeader::WriteMemberProperties(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		file << "#define " + currentFileId + "_PROPERTIES \\\n";
		WRITE_PRIVATE();

		if (data.Members.size())
		{
			// Write wrappers for the IO functions.
			// I think these are necessary because they use the __OFFSETOF__ constexprs.
			// The actual property table isn't happy taking a function pointer to the main ReadField+WriteField functions
			// because they need to be instantiated with the __OFFSETOF__ constexprs, but it's happy to have an untemplated
			// function that uses __OFFSETOF__ internally, because it doens't need to evaluate it.
			for (const auto& member : data.Members)
			{
				if (!CodeGenerate::IsSerialised(member))
				{
					continue;
				}
				
				const auto customSerialiser = CodeGenerate::GetCustomSerialiser(member);
				std::string readField, writeField;
				if (customSerialiser.length())
				{
					readField = "ReadCustomField<" + customSerialiser + ", " + member.Type + ", __OFFSETOF__" + member.Name + "()>";
					writeField = "WriteCustomFieldFromPtr<" + customSerialiser + ", " + member.Type + ">";
				}
				else
				{
					readField = "ReadField<" + member.Type + ", __OFFSETOF__" + member.Name + "()>";
					writeField = "WriteFieldFromPtr<" + member.Type + ">";
				}

				file << "\tstatic void __READ__" << member.Name << "(Reflect::IUnserialiser& u, std::istream& in, void* self) { " << readField << "(u, in, self); }\\\n";
				file << "\tstatic void __WRITE__" << member.Name << "(Reflect::ISerialiser& s, std::ostream& out, const void* self) { " << writeField << "(s, out, self); }\\\n";
			}

			//
			// TODO: Find a way to make this constexpr. Seems to not like the __OFFSETOF__[property]() functions. We only know
			//		 member offsets when the class body has been completely defined, which it hasn't when this __REFLECT_MEMBER_PROPS__
			//		 lives within the class body!
			//
			file << "\tstatic inline const std::array<Reflect::ReflectMemberProp, " << data.Members.size() << "> __REFLECT_MEMBER_PROPS__ = {\\\n";
			for (const auto& member : data.Members)
			{
				std::string readField, writeField;
				if (CodeGenerate::IsSerialised(member))
				{
					readField = "__READ__" + member.Name;
					writeField = "__WRITE__" + member.Name;
				}
				else
				{
					readField = writeField = "nullptr";
				}

				file << "\t\tReflect::CreateReflectMemberProp<" + member.Type + ">(\"" + member.Name + "\", Reflect::Util::GetTypeName<" + member.Type + ">(), __OFFSETOF__" + member.Name + "(), " + CodeGenerate::GetMemberProps(member.ContainerProps) + ", " + readField + ", " + writeField + "),\\\n";
			}
			file << "\t};\\\n";
		}

		WRITE_CLOSE();
	}

	void CodeGenerateHeader::WriteMemberPropertiesOffsets(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		file << "#define " + currentFileId + "_PROPERTIES_OFFSET \\\n";
		WRITE_PRIVATE();
		for (const auto& member : data.Members)
		{
			file << "\tstatic constexpr int __OFFSETOF__" + member.Name + "() { return offsetof(" + data.Name + ", " + member.Name + "); }; \\\n";
		}
		WRITE_CLOSE();
	}

	void CodeGenerateHeader::WriteMemberGet(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		file << "#define " + currentFileId + "_PROPERTIES_GET \\\n";
		WRITE_PUBLIC();
		file << "virtual Reflect::ReflectMember GetMember(const std::string_view& memberName) const override;\\\n";
		file << "virtual std::vector<Reflect::ReflectMember> GetMembers(std::vector<std::string> const& flags) const override;\\\n";
		file << "virtual std::vector<Reflect::ReflectMember> GetMembers() const override;\\\n";
		for (const auto& member : data.Members)
		{
			file << "static constexpr const char* " << member.Name << "_name = \"" << member.Name << "\";\\\n";
		}
		WRITE_CLOSE();
	}

	void CodeGenerateHeader::WriteFunctions(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		//TODO: Pass in parameters in someway. Prob need to use templates.

		auto populateArgs = [](const std::vector<ReflectTypeNameData>& args) -> std::string
		{
			std::string returnValue;
			for (const auto& arg : args)
			{
				returnValue += arg.Name + "Arg";
				if (arg != args.back())
				{
					returnValue += ", ";
				}
			}
			return returnValue;
		};

		auto castToType = [](const Reflect::ReflectTypeNameData& arg) -> std::string
		{
			if (arg.ReflectMemberType == Reflect::ReflectMemberType::Pointer)
			{
				return "static_cast<" + arg.Type + ">";
			}
			else if (arg.ReflectMemberType == Reflect::ReflectMemberType::Reference)
			{
				return "*static_cast<" + arg.Type.substr(0, arg.Type.length() - 1) + "*>";
			}
			else
			{
				return "*static_cast<" + arg.Type + "*>";
			}
		};

		file << "#define " + currentFileId + "_FUNCTION_DECLARE \\\n";
		WRITE_PRIVATE();
		for (const auto& func : data.Functions)
		{
			file << "\tstatic Reflect::ReflectReturnCode __REFLECT_FUNC__" + func.Name + "(void* objectPtr, void* returnValuePtr, Reflect::FunctionPtrArgs& functionArgs)\\\n";
			file << "\t{\\\n";
			int functionArgIndex = 0;
			for (const auto& arg : func.Parameters)
			{
				file << "\t\t" + arg.Type + " " + arg.Name + "Arg = " + castToType(arg) + "(functionArgs.GetArg(" + std::to_string(functionArgIndex++) + "));\\\n";
			}
			file << "\t\t" + data.Name + "* ptr = static_cast<" + data.Name + "*>(objectPtr);\\\n";
			if (func.Type != "void")
			{
				file << "\t\t*((" + func.Type + "*)returnValuePtr) = ptr->" + func.Name + "(" + populateArgs(func.Parameters) + ");\\\n";
				// TODO: (01/04/21) Check this cast. If it failed return ReflectFuncReturnCode::CAST_FAILED.
			}
			else
			{
				file << "\t\tptr->" + func.Name + "();\\\n";
			}
			file << "\t\treturn Reflect::ReflectReturnCode::SUCCESS;\\\n";
			file << "\t}\\\n";
		}
		WRITE_CLOSE();
	}

	void CodeGenerateHeader::WriteFunctionGet(const ReflectContainerData& data, std::ostream& file, const std::string& currentFileId, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		file << "#define " + currentFileId + "_FUNCTION_GET \\\n";
		WRITE_PUBLIC();
		file << "\tvirtual Reflect::ReflectFunction GetFunction(const std::string_view &functionName) const override;\\\n";
		WRITE_CLOSE();
	}

	void CodeGenerateHeader::WriteEnums(const FileParsedData& data, std::ostream& file, const CodeGenerateAddtionalOptions& addtionalOptions)
	{
		file << "\n\n";

		for (const auto& reflectData : data.ReflectData)
		{
			if (reflectData.ReflectType != ReflectType::Enum)
			{
				continue;
			}

			if (addtionalOptions.Namespace.length())
			{
				file << "namespace " << addtionalOptions.Namespace << " { enum class " << reflectData.Type << " : " << reflectData.SuperName << "; }\n";
			}

			const std::string typeWithNamespace = addtionalOptions.Namespace.length()
				? addtionalOptions.Namespace + "::" + reflectData.Type
				: reflectData.Type;

			file << "namespace Reflect {\n";

			const auto valueVector = "std::vector<std::pair<std::string, " + typeWithNamespace + ">>";
			const auto valueMap = "std::map<std::string, " + typeWithNamespace + ">";

			file << "template<> inline const " << valueVector << "& EnumValues() { \n";
			file << "\tstatic const " << valueVector << " values{\n";
			for (const auto& c : reflectData.Constants)
			{
				file << "\t\tstd::pair<std::string, " << typeWithNamespace << ">(\"" << c.Name << "\", " << typeWithNamespace << "(" << c.Value << ")),\n";
			}
			file << "\t};\n";
			file << "\treturn values;\n";
			file << "}\n\n";

			file << "template<> inline const " << valueMap << "& EnumMap() { \n";
			file << "\tstatic const " << valueMap << " values{\n";
			for (const auto& c : reflectData.Constants)
			{
				file << "\t\tstd::pair<std::string, " << typeWithNamespace << ">(\"" << c.Name << "\", " << typeWithNamespace << "(" << c.Value << ")),\n";
			}
			file << "\t};\n";
			file << "\treturn values;\n";
			file << "}\n\n";

			file << addtionalOptions.ExportMacro << " const char* EnumToString(" << typeWithNamespace << " x);\n";
			file << addtionalOptions.ExportMacro << " bool StringToEnum(const std::string& str, " << typeWithNamespace << "& x);\n";

			file << "}\n\n";	// Namespace
		}
	}
}