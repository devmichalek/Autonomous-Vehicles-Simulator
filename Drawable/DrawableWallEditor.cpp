#include "DrawableWallEditor.hpp"

DrawableWallEditor::DrawableWallEditor()
{

}

DrawableWallEditor::~DrawableWallEditor()
{

}

void DrawableWallEditor::capture()
{

}

void DrawableWallEditor::draw()
{
	for (const auto& i : m_segments)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	if (m_activeSegment)
	{
		m_line[0].position = m_segment[0];
		m_line[1].position = m_segment[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}
}