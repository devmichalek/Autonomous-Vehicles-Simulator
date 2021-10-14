#include "DrawableWorld.hpp"
#include "CoreWindow.hpp"
#include "DrawableVehicle.hpp"
#include "MapPrototype.hpp"
#include "VehicleBuilder.hpp"
#include "SimulatedEdge.hpp"
#include "SimulatedCheckpoint.hpp"
#include "SimulatedVehicle.hpp"
#include <Box2D\box2d.h>

DrawableWorld::DrawableWorld()
{
	m_world = new b2World(b2Vec2(0.0f, 0.0f));
	m_staticWorld = new b2World(b2Vec2(0.0f, 0.0f));
}

DrawableWorld::~DrawableWorld()
{
	delete m_world;
	delete m_staticWorld;

	for (auto & item : m_drawableVector)
	{
		switch (((SimulatedInterface*)item)->GetCategory())
		{
			case SimulatedInterface::CategoryEdge:
					delete[] item;
					break;
			case SimulatedInterface::CategoryCheckpoint:
			case SimulatedInterface::CategoryVehicle:
					delete item;
					break;
		}
	}

	m_drawableVector.clear();
}

bool DrawableWorld::AddMap(MapPrototype* prototype)
{
	if (!m_world)
		return false;

	AddEdgesChain(prototype->GetEdges(), prototype->IsClockwise());
	AddCheckpoints(prototype->GetCheckpoints());
	return true;
}

SimulatedVehicle* DrawableWorld::AddVehicle(VehiclePrototype* prototype)
{
	if (!m_world)
		return nullptr;

	// Create simulated vehicle
	auto simulatedVehicle = new SimulatedVehicle(prototype->GetNumberOfBodyPoints(),
												 prototype->GetSensorPoints(),
												 prototype->GetSensorBeamAngles(),
												 prototype->GetSensorMotionRanges());
	m_drawableVector.push_back(simulatedVehicle);

	// Create polygon shape
	auto& description = prototype->GetBodyPoints();
	const size_t numberOfPoints = description.size();
	b2Vec2* vertices = new b2Vec2[numberOfPoints];
	for (size_t i = 0; i < numberOfPoints; ++i)
	{
		vertices[i] = DrawableMath::ToBox2DPosition(description[i]);
	}
	b2PolygonShape polygonShape;
	polygonShape.Set(vertices, int32(numberOfPoints));
	delete[] vertices;

	// Create fixture definition
	b2FixtureDef fixtureDefinition;
	fixtureDefinition.filter.categoryBits = SimulatedInterface::CategoryVehicle;
	fixtureDefinition.filter.maskBits = SimulatedInterface::CategoryEdge | SimulatedInterface::CategoryCheckpoint;
	fixtureDefinition.shape = &polygonShape;
	fixtureDefinition.userData.pointer = reinterpret_cast<uintptr_t>(simulatedVehicle);
	fixtureDefinition.density = 1.0; // Set max density

	// Create body definition
	b2BodyDef bodyDefinition;
	bodyDefinition.angle = (float)DrawableMath::ToRadians(prototype->GetAngle());
	bodyDefinition.position = DrawableMath::ToBox2DPosition(prototype->GetCenter());
	bodyDefinition.type = b2_dynamicBody;

	// Create body
	b2Body* body = m_world->CreateBody(&bodyDefinition);
	body->CreateFixture(&fixtureDefinition);
	simulatedVehicle->SetBody(body);

	// Set mass data center
	b2MassData massData;
	body->GetMassData(&massData);
	massData.center = b2Vec2(0.f, 0.f);
	body->SetMassData(&massData);

	return simulatedVehicle;
}

void DrawableWorld::AddEdgesChain(const EdgeVector& edgesChain, const bool clockwise)
{
	const size_t numberOfEdges = edgesChain.size();
	const size_t offset = clockwise ? 1 : -1;
	b2Vec2* vertices = new b2Vec2[numberOfEdges];
	SimulatedEdge* simulatedEdge = new SimulatedEdge[numberOfEdges];
	m_drawableVector.push_back(simulatedEdge);
	size_t j = clockwise ? 0 : (numberOfEdges - 1);
	size_t part = clockwise ? 0 : 1;
	for (size_t i = 0; i < numberOfEdges; ++i, j += offset)
	{
		vertices[i] = DrawableMath::ToBox2DPosition(edgesChain[j][part]);
		simulatedEdge[i].SetPosition(edgesChain[j]);
	}

	b2ChainShape chainShape;
	chainShape.CreateLoop(vertices, int32(numberOfEdges));
	delete[] vertices;

	b2BodyDef bodyDefinition;
	bodyDefinition.position = b2Vec2(0.0f, 0.0f);
	bodyDefinition.type = b2_staticBody;

	b2Body* body = m_world->CreateBody(&bodyDefinition);
	b2Body* staticBody = m_staticWorld->CreateBody(&bodyDefinition);
	b2FixtureDef fixtureDefinition;
	fixtureDefinition.filter.categoryBits = SimulatedInterface::CategoryEdge;
	fixtureDefinition.shape = &chainShape;
	fixtureDefinition.userData.pointer = reinterpret_cast<uintptr_t>(simulatedEdge);
	body->CreateFixture(&fixtureDefinition);
	staticBody->CreateFixture(&fixtureDefinition);
}

void DrawableWorld::AddCheckpoints(const TriangleVector& checkpoints)
{
	for (size_t i = 0; i < checkpoints.size(); ++i)
	{
		b2PolygonShape polygonShape;
		b2Vec2 points[3];
		points[0] = DrawableMath::ToBox2DPosition(checkpoints[i][0]);
		points[1] = DrawableMath::ToBox2DPosition(checkpoints[i][1]);
		points[2] = DrawableMath::ToBox2DPosition(checkpoints[i][2]);
		SimulatedCheckpoint* simulatedCheckpoint = new SimulatedCheckpoint(i, checkpoints[i]);
		m_drawableVector.push_back(simulatedCheckpoint);
		polygonShape.Set(points, 3);

		b2BodyDef bodyDefinition;
		bodyDefinition.position = b2Vec2(0.0f, 0.0f);
		bodyDefinition.type = b2_staticBody;

		b2Body* body = m_world->CreateBody(&bodyDefinition);
		b2FixtureDef fixtureDefinition;
		fixtureDefinition.filter.categoryBits = SimulatedInterface::CategoryCheckpoint;
		fixtureDefinition.shape = &polygonShape;
		fixtureDefinition.userData.pointer = reinterpret_cast<uintptr_t>(simulatedCheckpoint);
		fixtureDefinition.isSensor = true;
		body->CreateFixture(&fixtureDefinition);
	}
}

bool DrawableWorld::DrawQueryCallback::ReportFixture(b2Fixture* fixture, int32 childIndex)
{
	switch (fixture->GetFilterData().categoryBits)
	{
		case SimulatedInterface::CategoryEdge:
			((SimulatedEdge*)fixture->GetUserData().pointer)[childIndex].Draw(fixture);
			break;
		case SimulatedInterface::CategoryCheckpoint:
			((SimulatedCheckpoint*)fixture->GetUserData().pointer)->Draw(fixture);
			break;
		case SimulatedInterface::CategoryVehicle:
			((SimulatedVehicle*)fixture->GetUserData().pointer)->Draw(fixture);
			break;
	}
	
	return true; // Return true to continue the query
}
