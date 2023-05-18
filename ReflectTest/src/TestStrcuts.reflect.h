 // This file is auto generated please don't modify.
#include "ReflectStructs.h"
#include "Core/Core.h"
#include "Core/Enums.h"
#include "Core/Util.h"
#include <array>

#ifdef C__mechsrc_terapixel_1_Dev_ThirdParty_Reflect_ReflectTest_src_TestStrcuts_h_reflect_h
#error "C__mechsrc_terapixel_1_Dev_ThirdParty_Reflect_ReflectTest_src_TestStrcuts_h_reflect.h already included, missing 'pragma once' in TestStrcuts.h"
#endif //C__mechsrc_terapixel_1_Dev_ThirdParty_Reflect_ReflectTest_src_TestStrcuts_h_reflect_h
#define C__mechsrc_terapixel_1_Dev_ThirdParty_Reflect_ReflectTest_src_TestStrcuts_h_reflect_h

#define _TestStrcuts_Source_h_22_STATIC_ENUM \
public:\
	using ThisClass = E;\
	using SuperClass = Reflect::IEnum;\
	using ValueType = int;\
	constexpr E() {}\
	constexpr E(Values v) : Value(v) {}\
private:\
	static Reflect::Enum::ConstantType __GetValue__(const void* ptr) { return ((E*)ptr)->Value; } \
	static void __SetValue__(void* ptr, Reflect::Enum::ConstantType value) { ((E*)ptr)->Value = (Values)value; } \
public:\
	static inline const Reflect::Enum StaticEnum = Reflect::Enum("E", Reflect::Util::GetTypeName<ValueType>(), \
		{"ValueType=int"}, \
		{ \
			Reflect::EnumConstant("Value1", 0, "Value1", { }), \
			Reflect::EnumConstant("Value2", 1, "Value2", { }), \
			Reflect::EnumConstant("Value3", 10, "Value3", { }), \
			Reflect::EnumConstant("Value4", 11, "Value4", { }), \
		}, \
		__GetValue__, __SetValue__ \
	);\



#define _TestStrcuts_Source_h_22_OPERATORS \
public:\
	constexpr operator Values() const { return Values(Value); }\
	explicit operator bool() const = delete;\
	constexpr bool operator==(Values rhs) const { return Value == rhs; }\
	constexpr bool operator!=(Values rhs) const { return Value != rhs; }\
	constexpr auto& operator=(Values v) { Value = v; return *this; }\
	constexpr auto& operator|=(Values rhs) { Value = (Values)(Value | rhs); return *this; }\
	constexpr auto& operator|=(const E& rhs) { Value = (Values)(Value | rhs.Value); return *this; }\


#define _TestStrcuts_Source_h_22_VALUES \
public:\
	static constexpr std::array<std::pair<std::string_view, Values>, 4> Names{\
		std::pair{ "Value1", Values(0) },\
		std::pair{ "Value2", Values(1) },\
		std::pair{ "Value3", Values(10) },\
		std::pair{ "Value4", Values(11) },\
	};\


#define _TestStrcuts_Source_h_22_MEMBERS \
protected:\
	Values Value = (Values)0;\


#define _TestStrcuts_Source_h_22_METHODS \
public:\
	static const auto & ToString(E v) { return StaticEnum.ToString(v); }\
	const auto& ToString() const { return StaticEnum.ToString(Value); }\
	static auto Parse(const std::string_view& value)		{ return E((Values)const_cast<Reflect::Enum&>(StaticEnum).Parse(value)); }\
	static auto TryParse(const std::string_view& value, E& v)	{ return const_cast<Reflect::Enum&>(StaticEnum).TryParse(value, v.Value); }\
	static auto ContainsProperty(std::vector<std::string> const& flags)	{ return StaticEnum.ContainsProperty(flags); }\
	static auto GetPropertyValue(const std::string_view& flag)	{ return StaticEnum.GetPropertyValue(flag); }\
	static auto TryGetPropertyValue(const std::string_view& flag, std::string_view& value)	{ return StaticEnum.TryGetPropertyValue(flag, value); }\
	const auto& GetConstant() const	{ return StaticEnum.GetConstant(Value); }\
	std::string ToBitfieldString() const {\
		std::string s;\
		for (const auto& it : StaticEnum.Values) {\
			if (Value & it.Value) {\
				s.reserve(s.length() + it.Name.length() + 1);\
				if (s.length()) {\
					s += '|';\
				}\
				s += it.Name;\
			}\
		}\
		return s;\
	}\
	static Values ParseBitfieldString(const std::string_view& values)	{ return (Values)StaticEnum.ParseBitfieldString(values); }\
	ValueType	load() const	{ return Value; }\
	void	store(ValueType v)		{ Value = (Values)v; }\


