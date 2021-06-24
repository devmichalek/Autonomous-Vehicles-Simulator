#pragma once
#include "SAbstract.hpp"
#include "DWallEditor.hpp"
#include "DCarEditor.hpp"

namespace State
{
	class Editor : public Abstract
	{
		DWallEditor m_wallEditor;
		DCarEditor m_carEditor;

	public:
		Editor(Editor&) = delete;

		Editor()
		{
		}
	};
}
