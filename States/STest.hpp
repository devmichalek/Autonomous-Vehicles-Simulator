#pragma once
#include "SAbstract.hpp"

namespace State
{
	class Test : public Abstract
	{
	public:
		Test(Test&) = delete;

		Test()
		{
		}
	};
}
