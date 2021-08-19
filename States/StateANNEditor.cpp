#include "StateANNEditor.hpp"
#include "CoreLogger.hpp"
#include "FunctionTimerObserver.hpp"
#include "TypeTimerObserver.hpp"

void StateANNEditor::CalculatePositions()
{
	m_layersPositions.clear();
	m_weightPositions.clear();

	const float screenWidth = float(CoreWindow::GetSize().x);
	const float screenHeight = float(CoreWindow::GetSize().y);
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
		}

		AddNeuron();
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
		}

		if (m_currentLayer == previousNumberOfNeuronLayers - 1)
		{
			if (m_currentLayer > 0)
				--m_currentLayer;
		}

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
			if (m_neuronLayerSizes[m_currentLayer] == 0)
				RemoveLayer();
		}

		m_upToDate = false;
	}
}

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

	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	m_neuronShape.setFillColor(sf::Color::White);
	m_neuronShape.setOutlineColor(sf::Color::White);
	m_neuronShape.setRadius(float(CoreWindow::GetSize().x) * 0.008f);
	m_weightShape[0].color = sf::Color(255, 255, 255, 128);
	m_weightShape[1].color = m_weightShape[0].color;
	m_textFunctions.reserve(16U);

	// Create dummy
	if (m_artificialNeuralNetworkBuilder.CreateDummy())
	{
		m_neuronLayerSizes = m_artificialNeuralNetworkBuilder.GetNeuronLayerSizes();
		m_activationFunctionIndexes = m_artificialNeuralNetworkBuilder.GetActivationFunctionIndexes();
		m_biasVector = m_artificialNeuralNetworkBuilder.GetBiasVector();
		m_currentLayer = 0;
		m_upToDate = false;
		CalculatePositions();
	}
}

StateANNEditor::~StateANNEditor()
{
}

void StateANNEditor::Reload()
{
	// Create dummy
	if (m_artificialNeuralNetworkBuilder.CreateDummy())
	{
		m_neuronLayerSizes = m_artificialNeuralNetworkBuilder.GetNeuronLayerSizes();
		m_activationFunctionIndexes = m_artificialNeuralNetworkBuilder.GetActivationFunctionIndexes();
		m_biasVector = m_artificialNeuralNetworkBuilder.GetBiasVector();
		m_currentLayer = 0;
		m_upToDate = false;
		CalculatePositions();
	}

	// Reset pressed keys
	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	// Reset texts
	m_currentLayerText.ResetObserverTimer();
	m_currentLayerNumberOfNeuronsText.ResetObserverTimer();
	m_currentLayerActivationFunctionText.ResetObserverTimer();
	m_currentLayerBiasText.ResetObserverTimer();
	m_filenameText.Reset();
	m_filenameText.ResetObserverTimer();
	m_totalNumberOfLayersText.ResetObserverTimer();
	m_totalNumberOfNeuronsText.ResetObserverTimer();
	m_totalNumberOfWeightsText.ResetObserverTimer();
	m_totalNumberOfActivationFunctionsText.ResetObserverTimer();

	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);
}

void StateANNEditor::Capture()
{
	m_filenameText.Capture();
	if (!m_filenameText.IsRenaming())
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
						++m_currentLayer;
						if (m_currentLayer >= m_neuronLayerSizes.size())
							m_currentLayer = 0;
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
						}
						break;
					case INCREASE_BIAS:
						if (!m_biasVector.empty() && m_currentLayer != 0)
						{
							m_biasVector[m_currentLayer - 1] += m_biasOffset;
							if (m_biasVector[m_currentLayer - 1] > ArtificialNeuralNetworkBuilder::GetMaxBiasValue())
								m_biasVector[m_currentLayer - 1] = ArtificialNeuralNetworkBuilder::GetMaxBiasValue();
							m_upToDate = false;
						}
						break;
					case DECREASE_BIAS:
						if (!m_biasVector.empty() && m_currentLayer != 0)
						{
							m_biasVector[m_currentLayer - 1] -= m_biasOffset;
							if (m_biasVector[m_currentLayer - 1] < ArtificialNeuralNetworkBuilder::GetMinBiasValue())
								m_biasVector[m_currentLayer - 1] = ArtificialNeuralNetworkBuilder::GetMinBiasValue();
							m_upToDate = false;
						}
						break;
				}
			}
		}

		if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end())
				m_pressedKeys[iterator->second] = false;
		}
	}
}

