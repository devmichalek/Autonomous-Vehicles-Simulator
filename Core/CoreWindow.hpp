#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class CoreWindow final
{
	inline static sf::RenderWindow m_renderWindow;
	inline static sf::View m_view;
	inline static sf::Clock m_clock;
	inline static sf::Event m_event;
	inline static bool m_open = false;
	inline static double m_elapsedTime = 0;
	inline static const float m_widthRatio = 0.8333f;
	inline static const float m_screenRatio = 0.5625f;
	inline static const sf::Color m_backgroundColor = sf::Color::Black;

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
	inline static sf::Vector2i GetMousePosition()
	{
		return sf::Mouse::getPosition(m_renderWindow);
	}

	// Returns window position
	inline static sf::Vector2i GetPosition()
	{
		return m_renderWindow.getPosition();
	}

	// Returns window size
	inline static sf::Vector2u GetSize()
	{
		return m_renderWindow.getSize();
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
		m_renderWindow.clear(m_backgroundColor);
	}

	// Displays on window what has been rendered so far
	inline static void Display()
	{
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

	// Returns render window
	inline static sf::RenderWindow& GetRenderWindow()
	{
		return m_renderWindow;
	}

	// Returns current window view
	inline static sf::View& GetView()
	{
		return m_view;
	}

	// Returns current view offset
	inline static sf::Vector2f GetViewOffset()
	{
		auto& size = m_view.getSize();
		return m_view.getCenter() - sf::Vector2f(size.x / 2, size.y / 2);
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
};
