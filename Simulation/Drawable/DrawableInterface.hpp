#pragma once

class b2Fixture;

class DrawableInterface
{
public:

	virtual ~DrawableInterface()
	{
	}

	// Draws specific shapes
	virtual void Draw(b2Fixture* fixture) = 0;
};