#define _TestStrcuts_Source_h_22_GENERATED_BODY \
_TestStrcuts_Source_h_22_STATIC_ENUM \
_TestStrcuts_Source_h_22_OPERATORS \
_TestStrcuts_Source_h_22_VALUES \
_TestStrcuts_Source_h_22_MEMBERS \
_TestStrcuts_Source_h_22_METHODS \


#define _TestStrcuts_Source_h_32_PROPERTIES_OFFSET \
private:\
	static constexpr int __OFFSETOF__Friends() { return offsetof(S, Friends); }; \
	static constexpr int __OFFSETOF__TimeOnline() { return offsetof(S, TimeOnline); }; \


#define _TestStrcuts_Source_h_32_STATIC_CLASS \
public:\
	using ThisClass = S;\
	using SuperClass = Reflect::IReflect;\
	static inline const Reflect::Class StaticClass = Reflect::Class( \
		Reflect::Util::GetTypeName<ThisClass>(), \
		nullptr, \
		Reflect::ClassAllocator::Create<S>(), \
		{ }, \
		Reflect::Util::make_vector<Reflect::ReflectMemberProp>( \
			Reflect::CreateReflectMemberProp<int>("Friends", Reflect::Util::GetTypeName<int>(), __OFFSETOF__Friends(), {"EditorOnly", "Public"}, nullptr, nullptr), \
			Reflect::CreateReflectMemberProp<int>("TimeOnline", Reflect::Util::GetTypeName<int>(), __OFFSETOF__TimeOnline(), {"Public"}, nullptr, nullptr)\
		), \
		{}, \
		{} \
	); \
private:\
	static inline const Reflect::LinkClass Linker = StaticClass; \


#define _TestStrcuts_Source_h_32_PROPERTIES \
public:\
	static constexpr const char* nameof_Friends = "Friends";\
	static constexpr const char* nameof_TimeOnline = "TimeOnline";\
private:\


#define _TestStrcuts_Source_h_32_FUNCTION_DECLARE \
private:\


#define _TestStrcuts_Source_h_32_GENERATED_BODY \
_TestStrcuts_Source_h_32_PROPERTIES_OFFSET \
_TestStrcuts_Source_h_32_PROPERTIES \
_TestStrcuts_Source_h_32_FUNCTION_DECLARE \
_TestStrcuts_Source_h_32_STATIC_CLASS \


#define _TestStrcuts_Source_h_53_PROPERTIES_OFFSET \
private:\


#define _TestStrcuts_Source_h_53_STATIC_CLASS \
public:\
	using ThisClass = Actor;\
	using SuperClass = Reflect::IReflect;\
	static inline const Reflect::Class StaticClass = Reflect::Class( \
		Reflect::Util::GetTypeName<ThisClass>(), \
		nullptr, \
		Reflect::ClassAllocator::Create<Actor>(), \
		{ }, \
		{}, \
		{}, \
		{} \
	); \
private:\
	static inline const Reflect::LinkClass Linker = StaticClass; \


#define _TestStrcuts_Source_h_53_PROPERTIES \
public:\
private:\


#define _TestStrcuts_Source_h_53_FUNCTION_DECLARE \
private:\


#define _TestStrcuts_Source_h_53_GENERATED_BODY \
_TestStrcuts_Source_h_53_PROPERTIES_OFFSET \
_TestStrcuts_Source_h_53_PROPERTIES \
_TestStrcuts_Source_h_53_FUNCTION_DECLARE \
_TestStrcuts_Source_h_53_STATIC_CLASS \


