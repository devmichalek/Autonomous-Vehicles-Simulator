#pragma once
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"

CoreWindow::CoreWindow()
{
	// Find correct window size
	const float m_screenWidth = sf::VideoMode::getDesktopMode().width * m_widthRatio;
	const float m_screenHeight = m_screenWidth * m_screenRatio;
	auto windowSize = sf::VideoMode(unsigned(m_screenWidth), unsigned(m_screenHeight));

	// Create window
	auto windowTitle = "Simulator";
	m_renderWindow.create(windowSize, windowTitle, sf::Style::Close);
	m_view.setSize(sf::Vector2f(m_screenWidth, m_screenHeight));
	m_view.setCenter(sf::Vector2f(m_screenWidth / 2, m_screenHeight / 2));
	m_renderWindow.setView(m_view);

	// Load window icon
	sf::Image icon;
	std::string filename = "Data/icon.png";
	if (icon.loadFromFile(filename))
		m_renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	else
		CoreLogger::PrintWarning("Cannot load window icon \"" + filename + "\"");

	// Set window as open
	m_open = true;
	CoreLogger::PrintSuccess("Window initialized correctly");
}
