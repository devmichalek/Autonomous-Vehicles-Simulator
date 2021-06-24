#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
class Engine;

class Window final
{
	inline static const float m_screenRatio = 0.5625f;
	inline static bool m_open = false;
	inline static double m_elapsedTime = 0;
	inline static sf::RenderWindow m_renderWindow;
	inline static sf::Clock m_clock;
	inline static sf::Event m_event;
	inline static sf::Color m_backgroundColor = sf::Color::Black;

	Window()
	{
		// Find correct window size
		float m_screenWidth = sf::VideoMode::getDesktopMode().width / 1.2f;
		float m_screenHeight = m_screenWidth * m_screenRatio;
		auto windowSize = sf::VideoMode(unsigned(m_screenWidth), unsigned(m_screenHeight));
		auto windowTitle = "Symulator systemu autonomicznego sterowania pojazdami bazujacego na sztucznej sieci neuronowej";
		m_renderWindow.create(windowSize, windowTitle, sf::Style::Close);
		sf::Image icon;
		if (icon.loadFromFile(""))
			m_renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		m_open = true;
	}

	friend Engine;
	inline static void restartClock()
	{
		m_elapsedTime = static_cast<double>(m_clock.restart().asMicroseconds()) / 1000000;
	}

public:
	static Window& getInstance()
	{
		static Window instance;
		return instance;
	}

	Window(Window const&) = delete;
	void operator=(Window const&) = delete;

	inline static bool isOpen()
	{
		return m_open;
	}

	inline static sf::Vector2i getMousePosition()
	{
		return sf::Mouse::getPosition(m_renderWindow);
	}

	inline static sf::Vector2i getPosition()
	{
		return m_renderWindow.getPosition();
	}

	inline static sf::Vector2u getSize()
	{
		return m_renderWindow.getSize();
	}

	inline static void close()
	{
		m_renderWindow.close();
		m_open = false;
	}

	inline static void clear()
	{
		m_renderWindow.clear(m_backgroundColor);
	}

	inline static void display()
	{
		m_renderWindow.display();
	}

	inline static bool isEvent()
	{
		return m_renderWindow.pollEvent(m_event);
	}

	inline static const sf::Event& getEvent()
	{
		return m_event;
	}

	inline static sf::RenderWindow& getRenderWindow()
	{
		return m_renderWindow;
	}

	inline static double getElapsedTime()
	{
		return m_elapsedTime;
	}
};
