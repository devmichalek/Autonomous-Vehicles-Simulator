#pragma once
#include "DrawableMath.hpp"
#include "CoreWindow.hpp"
#include <Box2D\Box2D.h>

class MapPrototype;
class VehiclePrototype;
class SimulatedVehicle;
class DrawableInterface;

// This class is responsible for connection between simulator physics and drawing
// Here we connect Box2D functionality and SFML functionality
// This class can be used only in simulation states (it cannot be used in editor states)
class DrawableWorld
{
public:

	DrawableWorld();

	~DrawableWorld();

	// Update internal world state
	inline void Update()
	{
		const float step = 1.f / 600.f;
		const float elapsedTime = float(CoreWindow::GetElapsedTime());
		static float leftProgress = 0.f;
		float progress = leftProgress;
		while (progress < elapsedTime)
		{
			m_world->Step(step, 8, 8);
			progress += step;
		}
		leftProgress = progress - elapsedTime;
		//m_world->Step(float(CoreWindow::GetElapsedTime()), 8, 3);
	}

	// Draws simulation environment
	inline void Draw()
	{
		b2AABB aabb;
		aabb.lowerBound = DrawableMath::ToBox2DPosition(CoreWindow::GetViewOffset());
		aabb.upperBound = DrawableMath::ToBox2DPosition(CoreWindow::GetViewOffset() + CoreWindow::GetSize());
		m_world->QueryAABB(&m_drawQueryCallback, aabb);
	}

	// Returns static world representation
	inline b2World* GetStaticWorld()
	{
		return m_staticWorld;
	}

	// Sets contact listener
	inline bool SetContactListener(b2ContactListener* contactListener)
	{
		if (!m_world)
			return false;

		m_world->SetContactListener(contactListener);
		return true;
	}

	// Adds map to the world and returns true in case of success
	bool AddMap(MapPrototype* prototype);

	// Adds vehicle to the world and returns simulated vehicle
	SimulatedVehicle* AddVehicle(VehiclePrototype* prototype);

private:

	// Adds edges chain to the world
	void AddEdgesChain(const EdgeVector& edgesChain, const bool clockwise);

	// Adds checkpoints to the world
	void AddCheckpoints(const TriangleVector& checkpoints);

	// Callback called to draw all polygons
	class DrawQueryCallback :
		public b2QueryCallback
	{
	public:

		bool ReportFixture(b2Fixture* fixture, int32 childIndex);
	};
	DrawQueryCallback m_drawQueryCallback;

	b2World* m_world; // World representation
	b2World* m_staticWorld; // Static world representation used as dynamic tree for edges

	std::vector<DrawableInterface*> m_drawableVector;
};