#pragma once
#include <vector>

using Neuron = double;
using NeuronLayer = std::vector<Neuron>;
using NeuronLayers = std::vector<NeuronLayer>;
using NeuronLayerSize = size_t;
using NeuronLayerSizes = std::vector<NeuronLayerSize>;
using Weight = double;
using Weights = std::vector<Weight>;
using WeightLayer = std::vector<Weights>;
using WeightLayers = std::vector<WeightLayer>;
using Bias = double;
using BiasVector = std::vector<Bias>;
