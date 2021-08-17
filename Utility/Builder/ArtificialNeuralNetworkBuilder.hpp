#pragma once
#include "Neural.hpp"
#include "ActivationFunctionContext.hpp"
#include <map>

class ArtificialNeuralNetwork;

class ArtificialNeuralNetworkBuilder
{
	enum
	{
		ERROR_UNKNOWN,
		SUCCESS_LOAD_COMPLETED,
		SUCCESS_SAVE_COMPLETED,
		SUCCESS_VALIDATION_PASSED,
		ERROR_TOO_LITTLE_NEURON_LAYERS,
		ERROR_TOO_MANY_NEURON_LAYERS,
		ERROR_TOO_LITTLE_NEURONS_IN_LAYER,
		ERROR_TOO_MANY_NEURONS_IN_LAYER,
		ERROR_INCORRECT_NUMBER_OF_ACTIVATION_FUNCTIONS,
		ERROR_INCORRECT_LENGTH_OF_BIAS_VECTOR,
		ERROR_BIAS_IS_LESS_THAN_MINIMUM_ALLOWED,
		ERROR_BIAS_IS_GREATER_THAN_MAXIMUM_ALLOWED,
		ERROR_UNKNOWN_ACTIVATION_FUNCTION_INDEX,
		ERROR_NUMBER_OF_WEIGHTS_MISMATCH,
		ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_READING,
		ERROR_CANNOT_OPEN_FILE_FOR_READING,
		ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING,
		ERROR_CANNOT_OPEN_FILE_FOR_WRITING
	};
	std::map<size_t, std::string> m_operationsMap;
	size_t m_lastOperationStatus;

	NeuronLayerSizes m_neuronLayerSizes;
	ActivationFunctionIndexes m_activationFunctionIndexes;
	BiasVector m_biasVector;
	size_t m_numberOfNeurons;
	size_t m_numberOfWeights;
	bool m_validated;

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

	// Validates internal fields
	bool Validate();

public:

	ArtificialNeuralNetworkBuilder();

	~ArtificialNeuralNetworkBuilder();

	// Clears internal fields
	void Clear();

	// Loads ANN from file
	bool Load(std::string filename, Neuron* rawData = nullptr);

	// Saves ANN to file
	bool Save(std::string filename, Neuron* rawData = nullptr);

	// Returns last operation status
	// Returns true in case of success and false in case of failure
	std::pair<bool, std::string> GetLastOperationStatus();

	// Creates dummy ANN
	bool CreateDummy();

	// Sets neuron layer sizes
	void SetNeuronLayerSizes(NeuronLayerSizes neuronLayerSizes);

	// Sets activation function indexes
	void SetActivationFunctionIndexes(ActivationFunctionIndexes activationFunctionIndexes);

	// Sets bias vector
	void SetBiasVector(BiasVector biasVector);

	// Returns intermediate representation of neuron layers
	NeuronLayerSizes GetNeuronLayerSizes();

	// Returns intermediate representation of activation functions
	ActivationFunctionIndexes GetActivationFunctionIndexes();

	// Returns bias vector
	BiasVector GetBiasVector();

	// Returns artificial neural network
	ArtificialNeuralNetwork* Get();
};