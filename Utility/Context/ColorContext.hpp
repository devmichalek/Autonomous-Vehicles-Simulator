#pragma once
#include "SFML/Graphics/Color.hpp"

namespace ColorContext
{
	const sf::Color ClearBackground = sf::Color(0xFF, 0xFF, 0xFF, 0);
	const sf::Color DarkBackground = sf::Color(0x19, 0x19, 0x19, 0xFF);
	const sf::Color LightBackground = sf::Color(0xE5, 0xE5, 0xE5, 0xFF);
	const sf::Color BeamBeggining = sf::Color(0xE5, 0xE5, 0xE5, 144);
	const sf::Color BeamEnd = sf::Color(0xE5, 0xE5, 0xE5, 32);
	const sf::Color BeamDisabled = ClearBackground;
	const sf::Color VehicleSensorDefault = sf::Color(0xAA, 0x4A, 0x44, 0xFF);
	const sf::Color VehicleSensorMarked = sf::Color(0x7F, 0xFF, 0x00, 0xFF);
	const sf::Color LeaderVehicle = sf::Color(0x60, 0x83, 0x41, 0xFF);
	const sf::Color ActiveText = sf::Color(0xFF, 0xAA, 0x1D, 0xFF);
	const sf::Color InactiveText = sf::Color(0xC0, 0xC0, 0xC0, 0xFF);
	const sf::Color PassiveText = sf::Color::White;
	const sf::Color ErrorText = sf::Color(0xEE, 0x4B, 0x2B, 0xFF);
	const sf::Color SuccessText = sf::Color(0x22, 0x8B, 0x22, 0xFF);
	const sf::Color EdgeDefault = LightBackground;
	const sf::Color EdgeMarked = ActiveText;
	const sf::Color EdgeRemove = ErrorText;
	const sf::Color NeuronDefault = InactiveText;
	const sf::Color NeuronActive = ActiveText;
	const sf::Color Grid = sf::Color(0, 0, 255, 96);
	const sf::Color WeightDefault = sf::Color(255, 255, 255, 128);
	const sf::Uint8 MaxChannelValue = 255;
	const sf::Uint8 MinChannelValue = 0;

	// Returns the result of two blended colors based on alpha factor
	inline sf::Color BlendColors(sf::Color a, sf::Color b, float alpha)
	{
		sf::Color result;
		result.r = sf::Uint8((1.f - alpha) * float(a.r) + alpha * float(b.r));
		result.g = sf::Uint8((1.f - alpha) * float(a.g) + alpha * float(b.g));
		result.b = sf::Uint8((1.f - alpha) * float(a.b) + alpha * float(b.b));
		return result;
	}

	// Creates color from given channels
	inline sf::Color Create(const sf::Uint8 red, const sf::Uint8 green, const sf::Uint8 blue, const sf::Uint8 alpha)
	{
		return sf::Color(red, green, blue, alpha);
	}

	// Creates color from given color and new alpha channel
	inline sf::Color Create(sf::Color color, const sf::Uint8 alpha)
	{
		color.a = alpha;
		return color;
	}

	// Creates color from given color channel and alpha channel
	inline sf::Color Create(const sf::Uint8 channel, const sf::Uint8 alpha)
	{
		return sf::Color(channel, channel, channel, alpha);
	}
}