void StateANNEditor::Update()
{
	if (m_filenameText.IsWriting())
	{
		if (!m_upToDate)
		{
			m_artificialNeuralNetworkBuilder.Clear();
			m_artificialNeuralNetworkBuilder.SetNeuronLayerSizes(m_neuronLayerSizes);
			m_artificialNeuralNetworkBuilder.SetActivationFunctionIndexes(m_activationFunctionIndexes);
			m_artificialNeuralNetworkBuilder.SetBiasVector(m_biasVector);
			bool success = m_artificialNeuralNetworkBuilder.Save(m_filenameText.GetFilename());
			std::string message;
			auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();
			m_filenameText.ShowStatusText();
			if (success)
				m_filenameText.SetSuccessStatusText(status.second);
			else
				m_filenameText.SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (m_filenameText.IsReading())
	{
		bool success = m_artificialNeuralNetworkBuilder.Load(m_filenameText.GetFilename());
		auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();
		m_filenameText.ShowStatusText();
		if (success)
		{
			m_filenameText.SetSuccessStatusText(status.second);
			m_neuronLayerSizes = m_artificialNeuralNetworkBuilder.GetNeuronLayerSizes();
			m_activationFunctionIndexes = m_artificialNeuralNetworkBuilder.GetActivationFunctionIndexes();
			m_biasVector = m_artificialNeuralNetworkBuilder.GetBiasVector();
			m_currentLayer = 0;
			m_upToDate = true;
			CalculatePositions();
		}
		else
			m_filenameText.SetErrorStatusText(status.second);
	}

	m_inputText.Update();
	m_outputText.Update();
	m_currentLayerText.Update();
	m_currentLayerNumberOfNeuronsText.Update();
	m_currentLayerActivationFunctionText.Update();
	m_currentLayerBiasText.Update();
	m_filenameText.Update();
	m_totalNumberOfLayersText.Update();
	m_totalNumberOfNeuronsText.Update();
	m_totalNumberOfWeightsText.Update();
	m_totalNumberOfActivationFunctionsText.Update();
}

bool StateANNEditor::Load()
{
	// Set consistent texts
	m_inputText.SetText("Input");
	m_outputText.SetText("Output");
	m_currentLayerText.SetConsistentText("Current layer:");
	m_currentLayerNumberOfNeuronsText.SetConsistentText("Current layer number of neurons:");
	m_currentLayerActivationFunctionText.SetConsistentText("Current layer activation function:");
	m_currentLayerBiasText.SetConsistentText("Current layer bias:");
	m_totalNumberOfLayersText.SetConsistentText("Total number of layers:");
	m_totalNumberOfNeuronsText.SetConsistentText("Total number of neurons:");
	m_totalNumberOfWeightsText.SetConsistentText("Total number of weights:");
	m_totalNumberOfActivationFunctionsText.SetConsistentText("Total number of activation functions:");

	// Set variable texts
	m_textFunctions.push_back([&] { return m_neuronLayerSizes.empty() ? "None" : ("L" + std::to_string(m_currentLayer)); });
	m_currentLayerText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));
	m_textFunctions.push_back([&] { return m_neuronLayerSizes.empty() ? "None" : std::to_string(m_neuronLayerSizes[m_currentLayer]); });
	m_currentLayerNumberOfNeuronsText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));
	m_textFunctions.push_back([&] {
		if (m_activationFunctionIndexes.empty() || m_currentLayer == 0)
			return std::string("None");
		return ActivationFunctionContext::GetString(m_activationFunctionIndexes[m_currentLayer - 1]);
	});
	m_currentLayerActivationFunctionText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.3));
	m_textFunctions.push_back([&] {
		if (m_biasVector.empty() || m_currentLayer == 0)
			return std::string("None");
		return std::to_string(m_biasVector[m_currentLayer - 1]);
	});
	m_currentLayerBiasText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return std::to_string(m_neuronLayerSizes.size()); });
	m_totalNumberOfLayersText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.5));
	m_totalNumberOfNeuronsText.SetObserver(new TypeTimerObserver<size_t>(m_totalNumberOfNeurons, 0.2));
	m_totalNumberOfWeightsText.SetObserver(new TypeTimerObserver<size_t>(m_totalNumberOfWeights, 0.2));
	m_textFunctions.push_back([&] { return std::to_string(m_activationFunctionIndexes.size()); });
	m_totalNumberOfActivationFunctionsText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.5));

	// Set information texts
	m_currentLayerText.SetInformationText("| [Tab] [Enter] [Backspace]");
	m_currentLayerNumberOfNeuronsText.SetInformationText("| [+] [-]");
	m_currentLayerActivationFunctionText.SetInformationText("| [*]");
	m_currentLayerBiasText.SetInformationText("| [Z] [X]");

	// Set text character size and rotation
	m_inputText.SetCharacterSize(4);
	m_outputText.SetCharacterSize(4);
	m_inputText.SetRotation(270.0f);
	m_outputText.SetRotation(90.0f);

	// Set text positions
	m_inputText.SetPosition({ FontContext::Component(2), {26} });
	m_outputText.SetPosition({ FontContext::Component(2, true), {18} });
	m_currentLayerText.SetPosition({ FontContext::Component(0), {7}, {14}, {0} });
	m_currentLayerNumberOfNeuronsText.SetPosition({ FontContext::Component(0), {7}, {14}, {1} });
	m_currentLayerActivationFunctionText.SetPosition({ FontContext::Component(0), {7}, {14}, {2} });
	m_currentLayerBiasText.SetPosition({ FontContext::Component(0), {7}, {14}, {3} });
	m_filenameText.SetPosition({ FontContext::Component(0), {7}, {14}, {23}, {4} });
	m_totalNumberOfLayersText.SetPosition({ FontContext::Component(0), {8}, {4, true} });
	m_totalNumberOfNeuronsText.SetPosition({ FontContext::Component(0), {8}, {3, true} });
	m_totalNumberOfWeightsText.SetPosition({ FontContext::Component(0), {8}, {2, true} });
	m_totalNumberOfActivationFunctionsText.SetPosition({ FontContext::Component(0), {8}, {1, true} });

	CoreLogger::PrintSuccess("State \"Artificial Neural Network Editor\" dependencies loaded correctly");
	return true;
}

void StateANNEditor::Draw()
{
	for (const auto& position : m_weightPositions)
	{
		m_weightShape[0].position = position[0];
		m_weightShape[1].position = position[1];
		CoreWindow::GetRenderWindow().draw(m_weightShape.data(), 2, sf::Lines);
	}

	for (size_t layerNr = 0; layerNr < m_layersPositions.size(); ++layerNr)
	{
		m_neuronShape.setFillColor(m_currentLayer == layerNr ? sf::Color::Yellow : sf::Color::White);
		for (const auto& position : m_layersPositions[layerNr])
		{
			m_neuronShape.setPosition(position);
			CoreWindow::GetRenderWindow().draw(m_neuronShape);
		}
	}

	m_inputText.Draw();
	m_outputText.Draw();
	m_currentLayerText.Draw();
	m_currentLayerNumberOfNeuronsText.Draw();
	m_currentLayerActivationFunctionText.Draw();
	m_currentLayerBiasText.Draw();
	m_filenameText.Draw();
	m_totalNumberOfLayersText.Draw();
	m_totalNumberOfNeuronsText.Draw();
	m_totalNumberOfWeightsText.Draw();
	m_totalNumberOfActivationFunctionsText.Draw();
}