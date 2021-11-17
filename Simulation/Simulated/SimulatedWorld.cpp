#include "SimulatedWorld.hpp"
#include "CoreWindow.hpp"
#include "DrawableVehicle.hpp"
#include "MapPrototype.hpp"
#include "VehicleBuilder.hpp"
#include "SimulatedEdge.hpp"
#include "SimulatedCheckpoint.hpp"
#include "SimulatedVehicle.hpp"
#include <Box2D\box2d.h>

SimulatedWorld::SimulatedWorld()
{
	m_world = new b2World(b2Vec2(0.0f, 0.0f));
	m_world->SetContactListener(&m_contactListener);
	m_edgesWorld = new b2World(b2Vec2(0.0f, 0.0f));
}

SimulatedWorld::~SimulatedWorld()
{
	delete m_world;
	delete m_edgesWorld;

	for (auto & item : m_simulatedObjects)
	{
		switch (((SimulatedAbstract*)item)->GetCategory())
		{
			case SimulatedAbstract::CategoryEdge:
				delete[] (SimulatedEdge*)item;
				break;
			case SimulatedAbstract::CategoryCheckpoint:
				delete (SimulatedCheckpoint*)item;
				break;
			case SimulatedAbstract::CategoryVehicle:
				delete (SimulatedVehicle*)item;
				break;
		}
	}

	m_simulatedObjects.clear();
}

void SimulatedWorld::EnableDeathOnEdgeContact()
{
	auto function = [&](b2Contact* contact)
	{
		if (contact->GetFixtureA()->GetFilterData().categoryBits == SimulatedAbstract::CategoryEdge &&
			contact->GetFixtureB()->GetFilterData().categoryBits == SimulatedAbstract::CategoryVehicle)
			((SimulatedVehicle*)contact->GetFixtureB()->GetUserData().pointer)->SetInactive();
	};
	m_contactListener.AddBeginContactFunction(function);
}

void SimulatedWorld::AddMap(MapPrototype* prototype)
{
	AddEdgesChain(prototype->GetInnerEdgesChain());
	AddEdgesChain(prototype->GetOuterEdgesChain());
	AddCheckpoints(prototype->GetCheckpoints());
}

SimulatedVehicle* SimulatedWorld::AddVehicle(VehiclePrototype* prototype)
{
	// Create simulated vehicle
	auto simulatedVehicle = new SimulatedVehicle(prototype->GetNumberOfBodyPoints(),
												 prototype->GetSensorPoints(),
												 prototype->GetSensorBeamAngles(),
												 prototype->GetSensorMotionRanges());
	m_simulatedObjects.push_back(simulatedVehicle);

	// Create polygon shape
	auto& description = prototype->GetBodyPoints();
	const size_t numberOfPoints = description.size();
	b2Vec2* vertices = new b2Vec2[numberOfPoints];
	for (size_t i = 0; i < numberOfPoints; ++i)
	{
		vertices[i] = MathContext::ToBox2DPosition(description[i]);
	}
	b2PolygonShape polygonShape;
	polygonShape.Set(vertices, int32(numberOfPoints));
	delete[] vertices;

	// Create fixture definition
	b2FixtureDef fixtureDefinition;
	fixtureDefinition.filter.categoryBits = SimulatedAbstract::CategoryVehicle;
	fixtureDefinition.filter.maskBits = SimulatedAbstract::CategoryEdge | SimulatedAbstract::CategoryCheckpoint;
	fixtureDefinition.shape = &polygonShape;
	fixtureDefinition.userData.pointer = reinterpret_cast<uintptr_t>(simulatedVehicle);
	fixtureDefinition.density = 1.0; // Set max density

	// Create body definition
	b2BodyDef bodyDefinition;
	bodyDefinition.angle = (float)MathContext::ToRadians(prototype->GetAngle());
	bodyDefinition.position = MathContext::ToBox2DPosition(prototype->GetCenter());
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

void SimulatedWorld::AddEdgesChain(const EdgeVector& edgesChain)
{
	const size_t numberOfEdges = edgesChain.size();
	b2Vec2* vertices = new b2Vec2[numberOfEdges];
	SimulatedEdge* simulatedEdge = new SimulatedEdge[numberOfEdges];
	m_simulatedObjects.push_back(simulatedEdge);
	for (size_t i = 0; i < numberOfEdges; ++i)
	{
		vertices[i] = MathContext::ToBox2DPosition(edgesChain[i][0]);
		simulatedEdge[i].SetPosition(edgesChain[i]);
	}

	b2ChainShape chainShape;
	chainShape.CreateLoop(vertices, int32(numberOfEdges));
	delete[] vertices;

	b2BodyDef bodyDefinition;
	bodyDefinition.position = b2Vec2(0.0f, 0.0f);
	bodyDefinition.type = b2_staticBody;

	b2Body* body = m_world->CreateBody(&bodyDefinition);
	b2Body* staticBody = m_edgesWorld->CreateBody(&bodyDefinition);
	b2FixtureDef fixtureDefinition;
	fixtureDefinition.filter.categoryBits = SimulatedAbstract::CategoryEdge;
	fixtureDefinition.shape = &chainShape;
	fixtureDefinition.userData.pointer = reinterpret_cast<uintptr_t>(simulatedEdge);
	body->CreateFixture(&fixtureDefinition);
	staticBody->CreateFixture(&fixtureDefinition);
}

void SimulatedWorld::AddCheckpoints(const RectangleVector& checkpoints)
{
	for (size_t i = 0; i < checkpoints.size(); ++i)
	{
		b2PolygonShape polygonShape;
		b2Vec2 points[4];
		points[0] = MathContext::ToBox2DPosition(checkpoints[i][0]);
		points[1] = MathContext::ToBox2DPosition(checkpoints[i][1]);
		points[2] = MathContext::ToBox2DPosition(checkpoints[i][2]);
		points[3] = MathContext::ToBox2DPosition(checkpoints[i][3]);
		SimulatedCheckpoint* simulatedCheckpoint = new SimulatedCheckpoint(i, checkpoints[i]);
		m_simulatedObjects.push_back(simulatedCheckpoint);
		polygonShape.Set(points, 4);

		b2BodyDef bodyDefinition;
		bodyDefinition.position = b2Vec2(0.0f, 0.0f);
		bodyDefinition.type = b2_staticBody;

		b2Body* body = m_world->CreateBody(&bodyDefinition);
		b2FixtureDef fixtureDefinition;
		fixtureDefinition.filter.categoryBits = SimulatedAbstract::CategoryCheckpoint;
		fixtureDefinition.shape = &polygonShape;
		fixtureDefinition.userData.pointer = reinterpret_cast<uintptr_t>(simulatedCheckpoint);
		fixtureDefinition.isSensor = true;
		body->CreateFixture(&fixtureDefinition);
	}
}

bool SimulatedWorld::DrawQueryCallback::ReportFixture(b2Fixture* fixture, int32 childIndex)
{
	switch (fixture->GetFilterData().categoryBits)
	{
		case SimulatedAbstract::CategoryEdge:
			((SimulatedEdge*)fixture->GetUserData().pointer)[childIndex].Draw();
			break;
		case SimulatedAbstract::CategoryCheckpoint:
			((SimulatedCheckpoint*)fixture->GetUserData().pointer)->Draw();
			break;
		case SimulatedAbstract::CategoryVehicle:
			((SimulatedVehicle*)fixture->GetUserData().pointer)->Draw();
			break;
	}
	
	return true; // Return true to continue the query
}
