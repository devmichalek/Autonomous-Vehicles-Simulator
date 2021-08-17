#include "ArtificialNeuralNetworkBuilder.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include <fstream>
#include <random>

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfLayers(size_t size)
{
	if (size < NEURAL_MIN_NUMBER_OF_LAYERS)
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_NEURON_LAYERS;
		return false;
	}

	if (size > NEURAL_MAX_NUMBER_OF_LAYERS)
	{
		m_lastOperationStatus = ERROR_TOO_MANY_NEURON_LAYERS;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfNeurons(size_t size)
{
	if (size < NEURAL_MIN_NUMBER_OF_NEURONS_PER_LAYER)
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_NEURONS_IN_LAYER;
		return false;
	}

	if (size > NEURAL_MAX_NUMBER_OF_NEURONS_PER_LAYER)
	{
		m_lastOperationStatus = ERROR_TOO_MANY_NEURONS_IN_LAYER;
		return false;
	}

	return true;
}

void ArtificialNeuralNetworkBuilder::CalculateNumberOfNeurons()
{
	m_numberOfNeurons = 0;
	for (const auto& neuronLayerSize : m_neuronLayerSizes)
		m_numberOfNeurons += neuronLayerSize;
}

void ArtificialNeuralNetworkBuilder::CalculateNumberOfWeights()
{
	m_numberOfWeights = 0;
	size_t numberOfLayers = m_neuronLayerSizes.size();
	for (size_t layerNr = 1; layerNr < numberOfLayers; ++layerNr)
	{
		const size_t numberOfWeights = m_neuronLayerSizes[layerNr - 1] * m_neuronLayerSizes[layerNr];
		m_numberOfWeights += numberOfWeights;
	}
}

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfActivationFunctionIndexes(size_t size)
{
	const size_t requiredNumberOfActivationFunctions = m_neuronLayerSizes.size() - 1;
	if (requiredNumberOfActivationFunctions != size)
	{
		m_lastOperationStatus = ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateActivationFunctionIndex(ActivationFunctionIndex activationFunctionIndex)
{
	if (activationFunctionIndex < ActivationFunctionContext::STUB_ACTIVATION_FUNCTION ||
		activationFunctionIndex >= ActivationFunctionContext::ACTIVATION_FUNCTIONS_COUNT)
	{
		m_lastOperationStatus = ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateBiasVectorLength(size_t length)
{
	const size_t requiredLengthOfBiasVector = m_neuronLayerSizes.size() - 1;
	if (requiredLengthOfBiasVector != length)
	{
		m_lastOperationStatus = ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateBias(Bias bias)
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

	return true;
}

bool ArtificialNeuralNetworkBuilder::Validate()
{
	if (m_validated)
	{
		m_lastOperationStatus = SUCCESS_VALIDATION_PASSED;
		return true;
	}

	if (!ValidateNumberOfLayers(m_neuronLayerSizes.size()))
		return false;

	for (const auto& neuronLayerSize : m_neuronLayerSizes)
	{
		if (!ValidateNumberOfNeurons(neuronLayerSize))
			return false;
	}

	CalculateNumberOfNeurons();

	CalculateNumberOfWeights();

	if (!ValidateNumberOfActivationFunctionIndexes(m_activationFunctionIndexes.size()))
		return false;

	for (const auto& activationFunctionIndex : m_activationFunctionIndexes)
	{
		if (!ValidateActivationFunctionIndex(activationFunctionIndex))
			return false;
	}

	if (!ValidateBiasVectorLength(m_biasVector.size()))
		return false;

	for (const auto& bias : m_biasVector)
	{
		if (!ValidateBias(bias))
			return false;
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
	m_operationsMap[ERROR_NUMBER_OF_WEIGHTS_MISMATCH] = "Error: Number of weights mismatch!";
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
	// Clear data
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

	// Read number of neuron layers
	size_t numberOfLayers = 0;
	input.read((char*)&numberOfLayers, sizeof(numberOfLayers));
	if (!ValidateNumberOfLayers(numberOfLayers))
		return false;

	// Read each neuron layer size
	while (numberOfLayers--)
	{
		NeuronLayerSize neuronLayerSize = 0;
		input.read((char*)&neuronLayerSize, sizeof(neuronLayerSize));
		if (!ValidateNumberOfNeurons(neuronLayerSize))
			return false;
		m_neuronLayerSizes.push_back(neuronLayerSize);
	}

	CalculateNumberOfNeurons();

	CalculateNumberOfWeights();

	// Read number of activation function indexes
	size_t numberOfActivationFunctionIndexes = 0;
	input.read((char*)&numberOfActivationFunctionIndexes, sizeof(numberOfActivationFunctionIndexes));
	if (!ValidateNumberOfActivationFunctionIndexes(numberOfActivationFunctionIndexes))
		return false;

	// Read each activation function index
	while (numberOfActivationFunctionIndexes--)
	{
		ActivationFunctionIndex activationFunctionIndex = ActivationFunctionContext::STUB_ACTIVATION_FUNCTION;
		input.read((char*)&activationFunctionIndex, sizeof(activationFunctionIndex));
		if (!ValidateActivationFunctionIndex(activationFunctionIndex))
			return false;
		m_activationFunctionIndexes.push_back(activationFunctionIndex);
	}

	// Read length of bias vector
	size_t lengthOfBiasVector = m_biasVector.size();
	input.read((char*)&lengthOfBiasVector, sizeof(lengthOfBiasVector));
	if (!ValidateBiasVectorLength(lengthOfBiasVector))
		return false;

	// Read each bias
	while (lengthOfBiasVector--)
	{
		Bias bias = 0.0;
		input.read((char*)&bias, sizeof(bias));
		if (!ValidateBias(bias))
			return false;
		m_biasVector.push_back(bias);
	}

	// Read number of weights
	size_t numberOfWeights = 0;
	input.read((char*)&numberOfWeights, sizeof(numberOfWeights));
	if (numberOfWeights != m_numberOfWeights)
	{
		m_lastOperationStatus = ERROR_NUMBER_OF_WEIGHTS_MISMATCH;
		return false;
	}

	// Read raw data
	if (rawData)
		input.read((char*)rawData, m_numberOfWeights * sizeof(Neuron));

	m_lastOperationStatus = SUCCESS_LOAD_COMPLETED;
	m_validated = true;
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
	output.write((const char*)&numberOfLayers, sizeof(numberOfLayers));

	// Save each neuron layer size
	for (const auto& neuronLayerSize : m_neuronLayerSizes)
		output.write((const char*)&neuronLayerSize, sizeof(neuronLayerSize));

	// Save number of activation function indexes
	size_t numberOfActivationFunctionIndexes = m_activationFunctionIndexes.size();
	output.write((const char*)&numberOfActivationFunctionIndexes, sizeof(numberOfActivationFunctionIndexes));

	// Save each activation function index
	for (const auto& activationFunctionIndex : m_activationFunctionIndexes)
		output.write((const char*)&activationFunctionIndex, sizeof(activationFunctionIndex));

	// Save length of bias vector
	size_t lengthOfBiasVector = m_biasVector.size();
	output.write((const char*)&lengthOfBiasVector, sizeof(lengthOfBiasVector));

	// Save each bias
	for (const auto& bias : m_biasVector)
		output.write((const char*)&bias, sizeof(bias));

	// Save number of weights
	output.write((const char*)&m_numberOfWeights, sizeof(m_numberOfWeights));

	if (rawData)
	{
		// Save raw data
		output.write((const char*)rawData, m_numberOfWeights * sizeof(Neuron));
	}
	else
	{
		// Save zeros
		for (size_t i = 0; i < m_numberOfWeights; ++i)
		{
			Neuron neuron = 0.0;
			output.write((const char*)&neuron, sizeof(neuron));
		}
	}

	// Clear data
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

bool ArtificialNeuralNetworkBuilder::CreateDummy()
{
	// Clear data
	Clear();

	// Rand dummy data
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<std::mt19937::result_type> ldistribution(NEURAL_MIN_NUMBER_OF_LAYERS,
																		   NEURAL_MAX_NUMBER_OF_LAYERS);
	std::uniform_int_distribution<std::mt19937::result_type> nlsdistribution(NEURAL_MIN_NUMBER_OF_NEURONS_PER_LAYER,
																			 NEURAL_MAX_NUMBER_OF_NEURONS_PER_LAYER);
	std::uniform_int_distribution<std::mt19937::result_type> afdistribution(ActivationFunctionContext::STUB_ACTIVATION_FUNCTION,
																			ActivationFunctionContext::ACTIVATION_FUNCTIONS_COUNT - 1);
	// Set dummy data
	size_t length = ldistribution(engine);
	m_neuronLayerSizes.resize(length);
	for (size_t i = 0; i < length; ++i)
		m_neuronLayerSizes[i] = nlsdistribution(engine);
	--length;
	m_activationFunctionIndexes.resize(length);
	m_biasVector.resize(length, 0.0);
	for (size_t i = 0; i < length; ++i)
		m_activationFunctionIndexes[i] = afdistribution(engine);

	// Validate
	return Validate();
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

NeuronLayerSizes ArtificialNeuralNetworkBuilder::GetNeuronLayerSizes()
{
	return m_neuronLayerSizes;
}

ActivationFunctionIndexes ArtificialNeuralNetworkBuilder::GetActivationFunctionIndexes()
{
	return m_activationFunctionIndexes;
}

BiasVector ArtificialNeuralNetworkBuilder::GetBiasVector()
{
	return m_biasVector;
}

ArtificialNeuralNetwork* ArtificialNeuralNetworkBuilder::Get()
{
	if (!Validate())
		return nullptr;

	auto* ann = new ArtificialNeuralNetwork;

	const size_t layersCount = m_neuronLayerSizes.size();

	// Set neuron layers
	ann->m_neuronLayers.resize(layersCount);
	for (size_t i = 0; i < layersCount; ++i)
		ann->m_neuronLayers[i].resize(m_neuronLayerSizes[i], 0.0);

	// Set number of neurons
	ann->m_numberOfNeurons = m_numberOfNeurons;

	// Set weights
	ann->m_weightLayers.resize(layersCount - 1);
	for (size_t i = 1; i < layersCount; ++i)
	{
		ann->m_weightLayers[i - 1].resize(m_neuronLayerSizes[i - 1]);
		size_t connectionsCount = ann->m_neuronLayers[i - 1].size();
		for (auto& weights : ann->m_weightLayers[i - 1])
			weights.resize(connectionsCount, 0.0);
	}

	// Set number of weights
	ann->m_numberOfWeights = m_numberOfWeights;

	// Set bias vector
	ann->m_biasVector = m_biasVector;

	// Set activation functions
	ann->m_activationFunctions.resize(layersCount - 1);
	for (size_t i = 0; i < layersCount - 1; ++i)
		ann->m_activationFunctions[i] = ActivationFunctionContext::Get(m_activationFunctionIndexes[i]);

	return ann;
}