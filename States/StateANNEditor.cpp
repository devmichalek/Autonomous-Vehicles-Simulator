#include "StateANNEditor.hpp"
#include "CoreLogger.hpp"
#include "FunctionEventObserver.hpp"
#include "TypeEventObserver.hpp"
#include "VariableText.hpp"
#include "FilenameText.hpp"

StateANNEditor::StateANNEditor() :
	m_biasOffset(0.2)
{
	m_controlKeys[sf::Keyboard::Tab] = SWITCH_LAYER;
	m_controlKeys[sf::Keyboard::Enter] = ADD_LAYER;
	m_controlKeys[sf::Keyboard::Backspace] = REMOVE_LAYER;
	m_controlKeys[sf::Keyboard::Add] = ADD_NEURON;
	m_controlKeys[sf::Keyboard::Subtract] = REMOVE_NEURON;
	m_controlKeys[sf::Keyboard::Multiply] = SWITCH_ACTIVATION_FUNCTION;
	m_controlKeys[sf::Keyboard::Z] = INCREASE_BIAS;
	m_controlKeys[sf::Keyboard::X] = DECREASE_BIAS;
	m_pressedKeys.resize(CONTROLS_COUNT, false);

	m_totalNumberOfNeurons = 0;
	m_totalNumberOfWeights = 0;
	m_currentLayer = 0;
	m_upToDate = false;

	// Create dummy
	m_artificialNeuralNetworkBuilder.CreateDummy();
	m_neuronLayerSizes = m_artificialNeuralNetworkBuilder.GetNeuronLayerSizes();
	m_activationFunctionIndexes = m_artificialNeuralNetworkBuilder.GetActivationFunctionIndexes();
	m_biasVector = m_artificialNeuralNetworkBuilder.GetBiasVector();
	CalculatePositions();

	m_neuronShape.setFillColor(sf::Color::White);
	m_neuronShape.setOutlineColor(sf::Color::White);
	m_neuronShape.setRadius(CoreWindow::GetWindowSize().x * 0.008f);
	m_weightShape[0].color = sf::Color(255, 255, 255, 128);
	m_weightShape[1].color = m_weightShape[0].color;

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateANNEditor::~StateANNEditor()
{
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateANNEditor::Reload()
{
	// Create dummy
	m_artificialNeuralNetworkBuilder.CreateDummy();
	m_neuronLayerSizes = m_artificialNeuralNetworkBuilder.GetNeuronLayerSizes();
	m_activationFunctionIndexes = m_artificialNeuralNetworkBuilder.GetActivationFunctionIndexes();
	m_biasVector = m_artificialNeuralNetworkBuilder.GetBiasVector();
	m_currentLayer = 0;
	m_upToDate = false;
	CalculatePositions();

	// Reset pressed keys
	for (size_t i = 0; i < CONTROLS_COUNT; ++i)
		m_pressedKeys[i] = false;

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}

	CoreWindow::Reset();
}

void StateANNEditor::Capture()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	filenameText->Capture();
	if (!filenameText->IsRenaming())
	{
		if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end() && !m_pressedKeys[iterator->second])
			{
				m_pressedKeys[iterator->second] = true;
				switch (iterator->second)
				{
					case SWITCH_LAYER:
						if (m_neuronLayerSizes.size() > 1)
						{
							++m_currentLayer;
							if (m_currentLayer >= m_neuronLayerSizes.size())
								m_currentLayer = 0;

							m_textObservers[CURRENT_LAYER_TEXT]->Notify();
							m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->Notify();
							m_textObservers[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT]->Notify();
							m_textObservers[CURRENT_LAYER_BIAS_TEXT]->Notify();
						}
						break;
					case ADD_LAYER:
						AddLayer();
						CalculatePositions();
						break;
					case REMOVE_LAYER:
						RemoveLayer();
						CalculatePositions();
						break;
					case ADD_NEURON:
						AddNeuron();
						CalculatePositions();
						break;
					case REMOVE_NEURON:
						RemoveNeuron();
						CalculatePositions();
						break;
					case SWITCH_ACTIVATION_FUNCTION:
						if (!m_activationFunctionIndexes.empty() && m_currentLayer != 0)
						{
							++m_activationFunctionIndexes[m_currentLayer - 1];
							if (m_activationFunctionIndexes[m_currentLayer - 1] >= ActivationFunctionContext::GetActivationFunctionsCount())
								m_activationFunctionIndexes[m_currentLayer - 1] = ActivationFunctionContext::GetMinActivationFunctionIndex();
							m_upToDate = false;
							m_textObservers[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT]->Notify();
						}
						break;
					case INCREASE_BIAS:
						if (!m_biasVector.empty() && m_currentLayer != 0)
						{
							m_biasVector[m_currentLayer - 1] += m_biasOffset;
							if (m_biasVector[m_currentLayer - 1] > ArtificialNeuralNetworkBuilder::GetMaxBiasValue())
								m_biasVector[m_currentLayer - 1] = ArtificialNeuralNetworkBuilder::GetMaxBiasValue();
							m_upToDate = false;
							m_textObservers[CURRENT_LAYER_BIAS_TEXT]->Notify();
						}
						break;
					case DECREASE_BIAS:
						if (!m_biasVector.empty() && m_currentLayer != 0)
						{
							m_biasVector[m_currentLayer - 1] -= m_biasOffset;
							if (m_biasVector[m_currentLayer - 1] < ArtificialNeuralNetworkBuilder::GetMinBiasValue())
								m_biasVector[m_currentLayer - 1] = ArtificialNeuralNetworkBuilder::GetMinBiasValue();
							m_upToDate = false;
							m_textObservers[CURRENT_LAYER_BIAS_TEXT]->Notify();
						}
						break;
				}
			}
		}
		else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end())
				m_pressedKeys[iterator->second] = false;
		}
	}
	else
		m_upToDate = false;
}

