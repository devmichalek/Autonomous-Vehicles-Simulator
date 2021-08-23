#pragma once
#include "StateInterface.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "DrawableMap.hpp"
#include "DrawableCheckpointMap.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableFilenameText.hpp"
#include <functional>

class StateTesting final :
	public StateInterface
{
	enum
	{
		STOPPED_MODE,
		RUNNING_MODE,
		MODES_COUNT
	};
	std::array<std::string, MODES_COUNT> m_modeStrings;
	size_t m_mode;
	
	enum
	{
		MAP_FILENAME_TYPE,
		ANN_FILENAME_TYPE,
		VEHICLE_FILENAME_TYPE,
		FILENAME_TYPES_COUNT
	};
	std::array<std::string, FILENAME_TYPES_COUNT> m_filenameTypeStrings;
	size_t m_filenameType;

	// Control keys
	std::pair<sf::Keyboard::Key, bool> m_modeKey;
	std::pair<sf::Keyboard::Key, bool> m_filenameTypeKey;

	// Objects of test
	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
	DrawableMap* m_drawableMap;
	DrawableVehicle* m_userVehicle;
	DrawableVehicleFactory m_drawableVehicleFactory;
	DrawableCheckpointMap* m_drawableCheckpointMap;

	// Texts
	DrawableTripleText m_modeText;
	DrawableTripleText m_fitnessText;
	DrawableTripleText m_filenameTypeText;
	DrawableFilenameText<true, false> m_filenameText;
	std::vector<std::function<std::string()>> m_textFunctions;

public:

	StateTesting(const StateTesting&) = delete;

	const StateTesting& operator=(const StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};
