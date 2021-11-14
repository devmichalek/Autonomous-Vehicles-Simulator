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
	auto windowTitle = "Artificial Neural Network Self Driving Vehicles Simulator";
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

	const std::string fragmentShader = \
		"uniform bool invert;" \
		"uniform sampler2D texture;" \
		"" \
		"void main()" \
		"{" \
		"	vec4 ref = texture2D(texture, gl_TexCoord[0].xy) * gl_Color;" \
		"	if (invert)" \
		"	{" \
		"" \
		"		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) - ref;" \
		"		gl_FragColor.a = ref.a;" \
		"	}" \
		"	else" \
		"		gl_FragColor = ref;" \
		"}";

	if (!m_invertColorShader.loadFromMemory(fragmentShader, sf::Shader::Fragment))
	{
		CoreLogger::PrintSuccess("Cannot intialize inverting colors shader!");
	}

	m_invertColorShader.setUniform("texture", sf::Shader::CurrentTexture);
	m_invertColorShader.setUniform("invert", false);

	m_currentRenderColor = 0;
	m_renderWindowColors[0] = sf::Color(0x19, 0x19, 0x19, 0xFF);
	m_renderWindowColors[1] = sf::Color(0xE5, 0xE5, 0xE5, 0xFF);

	// Set window as open
	m_open = true;
	CoreLogger::PrintSuccess("CoreWindow initialized correctly");
}