void StateANNEditor::Update()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (filenameText->IsWriting())
	{
		if (!m_upToDate)
		{
			m_artificialNeuralNetworkBuilder.Clear();
			m_artificialNeuralNetworkBuilder.SetNeuronLayerSizes(m_neuronLayerSizes);
			m_artificialNeuralNetworkBuilder.SetActivationFunctionIndexes(m_activationFunctionIndexes);
			m_artificialNeuralNetworkBuilder.SetBiasVector(m_biasVector);
			bool success = m_artificialNeuralNetworkBuilder.Save(filenameText->GetFilename());
			auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();
			filenameText->ShowStatusText();
			if (success)
				filenameText->SetSuccessStatusText(status.second);
			else
				filenameText->SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (filenameText->IsReading())
	{
		bool success = m_artificialNeuralNetworkBuilder.Load(filenameText->GetFilename());
		auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();
		filenameText->ShowStatusText();
		if (success)
		{
			filenameText->SetSuccessStatusText(status.second);
			m_neuronLayerSizes = m_artificialNeuralNetworkBuilder.GetNeuronLayerSizes();
			m_activationFunctionIndexes = m_artificialNeuralNetworkBuilder.GetActivationFunctionIndexes();
			m_biasVector = m_artificialNeuralNetworkBuilder.GetBiasVector();
			m_currentLayer = 0;
			m_upToDate = true;
			CalculatePositions();
			auto weights = m_artificialNeuralNetworkBuilder.GetRawNeuronData();

			// Find max value
			double max = 0.001;
			for (size_t i = 0; i < m_weightStrengths.size(); ++i)
			{
				if (weights[i] > max)
					max = weights[i];
			}

			// Calculate strengths
			for (size_t i = 0; i < m_weightStrengths.size(); ++i)
				m_weightStrengths[i] = GetWeightStrength(max, weights[i]);

			// Notify observers
			m_textObservers[CURRENT_LAYER_TEXT]->Notify();
			m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->Notify();
			m_textObservers[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT]->Notify();
			m_textObservers[CURRENT_LAYER_BIAS_TEXT]->Notify();
			m_textObservers[TOTAL_NUMBER_OF_LAYERS_TEXT]->Notify();
			m_textObservers[TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT]->Notify();
		}
		else
			filenameText->SetErrorStatusText(status.second);
	}

	for (const auto& text : m_texts)
		text->Update();
}

bool StateANNEditor::Load()
{
	// Create texts
	m_texts[INPUT_TEXT] = new VariableText({ "Input" });
	m_texts[OUTPUT_TEXT] = new VariableText({ "Output" });
	m_texts[CURRENT_LAYER_TEXT] = new TripleText({ "Current layer:", "", "| [Tab] [Enter] [Backspace]" });
	m_texts[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT] = new TripleText({ "Current layer number of neurons:", "", "| [+] [-]" });
	m_texts[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT] = new TripleText({ "Current layer activation function:", "", "| [*]" });
	m_texts[CURRENT_LAYER_BIAS_TEXT] = new TripleText({ "Current layer bias:", "", "| [Z] [X]" });
	m_texts[FILENAME_TEXT] = new FilenameText<true, true>("ann.bin");
	m_texts[TOTAL_NUMBER_OF_LAYERS_TEXT] = new DoubleText({ "Total number of layers:" });
	m_texts[TOTAL_NUMBER_OF_NEURONS_TEXT] = new DoubleText({ "Total number of neurons:" });
	m_texts[TOTAL_NUMBER_OF_WEIGHTS_TEXT] = new DoubleText({ "Total number of weights:" });
	m_texts[TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT] = new DoubleText({ "Total number of activation functions:" });

	// Create observers
	m_textObservers[INPUT_TEXT] = nullptr;
	m_textObservers[OUTPUT_TEXT] = nullptr;
	m_textObservers[CURRENT_LAYER_TEXT] = new FunctionEventObserver<std::string>([&] { return m_neuronLayerSizes.empty() ? "None" : ("Layer " + std::to_string(m_currentLayer)); });
	m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT] = new FunctionEventObserver<std::string>([&] { return m_neuronLayerSizes.empty() ? "None" : std::to_string(m_neuronLayerSizes[m_currentLayer]); });
	m_textObservers[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT] = new FunctionEventObserver<std::string>([&] { return m_currentLayer == 0 ? std::string("None") : ActivationFunctionContext::GetString(m_activationFunctionIndexes[m_currentLayer - 1]); });
	m_textObservers[CURRENT_LAYER_BIAS_TEXT] = new FunctionEventObserver<std::string>([&] { return m_currentLayer == 0 ? std::string("None") : std::to_string(m_biasVector[m_currentLayer - 1]); });
	m_textObservers[FILENAME_TEXT] = nullptr;
	m_textObservers[TOTAL_NUMBER_OF_LAYERS_TEXT] = new FunctionEventObserver<size_t>([&] { return m_neuronLayerSizes.size(); });
	m_textObservers[TOTAL_NUMBER_OF_NEURONS_TEXT] = new TypeEventObserver<size_t>(m_totalNumberOfNeurons);
	m_textObservers[TOTAL_NUMBER_OF_WEIGHTS_TEXT] = new TypeEventObserver<size_t>(m_totalNumberOfWeights);
	m_textObservers[TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT] = new FunctionEventObserver<size_t>([&] { return m_activationFunctionIndexes.size(); });

	// Set text observers
	for (size_t i = CURRENT_LAYER_TEXT; i < TEXT_COUNT; ++i)
		((DoubleText*)m_texts[i])->SetObserver(m_textObservers[i]);

	// Set text character size and rotation
	auto* inputText = static_cast<VariableText*>(m_texts[INPUT_TEXT]);
	auto* outputText = static_cast<VariableText*>(m_texts[OUTPUT_TEXT]);
	inputText->SetCharacterSize(4);
	outputText->SetCharacterSize(4);
	inputText->SetRotation(270.0f);
	outputText->SetRotation(90.0f);

	// Set text positions
	m_texts[INPUT_TEXT]->SetPosition({ FontContext::Component(26), {2} });
	m_texts[OUTPUT_TEXT]->SetPosition({ FontContext::Component(18), {2, true} });
	m_texts[CURRENT_LAYER_TEXT]->SetPosition({ FontContext::Component(0), {0}, {7}, {14} });
	m_texts[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->SetPosition({ FontContext::Component(1), {0}, {7}, {14} });
	m_texts[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT]->SetPosition({ FontContext::Component(2), {0}, {7}, {14} });
	m_texts[CURRENT_LAYER_BIAS_TEXT]->SetPosition({ FontContext::Component(3), {0}, {7}, {14} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(4), {0}, {7}, {14}, {23} });
	m_texts[TOTAL_NUMBER_OF_LAYERS_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {8} });
	m_texts[TOTAL_NUMBER_OF_NEURONS_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {8} });
	m_texts[TOTAL_NUMBER_OF_WEIGHTS_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {8} });
	m_texts[TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {8} });

	CoreLogger::PrintSuccess("State \"Artificial Neural Network Editor\" dependencies loaded correctly");
	return true;
}

