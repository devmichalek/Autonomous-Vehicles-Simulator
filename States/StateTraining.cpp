#pragma once
#include "StateTraining.hpp"
#include "DrawableMap.hpp"
#include "DrawableCheckpointMap.hpp"
#include "GeneticAlgorithm.hpp"
#include "FunctionEventObserver.hpp"
#include "TypeEventObserver.hpp"
#include "CoreLogger.hpp"

StateTraining::StateTraining() :
	m_waveTimer(0.0, 4.0),
	m_viewTimer(1.0, 0.1),
	m_viewMovementOffset(500.0),
	m_minPopulationSize(4),
	m_maxPopulationSize(60),
	m_minNumberOfGenerations(5),
	m_maxNumberOfGenerations(300)
{
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_mode = STOPPED_MODE;

	m_filenameTypeStrings[MAP_FILENAME_TYPE] = "Map";
	m_filenameTypeStrings[ANN_FILENAME_TYPE] = "ANN";
	m_filenameTypeStrings[VEHICLE_FILENAME_TYPE] = "Vehicle";
	m_filenameType = MAP_FILENAME_TYPE;

	m_geneticAlgorithm = nullptr;
	m_artificialNeuralNetworks.clear();
	m_drawableMap = nullptr;
	m_drawableVehicleFactory.clear();
	m_drawableCheckpointMap = nullptr;

	m_populationSize = 30;
	m_numberOfGenerations = 30;
	m_population = 1;
	m_generation = 1;
	m_eventObservers.reserve(16U);
}

StateTraining::~StateTraining()
{
	delete m_geneticAlgorithm;
	for (auto& ann : m_artificialNeuralNetworks)
		delete ann;
	delete m_drawableMap;
	for (auto& vehicle : m_drawableVehicleFactory)
		delete vehicle;
	delete m_drawableCheckpointMap;
}

void StateTraining::Reload()
{
	m_mode = STOPPED_MODE;
	m_filenameType = MAP_FILENAME_TYPE;

	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);
}

void StateTraining::Capture()
{
	if (m_filenameText.IsRenaming())
		m_filenameText.Capture();
	else
	{
		switch (m_mode)
		{
			case STOPPED_MODE:
			{

				break;
			}
			case RUNNING_MODE:
			{
				break;
			}
			default:
				break;
		}
	}
}

void StateTraining::Update()
{
	m_modeText.Update();
	m_populationSizeText.Update();
	m_numberOfGenerationsText.Update();

	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			m_filenameText.Update();
			break;
		}
		case RUNNING_MODE:
		{
			bool activity = false;
			for (size_t i = 0; i < m_drawableVehicleFactory.size(); ++i)
			{
				if (!m_drawableVehicleFactory[i]->IsActive())
					continue;

				activity = true;
				const NeuronLayer& input = m_drawableVehicleFactory[i]->ProcessOutput();
				const NeuronLayer& output = m_artificialNeuralNetworks[i]->Update(input);
				m_drawableVehicleFactory[i]->ProcessInput(output);
				m_drawableVehicleFactory[i]->Update();
			}

			m_drawableMap->Intersect(m_drawableVehicleFactory);

			if (!activity)
			{
				m_drawableCheckpointMap->Iterate(m_drawableVehicleFactory);

				// Generate new generation
				if (m_geneticAlgorithm->Iterate(m_drawableCheckpointMap->GetFitnessVector()))
				{
					++m_generation;

					for (size_t i = 0; i < m_artificialNeuralNetworks.size(); ++i)
						m_artificialNeuralNetworks[i]->SetFromRawData(m_geneticAlgorithm->GetIndividual(i));

					for (size_t i = 0; i < m_drawableVehicleFactory.size(); ++i)
					{
						delete m_drawableVehicleFactory[i];
						m_drawableVehicleFactory[i] = m_drawableVehicleBuilder.Get();
						auto details = m_drawableMapBuilder.GetVehicle();
						m_drawableVehicleFactory[i]->SetCenter(details.first);
						m_drawableVehicleFactory[i]->SetAngle(details.second);
						m_drawableVehicleFactory[i]->Update();
					}

					m_drawableCheckpointMap->Reset();
				}
				else
				{
					
				}
			}
			else
			{
				m_drawableCheckpointMap->UpdateTimers();

				if (m_viewTimer.Update())
				{
					auto index = m_drawableCheckpointMap->MarkLeader(m_drawableVehicleFactory);
					m_viewCenter = m_drawableVehicleFactory[index]->GetCenter();
				}

				if (m_waveTimer.Update())
					m_drawableCheckpointMap->Punish(m_drawableVehicleFactory);
			}

			auto& view = CoreWindow::GetView();
			auto currentViewCenter = view.getCenter();
			auto distance = DrawableMath::Distance(currentViewCenter, m_viewCenter);
			auto angle = DrawableMath::DifferenceVectorAngle(currentViewCenter, m_viewCenter);
			auto newCenter = DrawableMath::GetEndPoint(currentViewCenter, angle, float(-distance / m_viewMovementOffset));
			view.setCenter(newCenter);
			CoreWindow::GetRenderWindow().setView(view);

			m_populationText.Update();
			m_generationText.Update();
			m_highestFitnessText.Update();
			m_highestFitnessOverallText.Update();
			break;
		}
		default:
			break;
	}
}

