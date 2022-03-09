#pragma once

#include "Core/Core.h"
#include "Core/Allocator.h"
#include <string>
#include <algorithm>
#include <typeinfo>
#include <vector>
#include <map>

namespace Reflect
{
	struct IReflect;
	class Class;

	namespace Util
	{
		static std::string ToLower(std::string str)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](char c)
			{
				return static_cast<char>(std::tolower(static_cast<int>(c)));
			});
			return str;
		}

		std::string ValidateTypeName(const std::string& str);
		REFLECT_DLL std::string Demangled(const std::type_info& info);

		//
		// Cross platform name generator.
		//
		namespace detail
		{
			template <typename T>
			struct TypeNameImpl {
				static std::string get() { return Demangled(typeid(T)); }
			};

			template <>
			struct TypeNameImpl<std::string> {
				static std::string get() { return "std::string"; }
			};

			template <>
			struct TypeNameImpl<long long> {
				static std::string get() { return "__int64"; }
			};

			template <>
			struct TypeNameImpl<unsigned long long> {
				static std::string get() { return "unsigned __int64"; }
			};

			template <typename T>
			struct TypeNameImpl<Ref<T>> {
				static std::string get() { return "Ref<" + TypeNameImpl<T>::get() + ">"; }
			};

			template <typename T>
			struct TypeNameImpl<std::vector<T>> {
				static std::string get() { return "std::vector<" + TypeNameImpl<T>::get() + ">"; }
			};

			template <typename T>
			struct TypeNameImpl<std::vector<Ref<T>>> {
				static std::string get() { return "std::vector<Ref<" + TypeNameImpl<T>::get() + ">>"; }
			};

			template <typename K, typename V>
			struct TypeNameImpl<std::map<K, V>> {
				static std::string get() { return "std::map<" + TypeNameImpl<K>::get() + "," + TypeNameImpl<V>::get() + ">"; }
			};

			template <typename T>
			struct TypeNameImpl<T*> {
				static std::string get() { return TypeNameImpl<T>::get() + "*"; }
			};
		}

		template<typename T>
		std::string GetTypeName()
		{
			return detail::TypeNameImpl<T>::get();
		}

		template<typename T>
		std::string GetTypeName(const T& type)
		{
			return ValidateTypeName(GetTypeName<T>());
		}

		// Like std::find_if, but will loop around back to our initial position.
		template<typename T, typename TPred>
		T circular_find_if(const T& current, const T& begin, const T& end, TPred pred)
		{
			auto it = std::find_if(current, end, pred);
			if (it != end)
			{
				return it;
			}

			it = std::find_if(begin, current, pred);
			if (it != current)
			{
				return it;
			}

			return end;
		}

		//
		// Cross platform name generator.
		//
		namespace detail
		{
			template<typename T>
			inline typename std::enable_if<std::is_base_of_v<IReflect, T>, const Class *>::type GetStaticClass()
			{
				return &T::StaticClass;
			}

			template<typename T>
			inline typename std::enable_if<!std::is_pointer_v<T> && !std::is_base_of_v<IReflect, T>, const Class*>::type GetStaticClass()
			{
				return nullptr;
			}

			template<typename T>
			inline typename std::enable_if<std::is_pointer_v<T>, const Class*>::type GetStaticClass()
			{
				return GetStaticClass<typename std::remove_pointer<T>::type>();
			}
		}

		template<typename T>
		const Reflect::Class* GetStaticClass()
		{
			return detail::GetStaticClass<T>();
		}
	}
}
