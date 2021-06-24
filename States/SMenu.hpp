#pragma once
#include "SAbstract.hpp"

namespace State
{
	class Menu : public Abstract
	{
	public:
		Menu(Menu&) = delete;

		Menu()
		{
		}
	};
}
