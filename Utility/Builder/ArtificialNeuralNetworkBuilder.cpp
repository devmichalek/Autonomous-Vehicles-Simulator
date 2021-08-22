#include "ArtificialNeuralNetworkBuilder.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include <random>

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfLayers(size_t size)
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

bool ArtificialNeuralNetworkBuilder::ValidateNumberOfNeurons(size_t size)
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
	if (activationFunctionIndex >= ActivationFunctionContext::GetActivationFunctionsCount())
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

	CalculateNumberOfNeurons();

	CalculateNumberOfWeights();

	if (m_rawData.empty())
		m_rawData.resize(m_numberOfWeights, GetDefaultNeuronValue());

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
	for (size_t i = 0; i < length; ++i)
		m_neuronLayerSizes[i] = nlsdistribution(engine);
	--length;
	m_activationFunctionIndexes.resize(length);
	m_biasVector.resize(length, 0.0);
	for (size_t i = 0; i < length; ++i)
		m_activationFunctionIndexes[i] = afdistribution(engine);
}

ArtificialNeuralNetworkBuilder::ArtificialNeuralNetworkBuilder() :
	AbstractBuilder(std::ios::in | std::ios::binary, std::ios::out | std::ios::binary),
	m_numberOfNeurons(0),
	m_numberOfWeights(0)
{
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
	m_operationsMap[ERROR_RAW_DATA_VECTOR_LENGTH_IS_INCORRECT] = "Error: Raw data vector length is incorrect!";
	Clear();
}

ArtificialNeuralNetworkBuilder::~ArtificialNeuralNetworkBuilder()
{
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

void ArtificialNeuralNetworkBuilder::SetRawNeuronData(std::vector<Neuron> rawNeuronData)
{
	m_rawData = rawNeuronData;
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

std::vector<Neuron> ArtificialNeuralNetworkBuilder::GetRawNeuronData()
{
	return m_rawData;
}

ArtificialNeuralNetwork* ArtificialNeuralNetworkBuilder::Get()
{
	if (!Validate())
		return nullptr;

	// Create artifial neural network
	auto* ann = new ArtificialNeuralNetwork;

	// Get number of layers
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