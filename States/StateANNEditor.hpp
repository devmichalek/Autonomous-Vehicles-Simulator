#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableTextAbstract.hpp"
#include <SFML/Graphics/CircleShape.hpp>

class ObserverInterface;

class StateANNEditor final :
	public StateInterface
{
	// Control states
	enum
	{
		SWITCH_LAYER,
		ADD_LAYER,
		REMOVE_LAYER,
		ADD_NEURON,
		REMOVE_NEURON,
		SWITCH_ACTIVATION_FUNCTION,
		INCREASE_BIAS,
		DECREASE_BIAS,
		CONTROLS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::vector<bool> m_pressedKeys;

	// Subjects of change
	NeuronLayerSizes m_neuronLayerSizes;
	ActivationFunctionIndexes m_activationFunctionIndexes;
	BiasVector m_biasVector;
	const double m_biasOffset;
	size_t m_totalNumberOfNeurons;
	size_t m_totalNumberOfWeights;
	size_t m_currentLayer;
	ArtificialNeuralNetworkBuilder m_artificialNeuralNetworkBuilder;
	bool m_upToDate;

	// Drawable shapes and their position
	std::vector<std::vector<sf::Vector2f>> m_layersPositions;
	std::vector<Edge> m_weightPositions;
	std::vector<sf::Color> m_weightStrengths;
	sf::CircleShape m_neuronShape;
	Line m_weightShape;

	// Texts and text observers
	enum
	{
		INPUT_TEXT,
		OUTPUT_TEXT,
		CURRENT_LAYER_TEXT,
		CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT,
		CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT,
		CURRENT_LAYER_BIAS_TEXT,
		FILENAME_TEXT,
		NUMBER_OF_LAYERS_TEXT,
		NUMBER_OF_NEURONS_TEXT,
		NUMBER_OF_WEIGHTS_TEXT,
		NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT,
		TEXT_COUNT
	};
	std::vector<DrawableTextAbstract*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;

	// Calculates positions of neuron shapes and weight shapes
	void CalculatePositions();

	// Adds neuron layer to the container
	void AddLayer();

	// Removes neuron layer from the container
	void RemoveLayer();

	// Adds neuron to the current active neuron layer
	void AddNeuron();

	// Removes neuron from the current active neruon layer
	void RemoveNeuron();

	// Returns weight strength representation in color
	sf::Color GetWeightStrength(double max, double value);

public:

	StateANNEditor(const StateANNEditor&) = delete;

	const StateANNEditor& operator=(const StateANNEditor&) = delete;

	StateANNEditor();

	~StateANNEditor();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;
};