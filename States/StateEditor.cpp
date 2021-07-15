#include "StateEditor.hpp"
#include "CoreWindow.hpp"

StateEditor::StateEditor()
{

}

StateEditor::~StateEditor()
{

}

void StateEditor::capture()
{
	m_wallEditor.capture();
}

void StateEditor::update()
{
	m_wallEditor.update();
}

void StateEditor::load()
{
	m_wallEditor.load();
}

void StateEditor::draw()
{
	m_wallEditor.draw();
	m_carEditor.draw();
}