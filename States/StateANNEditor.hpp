#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "DrawableVariableText.hpp"
#include "DrawableDoubleText.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableFilenameText.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include <SFML/Graphics/CircleShape.hpp>

class StateANNEditor final : public StateInterface
{
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
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::map<size_t, bool> m_pressedKeys;

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
	sf::CircleShape m_neuronShape;
	Line m_weightShape;

	// Texts
	DrawableVariableText m_inputText;
	DrawableVariableText m_outputText;
	DrawableTripleText m_currentLayerText;
	DrawableTripleText m_currentLayerNumberOfNeuronsText;
	DrawableTripleText m_currentLayerActivationFunctionText;
	DrawableTripleText m_currentLayerBiasText;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableDoubleText m_totalNumberOfLayersText;
	DrawableDoubleText m_totalNumberOfNeuronsText;
	DrawableDoubleText m_totalNumberOfWeightsText;
	DrawableDoubleText m_totalNumberOfActivationFunctionsText;
	std::vector<std::function<std::string()>> m_textFunctions;

	void CalculatePositions();

	void AddLayer();

	void RemoveLayer();

	void AddNeuron();

	void RemoveNeuron();

public:

	StateANNEditor(const StateANNEditor&) = delete;

	const StateANNEditor& operator=(const StateANNEditor&) = delete;

	StateANNEditor();

	~StateANNEditor();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};