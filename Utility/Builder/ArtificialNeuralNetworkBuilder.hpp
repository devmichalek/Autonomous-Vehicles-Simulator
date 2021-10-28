#pragma once
#include "AbstractBuilder.hpp"
#include "Neural.hpp"
#include "ActivationFunctionContext.hpp"

class ArtificialNeuralNetworkBuilder final :
	public AbstractBuilder
{
	enum
	{
		ERROR_TOO_LITTLE_NEURON_LAYERS = LAST_ENUM_OPERATION_INDEX,
		ERROR_TOO_MANY_NEURON_LAYERS,
		ERROR_TOO_LITTLE_NEURONS_IN_LAYER,
		ERROR_TOO_MANY_NEURONS_IN_LAYER,
		ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS,
		ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR,
		ERROR_BIAS_IS_LESS_THAN_MINIMUM_ALLOWED,
		ERROR_BIAS_IS_GREATER_THAN_MAXIMUM_ALLOWED,
		ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX,
		ERROR_NUMBER_OF_WEIGHTS_MISMATCH,
		ERROR_RAW_DATA_VECTOR_LENGTH_IS_INCORRECT
	};

	NeuronLayerSizes m_neuronLayerSizes;
	ActivationFunctionIndexes m_activationFunctionIndexes;
	BiasVector m_biasVector;
	std::vector<Neuron> m_rawData;
	size_t m_numberOfNeurons;
	size_t m_numberOfWeights;

	// Validates number of layers
	bool ValidateNumberOfLayers(size_t size);

	// Validates number of neurons in a layer
	bool ValidateNumberOfNeurons(size_t size);

	// Calculates number of neurons
	void CalculateNumberOfNeurons();

	// Calculates number of weights
	void CalculateNumberOfWeights();

	// Validates number of activation function indexes
	bool ValidateNumberOfActivationFunctionIndexes(size_t size);

	// Validates activation function index
	bool ValidateActivationFunctionIndex(ActivationFunctionIndex activationFunctionIndex);

	// Validates bias vector length
	bool ValidateBiasVectorLength(size_t length);

	// Validates bias
	bool ValidateBias(Bias bias);

	// Validates raw data vector
	bool ValidateRawData();

	// Validate internal fields
	bool ValidateInternal();

	// Clears internal fields
	void ClearInternal();

	// Loads artificial neural network from file
	bool LoadInternal(std::ifstream& input);

	// Saves artificial neural network to file
	bool SaveInternal(std::ofstream& output);

	// Creates dummy artificial neural network
	void CreateDummyInternal();

public:

	ArtificialNeuralNetworkBuilder();

	~ArtificialNeuralNetworkBuilder()
	{
	}

	// Sets neuron layer sizes
	inline void SetNeuronLayerSizes(NeuronLayerSizes neuronLayerSizes)
	{
		m_neuronLayerSizes = neuronLayerSizes;
	}

	// Sets activation function indexes
	inline void SetActivationFunctionIndexes(ActivationFunctionIndexes activationFunctionIndexes)
	{
		m_activationFunctionIndexes = activationFunctionIndexes;
	}

	// Sets bias vector
	inline void SetBiasVector(BiasVector biasVector)
	{
		m_biasVector = biasVector;
	}

	// Sets raw neuron data
	inline void SetRawNeuronData(NeuronLayer rawNeuronData)
	{
		m_rawData = rawNeuronData;
	}

	// Returns intermediate representation of neuron layers
	inline NeuronLayerSizes GetNeuronLayerSizes() const
	{
		return m_neuronLayerSizes;
	}

	// Returns intermediate representation of activation functions
	inline ActivationFunctionIndexes GetActivationFunctionIndexes() const
	{
		return m_activationFunctionIndexes;
	}

	// Returns bias vector
	inline BiasVector GetBiasVector() const
	{
		return m_biasVector;
	}

	// Returns raw neuron data
	inline const Neuron* GetRawNeuronData() const
	{
		return &m_rawData[0];
	}

	void Set(ArtificialNeuralNetwork* artificialNeuralNetwork);

	// Returns artificial neural network
	ArtificialNeuralNetwork* Get();

	// Creates a copy of artificial neural network
	static ArtificialNeuralNetwork* Copy(const ArtificialNeuralNetwork* artificialNeuralNetwork);

	// Get maximum number of hidden layers
	inline static size_t GetMaxNumberOfHiddenLayers()
	{
		return 4;
	}

	// Get minimum number of layers
	inline static size_t GetMinNumberOfLayers()
	{
		return 3;
	}

	// Get maximum number of layers
	inline static size_t GetMaxNumberOfLayers()
	{
		return GetMinNumberOfLayers() + GetMaxNumberOfHiddenLayers();
	}

	// Get minimum number of neurons per layer
	inline static size_t GetMinNumberOfNeuronsPerLayer()
	{
		return 1;
	}

	// Get maximum number of neurons per layer
	inline static size_t GetMaxNumberOfNeuronsPerLayer()
	{
		return 12;
	}

	// Get minimum bias value
	inline static double GetMinBiasValue()
	{
		return -4.0;
	}

	// Get maximum bias value
	inline static double GetMaxBiasValue()
	{
		return 4.0;
	}

	// Get default bias value
	inline static double GetDefaultBiasValue()
	{
		return 0.0;
	}

	// Get min neuron value
	inline static double GetMinNeuronValue()
	{
		return 0.0;
	}

	// Get max neuron value
	inline static double GetMaxNeuronValue()
	{
		return 1.0;
	}

	// Checks if dummy can be created
	static bool Initialize();
};