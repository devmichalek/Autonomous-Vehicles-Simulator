#include "ArtificialNeuralNetworkBuilder.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include <fstream>

bool ArtificialNeuralNetworkBuilder::Validate()
{
	if (m_validated)
	{
		m_lastOperationStatus = SUCCESS_VALIDATION_PASSED;
		return true;
	}

	if (m_neuronLayerSizes.size() < NEURAL_MIN_NUMBER_OF_LAYERS)
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_NEURON_LAYERS;
		return false;
	}

	if (m_neuronLayerSizes.size() > NEURAL_MAX_NUMBER_OF_LAYERS)
	{
		m_lastOperationStatus = ERROR_TOO_MANY_NEURON_LAYERS;
		return false;
	}

	// Find number of neurons
	m_numberOfNeurons = 0;
	for (const auto& neuronLayerSize : m_neuronLayerSizes)
	{
		if (neuronLayerSize < NEURAL_MIN_NUMBER_OF_NEURONS_PER_LAYER)
		{
			m_lastOperationStatus = ERROR_TOO_LITTLE_NEURONS_IN_LAYER;
			return false;
		}

		if (neuronLayerSize > NEURAL_MAX_NUMBER_OF_NEURONS_PER_LAYER)
		{
			m_lastOperationStatus = ERROR_TOO_MANY_NEURONS_IN_LAYER;
			return false;
		}

		m_numberOfNeurons += neuronLayerSize;
	}

	// Find number of weights
	m_numberOfWeights = 0;
	size_t numberOfLayers = m_neuronLayerSizes.size();
	for (size_t layerNr = 1; layerNr < numberOfLayers; ++layerNr)
	{
		const size_t numberOfWeights = m_neuronLayerSizes[layerNr - 1] * m_neuronLayerSizes[layerNr];
		m_numberOfWeights += numberOfWeights;
	}

	const size_t requiredNumberOfActivationFunctions = m_neuronLayerSizes.size() - 1;
	if (requiredNumberOfActivationFunctions != m_activationFunctionIndexes.size())
	{
		m_lastOperationStatus = ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS;
		return false;
	}

	for (const auto& activationFunctionIndex : m_activationFunctionIndexes)
	{
		if (activationFunctionIndex < ActivationFunctionContext::STUB_ACTIVATION_FUNCTION ||
			activationFunctionIndex >= ActivationFunctionContext::ACTIVATION_FUNCTIONS_COUNT)
		{
			m_lastOperationStatus = ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX;
			return false;
		}
	}
	

	const size_t requiredLengthOfBiasVector = m_neuronLayerSizes.size() - 1;
	if (requiredLengthOfBiasVector != m_biasVector.size())
	{
		m_lastOperationStatus = ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR;
		return false;
	}

	for (const auto& bias : m_biasVector)
	{
		if (bias < NEURAL_MIN_BIAS)
		{
			m_lastOperationStatus = ERROR_BIAS_IS_LESS_THAN_MINIMUM_ALLOWED;
			return false;
		}

		if (bias > NEURAL_MAX_BIAS)
		{
			m_lastOperationStatus = ERROR_BIAS_IS_GREATER_THAN_MAXIMUM_ALLOWED;
			return false;
		}
	}

	m_lastOperationStatus = SUCCESS_VALIDATION_PASSED;
	m_validated = true;
	return true;
}

ArtificialNeuralNetworkBuilder::ArtificialNeuralNetworkBuilder()
{
	m_operationsMap[ERROR_UNKNOWN] = "Error: last status is unknown!";
	m_operationsMap[SUCCESS_LOAD_COMPLETED] = "Success: correctly opened file!";
	m_operationsMap[SUCCESS_SAVE_COMPLETED] = "Success: correctly saved file!";
	m_operationsMap[SUCCESS_VALIDATION_PASSED] = "Success: validation process has passed with no errors!";
	m_operationsMap[ERROR_TOO_LITTLE_NEURON_LAYERS] = "Error: There are too little neuron layers!";
	m_operationsMap[ERROR_TOO_MANY_NEURON_LAYERS] = "Error: There are too many neuron layers!";
	m_operationsMap[ERROR_TOO_LITTLE_NEURONS_IN_LAYER] = "Error: There are too little neurons in a layer!";
	m_operationsMap[ERROR_TOO_MANY_NEURONS_IN_LAYER] = "Error: There are too many neurons in a layer!";
	m_operationsMap[ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS] = "Error: Incorrect number of activation functions!";
	m_operationsMap[ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR] = "Error: Incorrect length of bias vector!";
	m_operationsMap[ERROR_BIAS_IS_LESS_THAN_MINIMUM_ALLOWED] = "Error: Bias value is less than minimum allowed!";
	m_operationsMap[ERROR_BIAS_IS_GREATER_THAN_MAXIMUM_ALLOWED] = "Error: Bias value is greater than maximum allowed!";
	m_operationsMap[ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX] = "Error: Unknown activation function index!";
	m_operationsMap[ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_READING] = "Error: filename is empty, cannot open file for reading!";
	m_operationsMap[ERROR_CANNOT_OPEN_FILE_FOR_READING] = "Error: cannot open file for reading!";
	m_operationsMap[ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING] = "Error: filename is empty, cannot open file for writing!";
	m_operationsMap[ERROR_CANNOT_OPEN_FILE_FOR_WRITING] = "Error: cannot open file for writing!";

	Clear();
}

