#pragma once
#include "Neural.hpp"
#include <functional>

using ActivationFunction = std::function<Neuron(const Neuron)>;
using ActivationFunctions = std::vector<ActivationFunction>;
using ActivationFunctionIndex = size_t;
using ActivationFunctionIndexes = std::vector<ActivationFunctionIndex>;

class ActivationFunctionContext final
{
	enum
	{
		LINEAR_ACTIVATION_FUNCTION,
		FAST_SIGMOID_ACTIVATION_FUNCTION,
		RELU_ACTIVATION_FUNCTION,
		LEAKY_RELU_ACTIVATION_FUNCTION,
		TANH_ACTIVATION_FUNCTION,
		ACTIVATION_FUNCTIONS_COUNT
	};

	static ActivationFunction m_activationFunctionTable[ACTIVATION_FUNCTIONS_COUNT];

	static const bool m_initialized;

public:
	
	static void Initialize();

	static Neuron Compute(const size_t index, const Neuron neuron);

	static std::string GetString(const size_t index);

	static size_t GetMinActivationFunctionIndex()
	{
		return LINEAR_ACTIVATION_FUNCTION;
	}

	static size_t GetActivationFunctionsCount()
	{
		return ACTIVATION_FUNCTIONS_COUNT;
	}
};