void StateANNEditor::Draw()
{
	for (size_t i = 0; i < m_weightPositions.size(); ++i)
	{
		m_weightShape[0].position = m_weightPositions[i][0];
		m_weightShape[1].position = m_weightPositions[i][1];
		m_weightShape[0].color = m_weightStrengths[i];
		m_weightShape[1].color = m_weightShape[0].color;
		CoreWindow::Draw(m_weightShape.data(), m_weightShape.size(), sf::Lines);
	}

	for (size_t layerNr = 0; layerNr < m_layersPositions.size(); ++layerNr)
	{
		m_neuronShape.setFillColor(m_currentLayer == layerNr ? sf::Color(0xFD, 0xDA, 0x0D, 0xFF) : sf::Color(0xE5, 0xE5, 0xE5, 0xFF));
		for (const auto& position : m_layersPositions[layerNr])
		{
			m_neuronShape.setPosition(position);
			CoreWindow::Draw(m_neuronShape);
		}
	}

	for (const auto& text : m_texts)
		text->Draw();
}

void StateANNEditor::CalculatePositions()
{
	m_layersPositions.clear();
	m_weightPositions.clear();
	m_weightStrengths.clear();

	auto windowSize = CoreWindow::GetWindowSize();
	const float screenWidth = windowSize.x;
	const float screenHeight = windowSize.y;
	const float availableWidth = screenWidth * 0.8f;
	const float availableHeight = screenHeight * 0.8f;

	// Find offset x
	const size_t numberOfLayers = m_neuronLayerSizes.size();
	const float offsetX = availableWidth / (numberOfLayers + 1);
	const float startX = (screenWidth / 2) - (offsetX * (numberOfLayers - 1)) / 2;

	// Add neuron shape positions
	m_totalNumberOfNeurons = 0;
	for (size_t layerNr = 0; layerNr < numberOfLayers; ++layerNr)
	{
		float x = startX + (offsetX * layerNr);
		const size_t numberOfNeurons = m_neuronLayerSizes[layerNr];
		const float offsetY = availableHeight / (numberOfNeurons + 1);
		const float startY = (screenHeight / 2) - (offsetY * (numberOfNeurons - 1)) / 2;

		std::vector<sf::Vector2f> neuronPositions;
		for (size_t neuronNr = 0; neuronNr < numberOfNeurons; ++neuronNr)
		{
			float y = startY + (offsetY * neuronNr);
			neuronPositions.push_back(sf::Vector2f(x, y));
			++m_totalNumberOfNeurons;
		}

		m_layersPositions.push_back(neuronPositions);
	}

	// Add weight shape positions
	for (size_t layerNr = 1; layerNr < numberOfLayers; ++layerNr)
	{
		const size_t numberOfNeurons = m_neuronLayerSizes[layerNr];
		for (size_t i = 0; i < numberOfNeurons; ++i)
		{
			const size_t previousLayerNumberOfNeurons = m_neuronLayerSizes[layerNr - 1];
			for (size_t j = 0; j < previousLayerNumberOfNeurons; ++j)
			{
				Edge edge;
				edge[0] = m_layersPositions[layerNr - 1][j];
				edge[1] = m_layersPositions[layerNr][i];
				m_weightPositions.push_back(edge);
				m_weightStrengths.push_back(GetWeightStrength(1.0, 1.0));
			}
		}
	}

	m_totalNumberOfWeights = m_weightPositions.size();

	// Normalize neuron shape positions
	for (auto& neurons : m_layersPositions)
	{
		for (auto& position : neurons)
		{
			position.x -= m_neuronShape.getRadius();
			position.y -= m_neuronShape.getRadius();
		}
	}

	if (!m_textObservers.empty())
	{
		m_textObservers[TOTAL_NUMBER_OF_NEURONS_TEXT]->Notify();
		m_textObservers[TOTAL_NUMBER_OF_WEIGHTS_TEXT]->Notify();
	}
}

