#pragma once
#include <vector>
#include <math.h>
#include "Neural.hpp"
#include "ActivationFunctionContext.hpp"

class ArtificialNeuralNetworkBuilder;

class ArtificialNeuralNetwork
{
	NeuronLayers m_neuronLayers;
	WeightLayers m_weightLayers;
	BiasVector m_biasVector; // Bias per neuron layer
	ActivationFunctionIndexes m_activationFunctionIndexes;
	size_t m_numberOfNeurons;
	size_t m_numberOfWeights;
	friend ArtificialNeuralNetworkBuilder;

	inline void UpdateInternal()
	{
		// For each neuron layer
		const size_t numberOfLayers = m_neuronLayers.size();
		for (size_t layerNr = 1; layerNr < numberOfLayers; ++layerNr)
		{
			// Get proper weight layer
			WeightLayer& weightLayer = m_weightLayers[layerNr - 1];

			// Get number of neurons
			const size_t numberOfNeurons = m_neuronLayers[layerNr].size();

			// For each neuron
			for (size_t neuronNr = 0; neuronNr < numberOfNeurons; ++neuronNr)
			{
				// Get proper neuron weights, get proper neuron and reset it
				Weights& weights = weightLayer[neuronNr];
				Neuron& neuron = m_neuronLayers[layerNr][neuronNr];
				neuron = 0;

				// Calculate neuron value
				// neuron = activation(w1*a1 + w2*a2 + ... + wn*an + bias)
				const size_t numberOfWeights = weights.size();
				for (size_t weightNr = 0; weightNr < numberOfWeights; ++weightNr)
					neuron += (weights[weightNr] * m_neuronLayers[layerNr - 1][weightNr]);
				
				// Add bias
				neuron += m_biasVector[layerNr - 1];

				// Call activation function
				neuron = ActivationFunctionContext::Compute(m_activationFunctionIndexes[layerNr - 1], neuron);
			}
		}
	}

	ArtificialNeuralNetwork() :
		m_numberOfNeurons(0),
		m_numberOfWeights(0)
	{
	}

public:

	~ArtificialNeuralNetwork()
	{
	}

	void SetFromRawData(const Neuron* data)
	{
		if (data)
		{
			size_t index = 0;
			for (auto& weightLayer : m_weightLayers)
			{
				for (auto& weights : weightLayer)
				{
					for (auto& weight : weights)
						weight = data[index++];
				}
			}
		}
	}

	void GetRawData(Neuron* data)
	{
		if (data)
		{
			size_t index = 0;
			for (auto& weightLayer : m_weightLayers)
			{
				for (auto& weights : weightLayer)
				{
					for (auto& weight : weights)
						data[index++] = weight;
				}
			}
		}
	}

	const NeuronLayer& Update(const NeuronLayer& inputLayer)
	{
		m_neuronLayers.front() = inputLayer;
		UpdateInternal();
		return m_neuronLayers.back();
	}

	size_t GetNumberOfNeurons() const
	{
		return m_numberOfNeurons;
	}

	size_t GetNumberOfWeights() const
	{
		return m_numberOfWeights;
	}

	size_t GetNumberOfInputNeurons() const
	{
		if (m_neuronLayers.empty())
			return 0;
		return m_neuronLayers.front().size();
	}

	size_t GetNumberOfOutputNeurons() const
	{
		if (m_neuronLayers.empty())
			return 0;
		return m_neuronLayers.back().size();
	}
};