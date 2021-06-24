#pragma once
#include "SAbstract.hpp"

namespace State
{
	class Training : public Abstract
	{
	public:
		Training(Training&) = delete;

		Training()
		{
		}
	};
}
