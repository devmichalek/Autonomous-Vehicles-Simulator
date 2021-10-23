#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <array>
#include <random>

class CoreWindow final
{
	inline static sf::RenderWindow m_renderWindow;
	inline static sf::RenderTexture m_renderTextureBackground;
	inline static sf::RenderTexture m_renderTextureForeground;
	inline static sf::Sprite m_backgroundSprite;
	inline static sf::Sprite m_foregroundSprite;
	inline static sf::View m_view, m_defaultView;
	inline static sf::Clock m_clock;
	inline static sf::Event m_event;
	inline static bool m_open = false;
	inline static double m_elapsedTime = 0.0;
	inline static sf::Vector2f m_windowSize;
	inline static sf::Shader m_invertColorShader;
	inline static size_t m_currentRenderColor;
	inline static std::array<sf::Color, 2> m_renderWindowColors;

	CoreWindow();

public:

	CoreWindow(CoreWindow const&) = delete;

	void operator=(CoreWindow const&) = delete;

	// Returns singleton instance
	static CoreWindow& GetInstance()
	{
		static CoreWindow instance;
		return instance;
	}

	// Creates singleton instance
	inline static void Initialize()
	{
		GetInstance();
	}

	// Returns true if window is open, false otherwise
	inline static bool IsOpen()
	{
		return m_open;
	}

	// Returns mouse position
	inline static sf::Vector2f GetMousePosition()
	{
		auto result = sf::Mouse::getPosition(m_renderWindow);
		return sf::Vector2f(float(result.x), float(result.y));
	}

	// Closes window
	inline static void Close()
	{
		m_renderWindow.close();
		m_open = false;
	}

	// Clears window drawing area with background color
	inline static void Clear()
	{
		m_renderTextureBackground.clear(sf::Color(0xFF, 0xFF, 0xFF, 0));
		m_renderTextureForeground.clear(sf::Color(0xFF, 0xFF, 0xFF, 0));
	}

	// Displays on window what has been rendered so far
	inline static void Display()
	{
		m_renderTextureBackground.display();
		m_renderTextureForeground.display();
		m_renderWindow.clear(m_renderWindowColors[m_currentRenderColor]);
		m_backgroundSprite.setTexture(m_renderTextureBackground.getTexture());
		m_foregroundSprite.setTexture(m_renderTextureForeground.getTexture());
		m_renderWindow.draw(m_backgroundSprite, &m_invertColorShader);
		m_renderWindow.draw(m_foregroundSprite, &m_invertColorShader);
		m_renderWindow.display();
	}

	// Returns true if event occurred, false otherwise
	inline static bool IsEvent()
	{
		return m_renderWindow.pollEvent(m_event);
	}

	// Returns system event
	inline static const sf::Event& GetEvent()
	{
		return m_event;
	}

	// Sets background render texture view
	inline static void SetView(sf::View view)
	{
		m_view = view;
		m_renderTextureBackground.setView(view);
	}

	// Draw drawable on the background
	inline static void Draw(const sf::Drawable& drawable)
	{
		return m_renderTextureBackground.draw(drawable);
	}

	// Draw vertices on the background
	inline static void Draw(const sf::Vertex* vertices, size_t vertexCount, sf::PrimitiveType type)
	{
		return m_renderTextureBackground.draw(vertices, vertexCount, type);
	}

	// Draw drawable on the foreground
	inline static void DrawForeground(const sf::Drawable& drawable)
	{
		return m_renderTextureForeground.draw(drawable);
	}

	// Returns window size
	inline static const sf::Vector2f& GetWindowSize()
	{
		return m_windowSize;
	}

	// Returns window center
	inline static sf::Vector2f GetWindowCenter()
	{
		return GetWindowSize() / 2.0f;
	}

	// Returns view size
	inline static const sf::Vector2f& GetViewSize()
	{
		return m_view.getSize();
	}

	// Returns view center
	inline static const sf::Vector2f& GetViewCenter()
	{
		return m_view.getCenter();
	}

	// Sets view center
	inline static void SetViewCenter(sf::Vector2f center)
	{
		m_view.setCenter(center);
		m_renderTextureBackground.setView(m_view);
	}

	// Returns current window view
	inline static sf::View& GetView()
	{
		return m_view;
	}

	// Returns current view offset
	inline static sf::Vector2f GetViewOffset()
	{
		auto& size = GetViewSize();
		return m_view.getCenter() - sf::Vector2f(size.x / 2, size.y / 2);
	}

	// Sets view zoom
	inline static void SetViewZoom(float zoom)
	{
		m_renderTextureBackground.setView(m_defaultView);
		auto center = m_view.getCenter();
		m_view = m_defaultView;
		m_view.setCenter(center);
		m_view.zoom(zoom);
		m_renderTextureBackground.setView(m_view);
	}

	// Returns elapsed time since last clock reset
	inline static double& GetElapsedTime()
	{
		return m_elapsedTime;
	}

	// Restarts clock
	inline static void RestartClock()
	{
		m_elapsedTime = static_cast<double>(m_clock.restart().asMicroseconds()) / 1000000;
	}

	// Resets to default settings
	inline static void Reset()
	{
		m_view = m_defaultView;
		m_renderTextureBackground.setView(m_defaultView);
	}

	// Returns mersenne twister
	inline static std::mt19937& GetMersenneTwister()
	{
		static std::mt19937 mersenneTwister((std::random_device())());
		return mersenneTwister;
	}

	// Switches display color from dark mode to light mode depending on current mode
	inline static void SwitchDisplayColorMode()
	{
		m_currentRenderColor = !m_currentRenderColor;
		m_invertColorShader.setUniform("invert", static_cast<bool>(m_currentRenderColor));
	}

	// Returns number of display color modes
	inline static std::string GetDisplayColorModeString()
	{
		static std::array<std::string, m_renderWindowColors.size()> strings = { "Dark mode", "Light mode" };
		return strings[m_currentRenderColor];
	}
};
