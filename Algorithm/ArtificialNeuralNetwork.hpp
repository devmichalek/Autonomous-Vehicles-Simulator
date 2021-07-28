#pragma once
#include <vector>
#include <math.h>
#include "Neural.hpp"

class ArtificialNeuralNetwork
{
	NeuronLayers m_neuronLayers;
	WeightLayers m_weightLayers;
	BiasVector m_biasVector; // Bias per neuron layer
	ActivationVector m_activationFunctions;

	inline void calculateInternal()
	{
		// For each neuron layer
		size_t layersNum = m_neuronLayers.size();
		for (size_t layerNr = 1; layerNr < layersNum; ++layerNr)
		{
			// Get proper weight layer
			WeightLayer& weightLayer = m_weightLayers[layerNr - 1];

			// Get number of neurons
			size_t neuronsNum = m_neuronLayers[layerNr].size();

			// For each neuron
			for (size_t neuronNr = 0; neuronNr < neuronsNum; ++neuronNr)
			{
				// Get proper neuron weights, get proper neuron and reset it
				Weights& weights = weightLayer[neuronNr];
				Neuron& neuron = m_neuronLayers[layerNr][neuronNr];
				neuron = 0;

				// Calculate neuron value
				// neuron = activation(w1*a1 + w2*a2 + ... + wn*an + bias)
				size_t numberOfWeights = weights.size();
				for (size_t weightNr = 0; weightNr < numberOfWeights; ++weightNr)
					neuron += (weights[weightNr] * m_neuronLayers[layerNr - 1][weightNr]);
				
				// Add bias
				neuron += m_biasVector[layerNr - 1];

				// Call activation function
				neuron = m_activationFunctions[layerNr - 1](neuron);
			}
		}
	}

public:
	ArtificialNeuralNetwork(size_t const inputLayerSize, size_t const outputLayerSize, std::vector<size_t> hiddenLayerSizes)
	{
		const size_t layersCount = 1 + 1 + hiddenLayerSizes.size();
		m_neuronLayers.resize(layersCount);
		m_weightLayers.resize(layersCount - 1);
		m_biasVector.resize(layersCount - 1, 0);
		m_activationFunctions.resize(layersCount - 1, activationStub);

		// Set up input layer
		m_neuronLayers.front().resize(inputLayerSize, 0);

		// Set up hidden layers
		for (size_t i = 1; i < layersCount - 1; ++i)
			m_neuronLayers[i].resize(hiddenLayerSizes[i - 1], 0);

		// Set up output layer
		m_neuronLayers.back().resize(outputLayerSize, 0);

		// Set up weights for neurons
		for (size_t i = 1; i < layersCount - 1; ++i)
		{
			m_weightLayers[i - 1].resize(hiddenLayerSizes[i - 1]);
			size_t connectionsCount = m_neuronLayers[i - 1].size();
			for (auto& weights : m_weightLayers[i - 1])
				weights.resize(connectionsCount, 0);
		}
		m_weightLayers.back().resize(outputLayerSize);
		size_t connectionsCount = m_neuronLayers[layersCount - 2].size();
		for (auto& weights : m_weightLayers.back())
			weights.resize(connectionsCount, 0);
	}

	void setBiasVector(BiasVector biasVector)
	{
		if (biasVector.size() == m_biasVector.size())
			m_biasVector = biasVector;
	}

	void setActivationVector(ActivationVector activationVector)
	{
		if (m_activationFunctions.size() == activationVector.size())
			m_activationFunctions = activationVector;
	}

	size_t getDataUnitsCount()
	{
		size_t result = 0;
		for (auto& i : m_weightLayers)
			result += i.size() * i.front().size();
		return result;
	}

	void setData(Neuron* data)
	{
		size_t index = 0;
		for (auto& weightLayer : m_weightLayers)
		{
			for (auto& weights : weightLayer)
			{
				for (auto& weight : weights)
				{
					weight = data[index++];
				}
			}
		}
	}

	NeuronLayer calculate(NeuronLayer inputLayer)
	{
		m_neuronLayers.front() = inputLayer;
		calculateInternal();
		return m_neuronLayers.back();
	}
};