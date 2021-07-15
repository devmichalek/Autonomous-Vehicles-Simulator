#pragma once

class StateAbstract
{
	enum Type : size_t
	{
		MENU,
		EDITOR,
		TRAINING,
		TESTING,
		COUNT
	};

	inline static Type m_type = EDITOR;

public:

	StateAbstract(StateAbstract&) = delete;

	StateAbstract()
	{
	}

	virtual ~StateAbstract()
	{
	}

	static inline size_t type()
	{
		return static_cast<size_t>(m_type);
	}

	static inline size_t count()
	{
		return static_cast<size_t>(COUNT);
	}

	virtual void load() = 0;
	virtual void draw() = 0;
};
