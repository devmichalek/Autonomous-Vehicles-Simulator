#pragma once
#include "Neural.hpp"
#include <functional>

using ActivationFunction = std::function<Neuron(Neuron)>;
using ActivationFunctions = std::vector<ActivationFunction>;
using ActivationFunctionIndex = size_t;
using ActivationFunctionIndexes = std::vector<ActivationFunctionIndex>;

class ActivationFunctionContext
{

public:

	enum
	{
		STUB_ACTIVATION_FUNCTION,
		FAST_SIGMOID_ACTIVATION_FUNCTION,
		RELU_ACTIVATION_FUNCTION,
		LEAKY_RELU_ACTIVATION_FUNCTION,
		TANH_ACTIVATION_FUNCTION,
		ACTIVATION_FUNCTIONS_COUNT
	};
	
	static void Initialize();

	static ActivationFunction& Get(size_t index);

	static std::string GetString(size_t index);

	static size_t GetMinActivationFunctionIndex() { return STUB_ACTIVATION_FUNCTION; }

	static size_t GetActivationFunctionsCount() { return ACTIVATION_FUNCTIONS_COUNT; }

private:

	static ActivationFunction m_activationFunctionTable[ACTIVATION_FUNCTIONS_COUNT];

	static bool m_initialized;
};