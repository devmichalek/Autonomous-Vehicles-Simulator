#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

namespace State
{
	class Abstract
	{
		enum Type : size_t
		{
			MENU,
			EDITOR,
			TRAINING,
			TEST,
			COUNT
		};

		inline static Type m_type = MENU;

	public:

		Abstract(Abstract&) = delete;

		Abstract()
		{
		}

		virtual ~Abstract()
		{
		}

		static inline size_t type()
		{
			return static_cast<size_t>(m_type);
		}

		static inline size_t count()
		{
			return static_cast<size_t>(COUNT);
		}

		inline void load()
		{

		}

		inline void draw()
		{

		}
	};
}