void StateANNEditor::AddLayer()
{
	if (m_neuronLayerSizes.size() < ArtificialNeuralNetworkBuilder::GetMaxNumberOfLayers())
	{
		m_currentLayer = m_neuronLayerSizes.empty() ? 0 : m_currentLayer + 1;
		m_neuronLayerSizes.insert(m_neuronLayerSizes.begin() + m_currentLayer, 0);

		if (m_currentLayer != 0)
		{
			m_activationFunctionIndexes.insert(m_activationFunctionIndexes.begin() + (m_currentLayer - 1), ActivationFunctionContext::GetMinActivationFunctionIndex());
			m_biasVector.insert(m_biasVector.begin() + (m_currentLayer - 1), ArtificialNeuralNetworkBuilder::GetDefaultBiasValue());
			m_textObservers[TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT]->Notify();
		}

		// Add neuron to newly created neuron layer
		AddNeuron();

		// Notify observers
		m_textObservers[CURRENT_LAYER_TEXT]->Notify();
		m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->Notify();
		m_textObservers[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT]->Notify();
		m_textObservers[CURRENT_LAYER_BIAS_TEXT]->Notify();
		m_textObservers[TOTAL_NUMBER_OF_LAYERS_TEXT]->Notify();

		// Set it as not up to date
		m_upToDate = false;
	}
}

