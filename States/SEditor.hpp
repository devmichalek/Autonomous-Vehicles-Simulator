#pragma once
#include "SAbstract.hpp"

namespace State
{
	class Editor : public Abstract
	{
	public:
		Editor(Editor&) = delete;

		Editor()
		{
		}
	};
}
