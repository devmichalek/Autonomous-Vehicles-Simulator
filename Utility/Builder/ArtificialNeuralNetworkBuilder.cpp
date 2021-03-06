#include "ArtificialNeuralNetworkBuilder.hpp"
#include "VehicleBuilder.hpp"
#include "CoreLogger.hpp"
#include <random>

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfLayers(const size_t size)
{
	if (size < GetMinNumberOfLayers())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_NEURON_LAYERS;
		return false;
	}

	if (size > GetMaxNumberOfLayers())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_NEURON_LAYERS;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfNeurons(const size_t size)
{
	if (size < GetMinNumberOfNeuronsPerLayer())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_NEURONS_IN_LAYER;
		return false;
	}

	if (size > GetMaxNumberOfNeuronsPerLayer())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_NEURONS_IN_LAYER;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfNeuronsInOutputLayer(const size_t size)
{
	if (size != VehicleBuilder::GetDefaultNumberOfInputs())
	{
		m_lastOperationStatus = ERROR_DIFFERENT_NUMBER_OF_NEURONS_IN_OUTPUT_LAYER;
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
	const size_t numberOfLayers = m_neuronLayerSizes.size();
	for (size_t layerNr = 1; layerNr < numberOfLayers; ++layerNr)
	{
		const size_t numberOfWeights = m_neuronLayerSizes[layerNr - 1] * m_neuronLayerSizes[layerNr];
		m_numberOfWeights += numberOfWeights;
	}
}

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfActivationFunctionIndexes(const size_t size)
{
	const size_t requiredNumberOfActivationFunctions = m_neuronLayerSizes.size() - 1;
	if (requiredNumberOfActivationFunctions != size)
	{
		m_lastOperationStatus = ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateActivationFunctionIndex(const ActivationFunctionIndex activationFunctionIndex)
{
	if (activationFunctionIndex >= ActivationFunctionContext::GetActivationFunctionsCount())
	{
		m_lastOperationStatus = ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateBiasVectorLength(const size_t length)
{
	const size_t requiredLengthOfBiasVector = m_neuronLayerSizes.size() - 1;
	if (requiredLengthOfBiasVector != length)
	{
		m_lastOperationStatus = ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateBias(const Bias bias)
{
	if (bias < GetMinBiasValue())
	{
		m_lastOperationStatus = ERROR_BIAS_IS_LESS_THAN_MINIMUM_ALLOWED;
		return false;
	}

	if (bias > GetMaxBiasValue())
	{
		m_lastOperationStatus = ERROR_BIAS_IS_GREATER_THAN_MAXIMUM_ALLOWED;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateRawData()
{
	if (m_rawData.size() != m_numberOfWeights)
	{
		m_lastOperationStatus = ERROR_RAW_DATA_VECTOR_LENGTH_IS_INCORRECT;
		return false;
	}

	return true;
}

bool ArtificialNeuralNetworkBuilder::ValidateInternal()
{
	if (!ValidateNumberOfLayers(m_neuronLayerSizes.size()))
		return false;

	for (const auto& neuronLayerSize : m_neuronLayerSizes)
	{
		if (!ValidateNumberOfNeurons(neuronLayerSize))
			return false;
	}

	if (!ValidateNumberOfNeuronsInOutputLayer(m_neuronLayerSizes.back()))
		return false;

	CalculateNumberOfNeurons();

	CalculateNumberOfWeights();

	if (m_rawData.empty())
		m_rawData.resize(m_numberOfWeights, GetMaxNeuronValue());

	if (!ValidateRawData())
		return false;

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

	return true;
}

void ArtificialNeuralNetworkBuilder::ClearInternal()
{
	m_neuronLayerSizes.clear();
	m_activationFunctionIndexes.clear();
	m_biasVector.clear();
	m_rawData.clear();
	m_numberOfNeurons = 0;
	m_numberOfWeights = 0;
}

bool ArtificialNeuralNetworkBuilder::LoadInternal(std::ifstream& input)
{
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

	if (!ValidateNumberOfNeuronsInOutputLayer(m_neuronLayerSizes.back()))
		return false;

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
		ActivationFunctionIndex activationFunctionIndex = ActivationFunctionContext::GetMinActivationFunctionIndex();
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
	m_rawData.resize(m_numberOfWeights);
	for (size_t i = 0; i < m_numberOfWeights; ++i)
		input.read((char*)&m_rawData[i], sizeof(Neuron));

	return true;
}

bool ArtificialNeuralNetworkBuilder::SaveInternal(std::ofstream& output)
{
	// Save number of neuron layers
	const size_t numberOfLayers = m_neuronLayerSizes.size();
	output.write((const char*)&numberOfLayers, sizeof(numberOfLayers));

	// Save each neuron layer size
	for (const auto& neuronLayerSize : m_neuronLayerSizes)
		output.write((const char*)&neuronLayerSize, sizeof(neuronLayerSize));

	// Save number of activation function indexes
	const size_t numberOfActivationFunctionIndexes = m_activationFunctionIndexes.size();
	output.write((const char*)&numberOfActivationFunctionIndexes, sizeof(numberOfActivationFunctionIndexes));

	// Save each activation function index
	for (const auto& activationFunctionIndex : m_activationFunctionIndexes)
		output.write((const char*)&activationFunctionIndex, sizeof(activationFunctionIndex));

	// Save length of bias vector
	const size_t lengthOfBiasVector = m_biasVector.size();
	output.write((const char*)&lengthOfBiasVector, sizeof(lengthOfBiasVector));

	// Save each bias
	for (const auto& bias : m_biasVector)
		output.write((const char*)&bias, sizeof(bias));

	// Save number of weights
	output.write((const char*)&m_numberOfWeights, sizeof(m_numberOfWeights));

	// Save raw data
	for (auto & data : m_rawData)
		output.write((const char*)&data, sizeof(Neuron));

	return true;
}

void ArtificialNeuralNetworkBuilder::CreateDummyInternal()
{
	// Rand dummy data
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<std::mt19937::result_type> ldistribution((unsigned)GetMinNumberOfLayers(),
																		   (unsigned)GetMaxNumberOfLayers());
	std::uniform_int_distribution<std::mt19937::result_type> nlsdistribution((unsigned)GetMinNumberOfNeuronsPerLayer(),
																			 (unsigned)GetMaxNumberOfNeuronsPerLayer());
	std::uniform_int_distribution<std::mt19937::result_type> afdistribution((unsigned)ActivationFunctionContext::GetMinActivationFunctionIndex(),
																			(unsigned)ActivationFunctionContext::GetActivationFunctionsCount() - 1);

	// Set dummy data
	size_t length = ldistribution(engine);
	m_neuronLayerSizes.resize(length);
	for (size_t i = 0; i < length - 1; ++i)
		m_neuronLayerSizes[i] = nlsdistribution(engine);
	m_neuronLayerSizes.back() = VehicleBuilder::GetDefaultNumberOfInputs();
	--length;
	m_activationFunctionIndexes.resize(length);
	m_biasVector.resize(length, 0.0);
	for (size_t i = 0; i < length; ++i)
		m_activationFunctionIndexes[i] = afdistribution(engine);
}

ArtificialNeuralNetworkBuilder::ArtificialNeuralNetworkBuilder() :
	AbstractBuilder(),
	m_numberOfNeurons(0),
	m_numberOfWeights(0)
{
	m_operationsMap.insert(std::pair(ERROR_TOO_LITTLE_NEURON_LAYERS, "Error: There are too little neuron layers!"));
	m_operationsMap.insert(std::pair(ERROR_TOO_MANY_NEURON_LAYERS, "Error: There are too many neuron layers!"));
	m_operationsMap.insert(std::pair(ERROR_TOO_LITTLE_NEURONS_IN_LAYER, "Error: There are too little neurons in a layer!"));
	m_operationsMap.insert(std::pair(ERROR_DIFFERENT_NUMBER_OF_NEURONS_IN_OUTPUT_LAYER, "Error: Number of neurons in output layer must be equal to three!"));
	m_operationsMap.insert(std::pair(ERROR_TOO_MANY_NEURONS_IN_LAYER, "Error: There are too many neurons in a layer!"));
	m_operationsMap.insert(std::pair(ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS, "Error: Incorrect number of activation functions!"));
	m_operationsMap.insert(std::pair(ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR, "Error: Incorrect length of bias vector!"));
	m_operationsMap.insert(std::pair(ERROR_BIAS_IS_LESS_THAN_MINIMUM_ALLOWED, "Error: Bias value is less than minimum allowed!"));
	m_operationsMap.insert(std::pair(ERROR_BIAS_IS_GREATER_THAN_MAXIMUM_ALLOWED, "Error: Bias value is greater than maximum allowed!"));
	m_operationsMap.insert(std::pair(ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX, "Error: Unknown activation function index!"));
	m_operationsMap.insert(std::pair(ERROR_NUMBER_OF_WEIGHTS_MISMATCH, "Error: Number of weights mismatch!"));
	m_operationsMap.insert(std::pair(ERROR_RAW_DATA_VECTOR_LENGTH_IS_INCORRECT, "Error: Raw data vector length is incorrect!"));
	Clear();
}

void ArtificialNeuralNetworkBuilder::Set(ArtificialNeuralNetwork* artificialNeuralNetwork)
{
	// Get number of neuron layers
	const size_t layersCount = artificialNeuralNetwork->m_neuronLayers.size();

	// Set neuron layers sizes
	m_neuronLayerSizes.resize(layersCount);
	for (size_t i = 0; i < layersCount; ++i)
		m_neuronLayerSizes[i] = artificialNeuralNetwork->m_neuronLayers[i].size();

	// Set number of neurons
	m_numberOfNeurons = artificialNeuralNetwork->m_numberOfNeurons;

	// Set number of weights
	m_numberOfWeights = artificialNeuralNetwork->m_numberOfWeights;

	// Set bias vector
	m_biasVector = artificialNeuralNetwork->m_biasVector;

	// Set activation function indexes
	m_activationFunctionIndexes = artificialNeuralNetwork->m_activationFunctionIndexes;

	// Set raw data
	m_rawData.resize(m_numberOfWeights);
	artificialNeuralNetwork->GetRawData(&m_rawData[0]);
}

ArtificialNeuralNetwork* ArtificialNeuralNetworkBuilder::Get()
{
	if (!Validate())
		return nullptr;

	// Create artificial neural network
	auto* artificialNeuralNetwork = new ArtificialNeuralNetwork;

	// Get number of neuron layers
	const size_t layersCount = m_neuronLayerSizes.size();

	// Set neuron layers
	artificialNeuralNetwork->m_neuronLayers.resize(layersCount);
	for (size_t i = 0; i < layersCount; ++i)
		artificialNeuralNetwork->m_neuronLayers[i].resize(m_neuronLayerSizes[i], 0.0);

	// Set number of neurons
	artificialNeuralNetwork->m_numberOfNeurons = m_numberOfNeurons;

	// Set weights
	artificialNeuralNetwork->m_weightLayers.resize(layersCount - 1);
	for (size_t i = 1; i < layersCount; ++i)
	{
		artificialNeuralNetwork->m_weightLayers[i - 1].resize(m_neuronLayerSizes[i]);
		size_t connectionsCount = artificialNeuralNetwork->m_neuronLayers[i - 1].size();
		for (auto& weights : artificialNeuralNetwork->m_weightLayers[i - 1])
			weights.resize(connectionsCount, 0.0);
	}

	// Set number of weights
	artificialNeuralNetwork->m_numberOfWeights = m_numberOfWeights;

	// Set bias vector
	artificialNeuralNetwork->m_biasVector = m_biasVector;

	// Set activation function indexes
	artificialNeuralNetwork->m_activationFunctionIndexes = m_activationFunctionIndexes;

	return artificialNeuralNetwork;
}

ArtificialNeuralNetwork* ArtificialNeuralNetworkBuilder::Copy(const ArtificialNeuralNetwork* artificialNeuralNetwork)
{
	if (!artificialNeuralNetwork)
		return nullptr;

	auto* result = new ArtificialNeuralNetwork;
	result->m_neuronLayers = artificialNeuralNetwork->m_neuronLayers;
	result->m_weightLayers = artificialNeuralNetwork->m_weightLayers;
	result->m_biasVector = artificialNeuralNetwork->m_biasVector;
	result->m_activationFunctionIndexes = artificialNeuralNetwork->m_activationFunctionIndexes;
	result->m_numberOfNeurons = artificialNeuralNetwork->m_numberOfNeurons;
	result->m_numberOfWeights = artificialNeuralNetwork->m_numberOfWeights;
	return result;
}

bool ArtificialNeuralNetworkBuilder::Initialize()
{
	ArtificialNeuralNetworkBuilder builder;

	// Call internal implementation
	builder.CreateDummy();

	if (!builder.Validate())
	{
		CoreLogger::PrintError("Cannot create ArtificialNeuralNetwork dummy!");
		return false;
	}

	CoreLogger::PrintSuccess("ArtificialNeuralNetworkBuilder initialized correctly");
	return true;
}
