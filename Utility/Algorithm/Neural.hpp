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

const size_t NEURAL_MAX_NUMBER_OF_HIDDEN_LAYERS = 4;
const size_t NEURAL_MIN_NUMBER_OF_LAYERS = 3;
const size_t NEURAL_MAX_NUMBER_OF_LAYERS = NEURAL_MAX_NUMBER_OF_HIDDEN_LAYERS + NEURAL_MIN_NUMBER_OF_LAYERS;
const size_t NEURAL_MIN_NUMBER_OF_NEURONS_PER_LAYER = 1;
const size_t NEURAL_MAX_NUMBER_OF_NEURONS_PER_LAYER = 12;
const double NEURAL_MIN_BIAS = -10.0;
const double NEURAL_MAX_BIAS = 10.0;
const double NEURAL_DEFAULT_BIAS = 0.0;
const double NEURAL_DEFAULT_NEURON_VALUE = 0.0;
