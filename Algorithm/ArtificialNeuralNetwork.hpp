#pragma once
#include <vector>
#include <functional>
#include <math.h>
#include "Neural.hpp"

namespace ActivationFunction
{
	using Activation = std::function<Neuron(Neuron)>;
	using ActivationVector = std::vector<Activation>;
	inline Activation stub = [](Neuron input) { return input; };
	inline Activation fastsigmoid = [](Neuron input) { return input / (1 + std::fabs(input)); };
	inline Activation relu = [](Neuron input) { return input < 0 ? 0 : input; };
	inline Activation leakyrelu = [](Neuron input) { return input >= 0 ? input : input * 0.1; };
	inline Activation tanh = [](Neuron input) { return tanh(input); };
}

class ArtificialNeuralNetwork
{
	NeuronLayers m_neuronLayers;
	WeightLayers m_weightLayers;
	BiasVector m_biasVector; // Bias per neuron layer
	ActivationFunction::ActivationVector m_activationFunctions;

	inline void calculateInternal()
	{
		// For each neuron layer
		size_t layersNum = m_neuronLayers.size();
		for (size_t layerNr = 1; layerNr <= layersNum; ++layerNr)
		{
			// Get proper weight layer
			WeightLayer& weightLayer = m_weightLayers[layerNr - 1];

			// Get number of neurons
			size_t neuronsNum = m_neuronLayers[layerNr].size();

			// For each neuron
			for (size_t neuronNr = 0; neuronNr < neuronsNum; ++neuronNr)
			{
				// Get proper neuron weights, get proper neuron and reset it
				NeuronWeights& neuronWeights = weightLayer[neuronNr];
				Neuron& neuron = m_neuronLayers[layerNr][neuronNr];
				neuron = 0;

				// Calculate neuron value
				// neuron = activation(w1*a1 + w2*a2 + ... + wn*an + bias)
				size_t neuronWeightsCount = neuronWeights.size();
				for (size_t weightNr = 0; weightNr < neuronWeightsCount; ++weightNr)
					neuron += (neuronWeights[weightNr] * m_neuronLayers[layerNr - 1][weightNr]);
				
				// Add bias
				neuron += m_biasVector[layerNr - 1];

				// Call activation function
				neuron = m_activationFunctions[layerNr - 1](neuron);
			}
		}
	}

	friend class GeneticAlgorithmNeuron;

public:
	ArtificialNeuralNetwork(size_t const inputLayerSize, size_t const outputLayerSize, std::vector<size_t> hiddenLayerSizes)
	{
		const size_t layersCount = 1 + 1 + hiddenLayerSizes.size();
		m_neuronLayers.resize(layersCount);
		m_weightLayers.resize(layersCount - 1);
		m_biasVector.resize(layersCount - 1, 0);
		m_activationFunctions.resize(layersCount - 1, ActivationFunction::stub);

		// Set up input layer
		m_neuronLayers.front().resize(inputLayerSize, 0);

		// Set up hidden layers
		for (size_t i = 1; i < layersCount - 1; ++i)
			m_neuronLayers[i].resize(hiddenLayerSizes[i - 1], 0);

		// Set up output layer
		m_neuronLayers.back().resize(outputLayerSize, 0);

		// Set up weights for neurons
		for (size_t i = 0; i < layersCount - 1; ++i)
		{
			m_weightLayers[i].resize(hiddenLayerSizes[i]);
			for (auto& neuronWeights : m_weightLayers[i])
				neuronWeights.resize(m_neuronLayers[i].size(), 0);
		}
	}

	void setBiasVector(BiasVector biasVector)
	{
		if (biasVector.size() == m_biasVector.size())
			m_biasVector = biasVector;
	}

	void setWeightLayers(std::vector<Neuron> data)
	{

	}

	void calculate(NeuronLayer inputLayer)
	{
		m_neuronLayers.front() = inputLayer;
		calculateInternal();
	}
};