#pragma once
#include "StateInterface.hpp"
#include "MathContext.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Keyboard.hpp>

class AbstractText;
class ObserverInterface;

class StateArtificialNeuralNetworkEditor final :
	public StateInterface
{
public:

	StateArtificialNeuralNetworkEditor(const StateArtificialNeuralNetworkEditor&) = delete;

	const StateArtificialNeuralNetworkEditor& operator=(const StateArtificialNeuralNetworkEditor&) = delete;

	StateArtificialNeuralNetworkEditor();

	~StateArtificialNeuralNetworkEditor();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;

private:

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

	// Control keys
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
		CONTROL_KEYS_COUNT
	};
	std::map<const size_t, const size_t> m_controlKeys;
	std::array<bool, CONTROL_KEYS_COUNT> m_pressedKeys;

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
	EdgeShape m_weightShape;

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
		TOTAL_NUMBER_OF_LAYERS_TEXT,
		TOTAL_NUMBER_OF_NEURONS_TEXT,
		TOTAL_NUMBER_OF_WEIGHTS_TEXT,
		TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT,
		TEXT_COUNT
	};
	std::vector<AbstractText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;
};