void StateANNEditor::RemoveLayer()
{
	if (!m_neuronLayerSizes.empty())
	{
		const size_t previousNumberOfNeuronLayers = m_neuronLayerSizes.size();
		m_neuronLayerSizes.erase(m_neuronLayerSizes.begin() + m_currentLayer);
		if (!m_neuronLayerSizes.empty())
		{
			auto correctedIndex = m_currentLayer == 0 ? m_currentLayer : (m_currentLayer - 1);
			m_activationFunctionIndexes.erase(m_activationFunctionIndexes.begin() + correctedIndex);
			m_biasVector.erase(m_biasVector.begin() + correctedIndex);
			m_textObservers[TOTAL_NUMBER_OF_ACTIVATION_FUNCTIONS_TEXT]->Notify();
		}

		if (m_currentLayer == previousNumberOfNeuronLayers - 1)
		{
			if (m_currentLayer > 0)
			{
				--m_currentLayer;
			}
		}

		// Notify observers
		m_textObservers[CURRENT_LAYER_TEXT]->Notify();
		m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->Notify();
		m_textObservers[CURRENT_LAYER_ACTIVATION_FUNCTION_TEXT]->Notify();
		m_textObservers[CURRENT_LAYER_BIAS_TEXT]->Notify();
		m_textObservers[TOTAL_NUMBER_OF_LAYERS_TEXT]->Notify();

		// Set it as not up to date
		m_upToDate = false;
	}
}

void StateANNEditor::AddNeuron()
{
	if (!m_neuronLayerSizes.empty())
	{
		if (m_neuronLayerSizes[m_currentLayer] + 1 <= ArtificialNeuralNetworkBuilder::GetMaxNumberOfNeuronsPerLayer())
		{
			++m_neuronLayerSizes[m_currentLayer];
			m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->Notify();
		}

		m_upToDate = false;
	}
}

void StateANNEditor::RemoveNeuron()
{
	if (!m_neuronLayerSizes.empty())
	{
		if (m_neuronLayerSizes[m_currentLayer] > 0)
		{
			--m_neuronLayerSizes[m_currentLayer];
			m_textObservers[CURRENT_LAYER_NUMBER_OF_NEURONS_TEXT]->Notify();
			if (m_neuronLayerSizes[m_currentLayer] == 0)
				RemoveLayer();
		}

		m_upToDate = false;
	}
}

sf::Color StateANNEditor::GetWeightStrength(double max, double value) const
{
	return sf::Color(255, 255, 255, 32 + sf::Uint8(128.0 * (value / max)));
}