ArtificialNeuralNetworkBuilder::~ArtificialNeuralNetworkBuilder()
{
}

void ArtificialNeuralNetworkBuilder::Clear()
{
	m_lastOperationStatus = ERROR_UNKNOWN;
	m_neuronLayerSizes.clear();
	m_activationFunctionIndexes.clear();
	m_biasVector.clear();
	m_numberOfNeurons = 0;
	m_numberOfWeights = 0;
	m_validated = false;
}

bool ArtificialNeuralNetworkBuilder::Load(std::string filename, Neuron* rawData)
{
	Clear();

	// Check if filename is not empty
	if (filename.empty())
	{
		m_lastOperationStatus = ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_READING;
		return false;
	}

	// Check if file can be opened for reading
	std::ifstream input(filename, std::ios::in | std::ios::binary);
	if (!input.is_open())
	{
		m_lastOperationStatus = ERROR_CANNOT_OPEN_FILE_FOR_READING;
		return false;
	}



	return true;
}

bool ArtificialNeuralNetworkBuilder::Save(std::string filename, Neuron* rawData)
{
	// Check if filename is not empty
	if (filename.empty())
	{
		m_lastOperationStatus = ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING;
		return false;
	}

	// Validate
	if (!Validate())
		return false;

	// Check if file can be opened for writing
	std::ofstream output(filename, std::ios::out | std::ios::binary);
	if (!output.is_open())
	{
		m_lastOperationStatus = ERROR_CANNOT_OPEN_FILE_FOR_WRITING;
		return false;
	}

	// Save number of neuron layers
	size_t numberOfLayers = m_neuronLayerSizes.size();
	output.write((const char*)&numberOfLayers, sizeof(size_t));

	// Save each neuron layer size
	for (const auto& neuronLayerSize : m_neuronLayerSizes)
		output.write((const char*)&neuronLayerSize, sizeof(size_t));

	// Save number of activation function indexes
	size_t numberOfActivationFunctionIndexes = m_activationFunctionIndexes.size();
	output.write((const char*)&numberOfActivationFunctionIndexes, sizeof(size_t));

	// Save each activation function index
	for (const auto& activationFunctionIndex : m_activationFunctionIndexes)
		output.write((const char*)&activationFunctionIndex, sizeof(size_t));

	// Save length of bias vector
	size_t lengthOfBiasVector = m_biasVector.size();
	output.write((const char*)&lengthOfBiasVector, sizeof(size_t));

	// Save each bias
	for (const auto& bias : m_biasVector)
		output.write((const char*)&bias, sizeof(double));

	// Save number of weights
	output.write((const char*)&m_numberOfWeights, sizeof(size_t));

	// Save raw data
	if (rawData)
	{

	}
	else
	{
		// Save zeros

	}

	Clear();

	m_lastOperationStatus = SUCCESS_SAVE_COMPLETED;
	return true;
}

std::pair<bool, std::string> ArtificialNeuralNetworkBuilder::GetLastOperationStatus()
{
	std::string message = m_operationsMap[m_lastOperationStatus];
	switch (m_lastOperationStatus)
	{
		case SUCCESS_SAVE_COMPLETED:
		case SUCCESS_LOAD_COMPLETED:
		case SUCCESS_VALIDATION_PASSED:
			return std::make_pair(true, message);
		default:
			return std::make_pair(false, message);
	}

	return std::make_pair(false, message);
}

void ArtificialNeuralNetworkBuilder::SetNeuronLayerSizes(NeuronLayerSizes neuronLayerSizes)
{
	m_neuronLayerSizes = neuronLayerSizes;
}

void ArtificialNeuralNetworkBuilder::SetActivationFunctionIndexes(ActivationFunctionIndexes activationFunctionIndexes)
{
	m_activationFunctionIndexes = activationFunctionIndexes;
}

void ArtificialNeuralNetworkBuilder::SetBiasVector(BiasVector biasVector)
{
	m_biasVector = biasVector;
}

ArtificialNeuralNetwork* ArtificialNeuralNetworkBuilder::Get()
{
	if (!Validate())
		return nullptr;

	return nullptr;
}