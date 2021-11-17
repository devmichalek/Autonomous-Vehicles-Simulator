#pragma once
#include "AbstractBuilder.hpp"
#include "MapPrototype.hpp"

class MapBuilder final :
	public AbstractBuilder
{
	enum
	{
		ERROR_VEHICLE_IS_NOT_POSITIONED = LAST_ENUM_OPERATION_INDEX,
		ERROR_VEHICLE_OUTSIDE_MAP_AREA,
		ERROR_VEHICLE_OUTSIDE_ROAD_AREA,
		ERROR_VEHICLE_ANGLE_IS_TOO_LITTLE,
		ERROR_VEHICLE_ANGLE_IS_TOO_LARGE,
		ERROR_EDGES_ARE_NOT_SPECIFIED,
		ERROR_TWO_EDGES_INTERSECTION,
		ERROR_INNER_EDGES_DO_NOT_CREATE_CHAIN,
		ERROR_OUTER_EDGES_DO_NOT_CREATE_CHAIN,
		ERROR_TOO_LITTLE_EDGES_PER_CHAIN,
		ERROR_TOO_MANY_EDGES_PER_CHAIN,
		ERROR_CANNOT_GENERATE_ALL_CHECKPOINTS
	};

	EdgeVector m_innerEdgesChain;
	EdgeVector m_outerEdgesChain;
	bool m_vehiclePositioned;
	sf::Vector2f m_vehicleCenter;
	double m_vehicleAngle;
	static std::pair<sf::Vector2f, sf::Vector2f> m_maxAllowedMapArea;
	static std::pair<sf::Vector2f, sf::Vector2f> m_maxAllowedViewArea;

	class EdgesChainGenerator
	{
		// Returns true if order of edges is clockwise and false if it is counter-clockwise
		static bool IsClockwiseOrder(const EdgeVector& edges);

	public:

		// Return edges chain
		static EdgeVector Generate(const EdgeVector& edges, bool innerCollision);
	};

	class RectangleCheckpointsGenerator
	{
		// Generates line checkpoints
		static EdgeVector GenerateInternal(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain);

	public:

		// Generates rectangle checkpoints for given vector of edges chains
		static RectangleVector Generate(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain);
	};

	// Validates if vehicle position is inside map area
	bool ValidateMapAreaVehiclePosition();

	// Validates if vehicle position is inside road area
	bool ValidateRoadAreaVehiclePosition();

	// Validates if vehicle's angle is correct
	bool ValidateVehicleAngle();

	// Validates number of edges per chain
	bool ValidateNumberOfEdgesPerChain(size_t count);

	// Validates if there is no intersection between inner and outer edges
	bool ValidateEdgesChainsIntersection();

	// Validates if inner and outer edges create chains
	bool ValidatesEdgesChains();

	// Validate checkpoints by creating dummies
	bool ValidateCheckpoints();

	// Validate internal fields
	bool ValidateInternal();

	// Clears internal fields
	void ClearInternal();

	// Loads map from file
	bool LoadInternal(std::ifstream& input);

	// Saves map to file
	bool SaveInternal(std::ofstream& output);

	// Creates dummy map
	void CreateDummyInternal();

public:

	MapBuilder();

	~MapBuilder();

	// Sets intermediate representation of vehicle
	void AddVehicle(double angle, sf::Vector2f center);

	// Adds edges chains to the intermediate representation of them
	inline void AddEdgesChains(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain)
	{
		m_innerEdgesChain.insert(m_innerEdgesChain.end(), innerEdgesChain.begin(), innerEdgesChain.end());
		m_outerEdgesChain.insert(m_outerEdgesChain.end(), outerEdgesChain.begin(), outerEdgesChain.end());
	}

	// Returns intermediate representation of inner edges chain
	inline const EdgeVector& GetInnerEdgesChain() const
	{
		return m_innerEdgesChain;
	}

	// Returns intermediate representation of outer edges chain
	inline const EdgeVector& GetOuterEdgesChain() const
	{
		return m_outerEdgesChain;
	}

	// Returns vehicle start center position
	inline sf::Vector2f GetVehicleCenter()
	{
		return m_vehicleCenter;
	}

	// Returns vehicle start angle
	inline double GetVehicleAngle()
	{
		return m_vehicleAngle;
	}

	// Returns map prototype
	MapPrototype* Get();

	// Returns minimum required number of edges per chain
	inline static size_t GetMinNumberOfEdgesPerChain()
	{
		return 4;
	}

	// Returns maximum required number of edges per chain
	inline static size_t GetMaxNumberOfEdgesPerChain()
	{
		return 512;
	}

	// Returns maximum allowed map area (position + size)
	inline std::pair<sf::Vector2f, sf::Vector2f> GetMaxAllowedMapArea() const
	{
		return m_maxAllowedMapArea;
	}

	// Returns maximum allowed view area (position + size)
	inline std::pair<sf::Vector2f, sf::Vector2f> GetMaxAllowedViewArea() const
	{
		return m_maxAllowedViewArea;
	}

	// Returns minimum vehicle's angle
	inline static double GetMinVehicleAngle()
	{
		return 0.0;
	}

	// Returns maximum vehicle's angle
	inline static double GetMaxVehicleAngle()
	{
		return 360.0;
	}

	// Returns vehicle's angle offset
	inline static double GetVehicleAngleOffset()
	{
		return 10.0;
	}
	
	// Checks if dummy can be created
	static bool Initialize();
};