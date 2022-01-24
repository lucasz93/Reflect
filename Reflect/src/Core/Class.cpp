#include "ReflectStructs.h"

#include <map>

namespace Reflect
{
	typedef std::map<std::string, Class*> class_map_t;

	// Static initialisation order is important, but can't be guaranteed.
	// So we allocate stack memory for the map, and manually invoke the class constructor when its used.
	// https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter
	static typename std::aligned_storage<sizeof(class_map_t), alignof (class_map_t)>::type s_class_map_buffer;

	static int s_class_map_counter = 0;
	static class_map_t& s_classes = reinterpret_cast<class_map_t&> (s_class_map_buffer);

	REFLECT_DLL void Class::Register(Class* c)
	{
		if (s_class_map_counter++ == 0)
		{
			new (&s_classes) class_map_t();
		}

		s_classes[c->m_name] = c;
	}

	REFLECT_DLL Class* Class::Lookup(const char* name)
	{
		auto it = s_classes.find(name);
		return it != s_classes.end() ? it->second : nullptr;
	}

	REFLECT_DLL void Class::Unregister(Class* c)
	{
		s_classes.erase(c->m_name);

		if (--s_class_map_counter == 0)
		{
			s_classes.~map();
		}
	}
}
