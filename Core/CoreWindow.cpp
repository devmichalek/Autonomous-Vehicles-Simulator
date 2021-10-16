#pragma once
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"

CoreWindow::CoreWindow()
{
	// Find correct window size
	const float widthRatio = 0.8333f;
	const float screenRatio = 0.5625f;
	const float screenWidth = sf::VideoMode::getDesktopMode().width * widthRatio;
	const float screenHeight = screenWidth * screenRatio;
	auto windowSize = sf::VideoMode(unsigned(screenWidth), unsigned(screenHeight));

	// Create window
	auto windowTitle = "Simulator ~ created by Adrian Michalek";
	m_renderWindow.create(windowSize, windowTitle, sf::Style::Close);
	m_view.setSize(sf::Vector2f(screenWidth, screenHeight));
	m_view.setCenter(sf::Vector2f(screenWidth / 2.f, screenHeight / 2.f));
	m_defaultView = m_view;
	m_renderWindow.setView(m_view);
	m_windowSize = sf::Vector2f(screenWidth, screenHeight);

	if (!m_renderTextureBackground.create(m_renderWindow.getSize().x, m_renderWindow.getSize().y))
	{
		CoreLogger::PrintError("Cannot create render texture for the background!");
	}
	else
		m_renderTextureBackground.setSmooth(true);

	if (!m_renderTextureForeground.create(m_renderWindow.getSize().x, m_renderWindow.getSize().y))
	{
		CoreLogger::PrintError("Cannot create render texture for the foreground!");
	}

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
