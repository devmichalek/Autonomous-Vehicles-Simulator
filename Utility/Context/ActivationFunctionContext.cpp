#include "ActivationFunctionContext.hpp"

ActivationFunction ActivationFunctionContext::m_activationFunctionTable[ACTIVATION_FUNCTIONS_COUNT];

void ActivationFunctionContext::Initialize()
{
	m_activationFunctionTable[STUB_ACTIVATION_FUNCTION] = [](Neuron input) { return input; };
	m_activationFunctionTable[FAST_SIGMOID_ACTIVATION_FUNCTION] = [](Neuron input) { return input / (1 + std::fabs(input)); };
	m_activationFunctionTable[RELU_ACTIVATION_FUNCTION] = [](Neuron input) { return input < 0 ? 0 : input; };
	m_activationFunctionTable[LEAKY_RELU_ACTIVATION_FUNCTION] = [](Neuron input) { return input >= 0 ? input : input * 0.1; };
	m_activationFunctionTable[TANH_ACTIVATION_FUNCTION] = [](Neuron input) { return tanh(input); };
}

ActivationFunction& ActivationFunctionContext::Get(size_t index)
{
	if (index >= ACTIVATION_FUNCTIONS_COUNT)
		return m_activationFunctionTable[STUB_ACTIVATION_FUNCTION];
	return m_activationFunctionTable[index];
}

std::string ActivationFunctionContext::GetString(size_t index)
{
	switch (index)
	{
		case FAST_SIGMOID_ACTIVATION_FUNCTION:	return "Fast sigmoid activation function";	break;
		case RELU_ACTIVATION_FUNCTION:			return "ReLu activation function";			break;
		case LEAKY_RELU_ACTIVATION_FUNCTION:	return "Leaky ReLu activation function";	break;
		case TANH_ACTIVATION_FUNCTION:			return "Tanh activation function";			break;
		case STUB_ACTIVATION_FUNCTION:
		default:								return "Stub activation function";			break;
	}
}