bool StateTraining::Load()
{
	// Set texts strings
	m_modeText.SetStrings({ "Mode:", "", "| [M]" });
	m_populationSizeText.SetStrings({ "Population size:", "", "| [Z] [X]" });
	m_numberOfGenerationsText.SetStrings({ "Number of generations:", "", "| [A] [S]" });
	m_populationText.SetStrings({ "Current population size:" });
	m_generationText.SetStrings({ "Current generation:" });
	m_highestFitnessText.SetStrings({ "Highest fitness:" });
	m_highestFitnessOverallText.SetStrings({ "Highest fitness overall:" });

	// Set variable texts
	m_eventObservers.push_back(new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; }));
	m_modeText.SetObserver(m_eventObservers.back());
	m_eventObservers.push_back(new TypeEventObserver<size_t>(m_populationSize));
	m_populationSizeText.SetObserver(m_eventObservers.back());
	m_eventObservers.push_back(new TypeEventObserver<size_t>(m_numberOfGenerations));
	m_numberOfGenerationsText.SetObserver(m_eventObservers.back());
	m_eventObservers.push_back(new TypeEventObserver<size_t>(m_population));
	m_populationText.SetObserver(m_eventObservers.back());
	m_eventObservers.push_back(new TypeEventObserver<size_t>(m_generation));
	m_generationText.SetObserver(m_eventObservers.back());
	/*m_eventObservers.push_back(new FunctionEventObserver<std::string>());
	m_highestFitnessText.SetObserver(m_eventObservers.back());
	m_eventObservers.push_back(new FunctionEventObserver<std::string>());
	m_highestFitnessOverallText.SetObserver(m_eventObservers.back());*/

	// Set texts positions
	m_modeText.SetPosition({ FontContext::Component(0), {0}, {3}, {7} });
	m_filenameText.SetPosition({ FontContext::Component(3), {0}, {3}, {7}, {13} });
	m_populationSizeText.SetPosition({ FontContext::Component(1), {0}, {3}, {7} });
	m_numberOfGenerationsText.SetPosition({ FontContext::Component(2), {0}, {3}, {7} });
	m_populationText.SetPosition({ FontContext::Component(3, true), {0}, {3} });
	m_generationText.SetPosition({ FontContext::Component(2, true), {0}, {3} });
	m_highestFitnessText.SetPosition({ FontContext::Component(1, true), {0}, {3} });
	m_highestFitnessOverallText.SetPosition({ FontContext::Component(0, true), {0}, {3} });

	/*
	m_drawableCheckpointMap = m_drawableMapBuilder.GetDrawableCheckpointMap();
	m_drawableCheckpointMap->Init(m_populationSize, 0.02);

	m_drawableMap = m_drawableMapBuilder.GetDrawableMap();
	
	for (size_t i = 0; i < m_populationSize; ++i)
	{
		DrawableVehicle* vehicle = m_drawableVehicleBuilder.Get();
		vehicle->Init();
		m_drawableVehicleFactory.push_back(std::pair(vehicle, true));
	}

	std::vector<size_t> hiddenLayersSizes = { 12, 12 };
	ArtificialNeuralNetwork ann(m_annNumberOfInputs, DrawableVehicle::GetNumberOfInputs(), hiddenLayersSizes);
	ann.setBiasVector({ 0.25, 0.1, 0.05 });
	ann.setActivationVector({ ActivationLeakyRelu, ActivationTanh, ActivationRelu });
	m_artificialNeuralNetworks.resize(m_populationSize, ann);

	const size_t chromosomeLength = ann.GetNumberOfWeights();
	double crossoverProbability = 0.5;
	double mutationProbability = 0.05;
	bool decreaseMutationOverGenerations = false;
	bool singlePointCrossover = false;
	unsigned int precision = 1000;
	std::pair<Neuron, Neuron> range = std::pair(-1.0, 1.0);
	m_geneticAlgorithm = new GeneticAlgorithmNeuron(m_numberOfGenerations,
												chromosomeLength,
												m_populationSize,
												crossoverProbability,
												mutationProbability,
												decreaseMutationOverGenerations,
												singlePointCrossover,
												precision,
												range);
	for (size_t i = 0; i < m_artificialNeuralNetworks.size(); ++i)
		m_artificialNeuralNetworks[i]->SetFromRawData(m_geneticAlgorithm->GetIndividual(i));

	m_populationText.SetVariableText(std::to_string(m_populationSize));
	m_generationText.SetObserver(new TypeTimerObserver(m_generation, 0.2, "", "/" + std::to_string(m_numberOfGenerations)));
	m_highestFitnessText.SetObserver(new TypeTimerObserver(m_drawableCheckpointMap->GetHighestFitness(), 0.1));
	m_highestFitnessOverallText.SetObserver(new TypeTimerObserver(m_drawableCheckpointMap->GetHighestFitnessOverall(), 0.5));*/

	CoreLogger::PrintSuccess("State \"Training\" dependencies loaded correctly");
	return true;
}

void StateTraining::Draw()
{
	for (auto& vehicle : m_drawableVehicleFactory)
	{
		vehicle->DrawBody();

		if (!vehicle->IsActive())
			continue;

		vehicle->DrawBeams();
	}

	if (m_drawableMap)
		m_drawableMap->Draw();

	m_modeText.Draw();
	m_populationSizeText.Draw();
	m_numberOfGenerationsText.Draw();

	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			m_filenameText.Draw();
			break;
		}
		case RUNNING_MODE:
		{
			m_populationText.Draw();
			m_generationText.Draw();
			m_highestFitnessText.Draw();
			m_highestFitnessOverallText.Draw();
			break;
		}
		default:
			break;
	}
}
