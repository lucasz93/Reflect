#include "TestStrcuts.h"
#include "Core/Util.h"
#include "Core/FieldIO.h"

Reflect::ReflectMemberProp S::__REFLECT_MEMBER_PROPS__[2] = {
	Reflect::CreateReflectMemberProp<int>("Friends", Reflect::Util::GetTypeName<int>(), __REFLECT__Friends(), {"EditorOnly", "Public"}),
	Reflect::CreateReflectMemberProp<int>("TimeOnline", Reflect::Util::GetTypeName<int>(), __REFLECT__TimeOnline(), {"Public"}),
};

const Reflect::Class S::StaticClass = Reflect::Class("S", nullptr, {}, 2, __REFLECT_MEMBER_PROPS__, 0, nullptr, Reflect::ClassAllocator::Create<S>());

Reflect::ReflectFunction S::GetFunction(const std::string_view &functionName)
{
	return SuperClass::GetFunction(functionName);
}

Reflect::ReflectMember S::GetMember(const std::string_view& memberName)
{
	const Reflect::UnserialiseField* unserialise = nullptr;
	for(const auto& member : __REFLECT_MEMBER_PROPS__)
	{
		if(memberName == member.Name)
		{
			return Reflect::ReflectMember(&member, unserialise, ((char*)this) + member.Offset);
		}
	}
	return SuperClass::GetMember(memberName);
}

std::vector<Reflect::ReflectMember> S::GetMembers(std::vector<std::string> const& flags)
{
	std::vector<Reflect::ReflectMember> members = SuperClass::GetMembers(flags);
	for(auto& member : __REFLECT_MEMBER_PROPS__)
	{
		if(member.ContainsProperty(flags))
		{
			const Reflect::UnserialiseField* unserialise = nullptr;
			members.push_back(Reflect::ReflectMember(&member, unserialise, ((char*)this) + member.Offset));
		}
	}
	return members;
}

void S::Serialise(Reflect::Serialiser &s, std::ostream &out) const {
	SuperClass::Serialise(s, out);
}

void S::Unserialise(Reflect::Unserialiser &u, std::istream &in) {
	SuperClass::Unserialise(u, in);
}

const Reflect::Class Actor::StaticClass = Reflect::Class("Actor", nullptr, {}, 0, nullptr, 0, nullptr, Reflect::ClassAllocator::Create<Actor>());

Reflect::ReflectFunction Actor::GetFunction(const std::string_view &functionName)
{
	return SuperClass::GetFunction(functionName);
}

Reflect::ReflectMember Actor::GetMember(const std::string_view& memberName)
{
	const Reflect::UnserialiseField* unserialise = nullptr;
	return SuperClass::GetMember(memberName);
}

std::vector<Reflect::ReflectMember> Actor::GetMembers(std::vector<std::string> const& flags)
{
	std::vector<Reflect::ReflectMember> members = SuperClass::GetMembers(flags);
	return members;
}

void Actor::Serialise(Reflect::Serialiser &s, std::ostream &out) const {
	SuperClass::Serialise(s, out);
}

void Actor::Unserialise(Reflect::Unserialiser &u, std::istream &in) {
	SuperClass::Unserialise(u, in);
}

Reflect::ReflectMemberProp Player::__REFLECT_MEMBER_PROPS__[2] = {
	Reflect::CreateReflectMemberProp<int>("Friends", Reflect::Util::GetTypeName<int>(), __REFLECT__Friends(), {"EditorOnly", "Public"}),
	Reflect::CreateReflectMemberProp<int>("TimeOnline", Reflect::Util::GetTypeName<int>(), __REFLECT__TimeOnline(), {"Public"}),
};

const Reflect::Class Player::StaticClass = Reflect::Class("Player", &Actor::StaticClass, {}, 2, __REFLECT_MEMBER_PROPS__, 0, nullptr, Reflect::ClassAllocator::Create<Player>());

Reflect::ReflectFunction Player::GetFunction(const std::string_view &functionName)
{
	if(functionName == "GetOnlineFriendsCount")
	{
		return Reflect::ReflectFunction(this, Player::__REFLECT_FUNC__GetOnlineFriendsCount);
	}
	if(functionName == "PrintHelloWorld")
	{
		return Reflect::ReflectFunction(this, Player::__REFLECT_FUNC__PrintHelloWorld);
	}
	if(functionName == "GetId")
	{
		return Reflect::ReflectFunction(this, Player::__REFLECT_FUNC__GetId);
	}
	return SuperClass::GetFunction(functionName);
}

Reflect::ReflectMember Player::GetMember(const std::string_view& memberName)
{
	const Reflect::UnserialiseField* unserialise = nullptr;
	for(const auto& member : __REFLECT_MEMBER_PROPS__)
	{
		if(memberName == member.Name)
		{
			return Reflect::ReflectMember(&member, unserialise, ((char*)this) + member.Offset);
		}
	}
	return SuperClass::GetMember(memberName);
}

std::vector<Reflect::ReflectMember> Player::GetMembers(std::vector<std::string> const& flags)
{
	std::vector<Reflect::ReflectMember> members = SuperClass::GetMembers(flags);
	for(auto& member : __REFLECT_MEMBER_PROPS__)
	{
		if(member.ContainsProperty(flags))
		{
			const Reflect::UnserialiseField* unserialise = nullptr;
			members.push_back(Reflect::ReflectMember(&member, unserialise, ((char*)this) + member.Offset));
		}
	}
	return members;
}

void Player::Serialise(Reflect::Serialiser &s, std::ostream &out) const {
	SuperClass::Serialise(s, out);
}

void Player::Unserialise(Reflect::Unserialiser &u, std::istream &in) {
	SuperClass::Unserialise(u, in);
}