#define _TestStrcuts_Source_h_68_PROPERTIES_OFFSET \
private:\
	static constexpr int __OFFSETOF__Id() { return offsetof(Player, Id); }; \
	static constexpr int __OFFSETOF__Friends() { return offsetof(Player, Friends); }; \
	static constexpr int __OFFSETOF__TimeOnline() { return offsetof(Player, TimeOnline); }; \


#define _TestStrcuts_Source_h_68_STATIC_CLASS \
public:\
	using ThisClass = Player;\
	using SuperClass = Actor;\
	static inline const Reflect::Class StaticClass = Reflect::Class( \
		Reflect::Util::GetTypeName<ThisClass>(), \
		&Actor::StaticClass, \
		Reflect::ClassAllocator::Create<Player>(), \
		{"AllPrivate", "ShowInEditorOnly", "EditorOnly"}, \
		Reflect::Util::make_vector<Reflect::ReflectMemberProp>( \
			Reflect::CreateReflectMemberProp<std::string>("Id", Reflect::Util::GetTypeName<std::string>(), __OFFSETOF__Id(), {"EditorOnly", "Public"}, nullptr, nullptr), \
			Reflect::CreateReflectMemberProp<int>("Friends", Reflect::Util::GetTypeName<int>(), __OFFSETOF__Friends(), {"EditorOnly", "Public"}, nullptr, nullptr), \
			Reflect::CreateReflectMemberProp<int>("TimeOnline", Reflect::Util::GetTypeName<int>(), __OFFSETOF__TimeOnline(), {"Public"}, nullptr, nullptr)\
		), \
		Reflect::Util::make_vector<Reflect::ReflectMemberFunction>( \
			Reflect::ReflectMemberFunction("GetOnlineFriendsCount", __REFLECT_FUNC__GetOnlineFriendsCount), \
			Reflect::ReflectMemberFunction("PrintHelloWorld", __REFLECT_FUNC__PrintHelloWorld), \
			Reflect::ReflectMemberFunction("GetId", __REFLECT_FUNC__GetId)\
		), \
		{} \
	); \
private:\
	static inline const Reflect::LinkClass Linker = StaticClass; \


#define _TestStrcuts_Source_h_68_PROPERTIES \
public:\
	static constexpr const char* nameof_Id = "Id";\
	static constexpr const char* nameof_Friends = "Friends";\
	static constexpr const char* nameof_TimeOnline = "TimeOnline";\
private:\


#define _TestStrcuts_Source_h_68_FUNCTION_DECLARE \
private:\
	static Reflect::ReflectReturnCode __REFLECT_FUNC__GetOnlineFriendsCount(void* objectPtr, void* returnValuePtr, Reflect::FunctionPtrArgs& functionArgs)\
	{\
		int const& maxPlayerCountArg = *static_cast<int const*>(functionArgs.GetArg(0));\
		Player* ptr = static_cast<Player*>(objectPtr);\
		*((int*)returnValuePtr) = ptr->GetOnlineFriendsCount(maxPlayerCountArg);\
		return Reflect::ReflectReturnCode::SUCCESS;\
	}\
	static Reflect::ReflectReturnCode __REFLECT_FUNC__PrintHelloWorld(void* objectPtr, void* returnValuePtr, Reflect::FunctionPtrArgs& functionArgs)\
	{\
		Player* ptr = static_cast<Player*>(objectPtr);\
		ptr->PrintHelloWorld();\
		return Reflect::ReflectReturnCode::SUCCESS;\
	}\
	static Reflect::ReflectReturnCode __REFLECT_FUNC__GetId(void* objectPtr, void* returnValuePtr, Reflect::FunctionPtrArgs& functionArgs)\
	{\
		Player* ptr = static_cast<Player*>(objectPtr);\
		*((std::string*)returnValuePtr) = ptr->GetId();\
		return Reflect::ReflectReturnCode::SUCCESS;\
	}\


#define _TestStrcuts_Source_h_68_GENERATED_BODY \
_TestStrcuts_Source_h_68_PROPERTIES_OFFSET \
_TestStrcuts_Source_h_68_PROPERTIES \
_TestStrcuts_Source_h_68_FUNCTION_DECLARE \
_TestStrcuts_Source_h_68_STATIC_CLASS \


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID _TestStrcuts_Source_h
