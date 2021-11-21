#include "ActivationFunctionContext.hpp"
#include "CoreLogger.hpp"

ActivationFunction ActivationFunctionContext::m_activationFunctionTable[ACTIVATION_FUNCTIONS_COUNT];
const bool ActivationFunctionContext::m_initialized = false;

void ActivationFunctionContext::Initialize()
{
	if (!m_initialized)
	{
		m_activationFunctionTable[LINEAR_ACTIVATION_FUNCTION] = [](const Neuron input) { return input; };
		m_activationFunctionTable[FAST_SIGMOID_ACTIVATION_FUNCTION] = [](const Neuron input) { return input / (1 + std::fabs(input)); };
		m_activationFunctionTable[RELU_ACTIVATION_FUNCTION] = [](const Neuron input) { return input < 0 ? 0 : input; };
		m_activationFunctionTable[LEAKY_RELU_ACTIVATION_FUNCTION] = [](const Neuron input) { return input >= 0 ? input : input * 0.1; };
		m_activationFunctionTable[TANH_ACTIVATION_FUNCTION] = [](const Neuron input) { return tanh(input); };
		CoreLogger::PrintSuccess("ActivationFunctionContext initialized correctly");
	}
	else
		CoreLogger::PrintError("Activation function context initialization was performed more than once!");
}

Neuron ActivationFunctionContext::Compute(const size_t index, const Neuron neuron)
{
	if (index >= ACTIVATION_FUNCTIONS_COUNT)
	{
		CoreLogger::PrintError("Activation function index is out of range!");
		return m_activationFunctionTable[LINEAR_ACTIVATION_FUNCTION](neuron);
	}

	return m_activationFunctionTable[index](neuron);
}

std::string ActivationFunctionContext::GetString(const size_t index)
{
	switch (index)
	{
		case FAST_SIGMOID_ACTIVATION_FUNCTION:
			return "Fast sigmoid activation function";
		case RELU_ACTIVATION_FUNCTION:
			return "ReLU activation function";
		case LEAKY_RELU_ACTIVATION_FUNCTION:
			return "Leaky ReLU activation function";
		case TANH_ACTIVATION_FUNCTION:
			return "Tanh activation function";
		case LINEAR_ACTIVATION_FUNCTION:
			return "Linear activation function";
	}

	CoreLogger::PrintError("Activation function index is out of range!");
	return GetString(LINEAR_ACTIVATION_FUNCTION);
}
