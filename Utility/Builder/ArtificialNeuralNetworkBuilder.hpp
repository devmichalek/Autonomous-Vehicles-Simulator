#pragma once
#include "AbstractBuilder.hpp"
#include "Neural.hpp"
#include "ActivationFunctionContext.hpp"

class ArtificialNeuralNetwork;

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

	// Loads ANN from file
	bool LoadInternal(std::ifstream& input);

	// Saves ANN to file
	bool SaveInternal(std::ofstream& output);

	// Creates dummy ANN
	void CreateDummyInternal();

public:

	ArtificialNeuralNetworkBuilder();

	~ArtificialNeuralNetworkBuilder();

	// Sets neuron layer sizes
	void SetNeuronLayerSizes(NeuronLayerSizes neuronLayerSizes);

	// Sets activation function indexes
	void SetActivationFunctionIndexes(ActivationFunctionIndexes activationFunctionIndexes);

	// Sets bias vector
	void SetBiasVector(BiasVector biasVector);

	// Sets raw neuron data
	void SetRawNeuronData(NeuronLayer rawNeuronData);

	// Returns intermediate representation of neuron layers
	NeuronLayerSizes GetNeuronLayerSizes();

	// Returns intermediate representation of activation functions
	ActivationFunctionIndexes GetActivationFunctionIndexes();

	// Returns bias vector
	BiasVector GetBiasVector();

	// Returns raw neuron data
	const Neuron* GetRawNeuronData();

	// Returns artificial neural network
	ArtificialNeuralNetwork* Get();

	// Creates a copy of artificial neural network
	static ArtificialNeuralNetwork* Copy(const ArtificialNeuralNetwork* artificialNeuralNetwork);

	// Get maximum number of hidden layers
	inline static size_t GetMaxNumberOfHiddenLayers() { return 4; }

	// Get minimum number of layers
	inline static size_t GetMinNumberOfLayers() { return 3; }

	// Get maximum number of layers
	inline static size_t GetMaxNumberOfLayers() { return GetMinNumberOfLayers() + GetMaxNumberOfHiddenLayers(); }

	// Get minimum number of neurons per layer
	inline static size_t GetMinNumberOfNeuronsPerLayer() { return 1; }

	// Get maximum number of neurons per layer
	inline static size_t GetMaxNumberOfNeuronsPerLayer() { return 12; }

	// Get minimum bias value
	inline static double GetMinBiasValue() { return -10.0; }

	// Get maximum bias value
	inline static double GetMaxBiasValue() { return 10.0; }

	// Get default bias value
	inline static double GetDefaultBiasValue() { return 0.0; }

	// Get default neuron value
	inline static double GetDefaultNeuronValue() { return 0.0; }

	// Checks if dummy can be created
	static bool Initialize();
};