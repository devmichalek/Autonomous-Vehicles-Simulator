#pragma once
#include <functional>

using Neuron = double;
using NeuronLayer = std::vector<Neuron>;
using NeuronLayers = std::vector<NeuronLayer>;
using Weight = double;
using Weights = std::vector<Weight>;
using WeightLayer = std::vector<Weights>;
using WeightLayers = std::vector<WeightLayer>;
using Bias = double;
using BiasVector = std::vector<Bias>;

using Activation = std::function<Neuron(Neuron)>;
using ActivationVector = std::vector<Activation>;
inline Activation activationStub = [](Neuron input) { return input; };
inline Activation activationFastsigmoid = [](Neuron input) { return input / (1 + std::fabs(input)); };
inline Activation activationRelu = [](Neuron input) { return input < 0 ? 0 : input; };
inline Activation activationLeakyrelu = [](Neuron input) { return input >= 0 ? input : input * 0.1; };
inline Activation activationTanh = [](Neuron input) { return tanh(input); };