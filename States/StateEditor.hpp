#pragma once
#include "StateAbstract.hpp"
#include "DrawableWallEditor.hpp"
#include "DrawableCarEditor.hpp"

class StateEditor : public StateAbstract
{
	DrawableWallEditor m_wallEditor;
	DrawableCarEditor m_carEditor;

public:
	StateEditor(StateEditor&) = delete;

	StateEditor();

	~StateEditor();

	void load();

	void draw();
};
