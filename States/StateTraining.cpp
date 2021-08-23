#pragma once
#include "StateTraining.hpp"
#include "TypeTimerObserver.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "DrawableMap.hpp"
#include "DrawableCheckpointMap.hpp"
#include "GeneticAlgorithm.hpp"
#include "CoreLogger.hpp"

StateTraining::StateTraining() :
	m_waveTimer(0.0, 4.0),
	m_viewTimer(1.0, 0.1),
	m_viewMovementOffset(500.0)
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

	m_generation = 1;
	m_textFunctions.resize(16U);
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

}

void StateTraining::Update()
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
			// Finish
			std::cout << "Stop\n";
		}
	}
	else
	{
		m_drawableCheckpointMap->IncrementTimers();

		if (m_viewTimer.Increment())
		{
			auto index = m_drawableCheckpointMap->MarkLeader(m_drawableVehicleFactory);
			m_viewCenter = m_drawableVehicleFactory[index]->GetCenter();
		}

		if (m_waveTimer.Increment())
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
}

bool StateTraining::Load()
{
	/*
	if (!m_drawableMapBuilder.Load())
	{

		return false;
	}

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

	m_populationText.SetConsistentText("Population size:");
	m_generationText.SetConsistentText("Generation:");
	m_highestFitnessText.SetConsistentText("Highest fitness:");
	m_highestFitnessOverallText.SetConsistentText("Highest fitness overall:");
	m_populationText.SetPosition(0.0039, 0.078, 0.005);
	m_generationText.SetPosition(0.0039, 0.055, 0.025);
	m_highestFitnessText.SetPosition(0.0039, 0.078, 0.045);
	m_highestFitnessOverallText.SetPosition(0.0039, 0.12, 0.065);
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

		if(!vehicle->IsActive())
			continue;

		vehicle->DrawBeams();
	}

	m_drawableMap->Draw();

	m_populationText.Draw();
	m_generationText.Draw();
	m_highestFitnessText.Draw();
	m_highestFitnessOverallText.Draw();
}
