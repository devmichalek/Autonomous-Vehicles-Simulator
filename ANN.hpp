#pragma once
#include <vector>
#include <functional>

using Neuron = double;
using NeuronLayer = std::vector<Neuron>;
using NeuronLayers = std::vector<NeuronLayer>;
using Weight = double;
using NeuronWeights = std::vector<Weight>;
using WeightLayer = std::vector<NeuronWeights>;
using WeightLayers = std::vector<WeightLayer>;
using Bias = double;
using BiasVector = std::vector<Bias>;

namespace ActivationFunction
{
	using Activation = std::function<Neuron(Neuron)>;
	using ActivationVector = std::vector<Activation>;
	Activation stub = [](Neuron input) { return input; };
	Activation sigmoid = [](Neuron input) { return input; };
	Activation relu = [](Neuron input) { return input; };
	Activation leakyrelu = [](Neuron input) { return input; };
	Activation tanh = [](Neuron input) { return input; };
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
				neuron += m_biasVector[layerNr];

				// Call activation function
				neuron = m_activationFunctions[layerNr](neuron);
			}
		}
	}

public:
	ArtificialNeuralNetwork(size_t const inputLayerSize, size_t const outputLayerSize, std::vector<size_t> hiddenLayerSizes)
	{
		m_neuronLayers.resize(1 + 1 + hiddenLayerSizes.size());
		m_weightLayers.resize(1 + hiddenLayerSizes.size());
		m_biasVector.resize(1 + 1 + hiddenLayerSizes.size(), 0);
		m_activationFunctions.resize(1 + 1 + hiddenLayerSizes.size(), ActivationFunction::stub);
		auto neuronLayersSize = m_neuronLayers.size();
		m_neuronLayers.front().resize(inputLayerSize, 0); // Input layer
		m_neuronLayers.back().resize(outputLayerSize, 0); // Output layer
		for (size_t i = 1; i < neuronLayersSize - 1; ++i)
		{
			m_neuronLayers[i].resize(hiddenLayerSizes[i - 1], 0);
			m_weightLayers[i - 1].resize(hiddenLayerSizes[i - 1]);
			for (auto& neuronWeights : m_weightLayers[i - 1])
				neuronWeights.resize(m_neuronLayers[i - 1].size());
		}
		m_weightLayers.back().resize(outputLayerSize);
		for (auto& neuronWeights : m_weightLayers.back())
			neuronWeights.resize(hiddenLayerSizes.back());
	}

	void setBiasVector(BiasVector biasVector)
	{
		if (biasVector.size() == m_biasVector.size())
			m_biasVector = biasVector;
	}

	void setWeightLayers(WeightLayers)
	{

	}

	void calculate(NeuronLayer inputLayer)
	{
		m_neuronLayers.front() = inputLayer;
		calculateInternal();
	}
};