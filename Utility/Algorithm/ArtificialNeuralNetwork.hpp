#pragma once
#include <vector>
#include <math.h>
#include "Neural.hpp"
#include "ActivationFunctionContext.hpp"

class ArtificialNeuralNetwork final
{
	NeuronLayers m_neuronLayers;
	WeightLayers m_weightLayers;
	BiasVector m_biasVector; // Bias per neuron layer
	ActivationFunctionIndexes m_activationFunctionIndexes;
	size_t m_numberOfNeurons;
	size_t m_numberOfWeights;

	friend class ArtificialNeuralNetworkBuilder;

	inline void UpdateInternal()
	{
		// For each neuron layer
		const size_t numberOfLayers = m_neuronLayers.size();
		for (size_t layerNr = 1; layerNr < numberOfLayers; ++layerNr)
		{
			// Get proper weight layer
			const WeightLayer& weightLayer = m_weightLayers[layerNr - 1];

			// Get number of neurons
			const size_t numberOfNeurons = m_neuronLayers[layerNr].size();

			// For each neuron
			for (size_t neuronNr = 0; neuronNr < numberOfNeurons; ++neuronNr)
			{
				// Get proper neuron weights, get proper neuron and reset it
				const Weights& weights = weightLayer[neuronNr];
				Neuron& neuron = m_neuronLayers[layerNr][neuronNr];
				neuron = 0;

				// Calculate neuron value
				// w1*a1 + w2*a2 + ... + wn*an
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

public:

	ArtificialNeuralNetwork(const NeuronLayers neuronLayers,
							const WeightLayers weightLayers,
							const BiasVector biasVector,
							const ActivationFunctionIndexes activationFunctionIndexes,
							const size_t numberOfNeurons,
							const size_t numberOfWeights) :
		m_neuronLayers(neuronLayers),
		m_weightLayers(weightLayers),
		m_biasVector(biasVector),
		m_activationFunctionIndexes(activationFunctionIndexes),
		m_numberOfNeurons(numberOfNeurons),
		m_numberOfWeights(numberOfWeights)
	{
	}

	ArtificialNeuralNetwork() :
		m_numberOfNeurons(0),
		m_numberOfWeights(0)
	{
	}

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

	void GetRawData(Neuron* data) const
	{
		if (data)
		{
			size_t index = 0;
			for (const auto& weightLayer : m_weightLayers)
			{
				for (const auto& weights : weightLayer)
				{
					for (const auto& weight : weights)
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

	inline size_t GetNumberOfNeurons() const
	{
		return m_numberOfNeurons;
	}

	inline size_t GetNumberOfWeights() const
	{
		return m_numberOfWeights;
	}

	inline size_t GetNumberOfInputNeurons() const
	{
		if (m_neuronLayers.empty())
			return 0;
		return m_neuronLayers.front().size();
	}

	inline size_t GetNumberOfOutputNeurons() const
	{
		if (m_neuronLayers.empty())
			return 0;
		return m_neuronLayers.back().size();
	}
};