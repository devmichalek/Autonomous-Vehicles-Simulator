#pragma once
#include "StateTraining.hpp"
#include "TypeTimerObserver.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "DrawableEdgeManager.hpp"
#include "DrawableCheckpointMap.hpp"
#include "GeneticAlgorithm.hpp"
#include "CoreLogger.hpp"

StateTraining::~StateTraining()
{
	delete m_evolution;
	delete m_edgeManager;
	for (auto& i : m_vehicleFactory)
		delete std::get<0>(i);
	delete m_checkpointMap;
}

void StateTraining::Reload()
{
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
	for (size_t i = 0; i < m_vehicleFactory.size(); ++i)
	{
		if (!m_vehicleFactory[i].second)
			continue;
		activity = true;
		NeuronLayer output = m_brains[i].Update(m_vehicleFactory[i].first->ProcessOutput());
		m_vehicleFactory[i].first->ProcessInput(output);
		m_vehicleFactory[i].first->Update();
	}

	m_edgeManager->Intersect(m_vehicleFactory);

	if (!activity)
	{
		// Calculate fitness and highest fitness overall
		m_checkpointMap->iterate(m_vehicleFactory);

		// Generate new generation
		if (m_evolution->Iterate(m_checkpointMap->getFitnessVector()))
		{
			++m_generationNumber;

			for (size_t i = 0; i < m_brains.size(); ++i)
				m_brains[i].SetFromRawData(m_evolution->GetIndividual(i));

			for (size_t i = 0; i < m_vehicleFactory.size(); ++i)
			{
				delete m_vehicleFactory[i].first;
				m_vehicleFactory[i] = std::pair(m_drawableVehicleBuilder.Get(), true);
				auto details = m_drawableMapBuilder.GetVehicle();
				m_vehicleFactory[i].first->SetCenter(details.first);
				m_vehicleFactory[i].first->SetAngle(details.second);
				m_vehicleFactory[i].first->Update();
			}

			m_checkpointMap->reset();
		}
		else
		{
			// Finish
			std::cout << "Stop\n";
		}
	}
	else
	{
		m_checkpointMap->incrementTimers();

		if (m_viewTimer.Increment())
		{
			auto index = m_checkpointMap->markLeader(m_vehicleFactory);
			m_viewCenter = m_vehicleFactory[index].first->GetCenter();
		}

		if (m_waveTimer.Increment())
			m_checkpointMap->punish(m_vehicleFactory);
	}

	auto& view = CoreWindow::GetView();
	auto currentViewCenter = view.getCenter();
	auto distance = DrawableMath::Distance(currentViewCenter, m_viewCenter);
	auto angle = DrawableMath::DifferenceVectorAngle(currentViewCenter, m_viewCenter);
	auto newCenter = DrawableMath::GetEndPoint(currentViewCenter, angle, float(-distance / m_viewMovementConst));
	view.setCenter(newCenter);
	CoreWindow::GetRenderWindow().setView(view);

	m_populationText.Update();
	m_generationText.Update();
	m_highestFitnessText.Update();
	m_highestFitnessOverallText.Update();
}

bool StateTraining::Load()
{
	/*if (!m_drawableMapBuilder.Load())
	{

		return false;
	}

	m_checkpointMap = m_drawableMapBuilder.GetDrawableCheckpointMap();
	m_checkpointMap->restart(m_populationSize, 0.02);

	m_edgeManager = m_drawableMapBuilder.GetDrawableManager();
	
	for (size_t i = 0; i < m_populationSize; ++i)
	{
		DrawableVehicle* vehicle = m_drawableVehicleBuilder.Get();
		vehicle->Init();
		m_vehicleFactory.push_back(std::pair(vehicle, true));
	}

	std::vector<size_t> hiddenLayersSizes = { 12, 12 };
	ArtificialNeuralNetwork ann(m_annNumberOfInputs, DrawableVehicle::GetNumberOfInputs(), hiddenLayersSizes);
	ann.setBiasVector({ 0.25, 0.1, 0.05 });
	ann.setActivationVector({ ActivationLeakyRelu, ActivationTanh, ActivationRelu });
	m_brains.resize(m_populationSize, ann);

	const size_t chromosomeLength = ann.GetNumberOfWeights();
	double crossoverProbability = 0.5;
	double mutationProbability = 0.05;
	bool decreaseMutationOverGenerations = false;
	bool singlePointCrossover = false;
	unsigned int precision = 1000;
	std::pair<Neuron, Neuron> range = std::pair(-1.0, 1.0);
	m_evolution = new GeneticAlgorithmNeuron(m_numberOfGenerations,
												chromosomeLength,
												m_populationSize,
												crossoverProbability,
												mutationProbability,
												decreaseMutationOverGenerations,
												singlePointCrossover,
												precision,
												range);
	for (size_t i = 0; i < m_brains.size(); ++i)
		m_brains[i].setData(m_evolution->getIndividual(i));

	m_populationText.SetConsistentText("Population size:");
	m_generationText.SetConsistentText("Generation:");
	m_highestFitnessText.SetConsistentText("Highest fitness:");
	m_highestFitnessOverallText.SetConsistentText("Highest fitness overall:");
	m_populationText.SetPosition(0.0039, 0.078, 0.005);
	m_generationText.SetPosition(0.0039, 0.055, 0.025);
	m_highestFitnessText.SetPosition(0.0039, 0.078, 0.045);
	m_highestFitnessOverallText.SetPosition(0.0039, 0.12, 0.065);
	m_populationText.SetVariableText(std::to_string(m_populationSize));
	m_generationText.SetObserver(new TypeTimerObserver(m_generationNumber, 0.2, "", "/" + std::to_string(m_numberOfGenerations)));
	m_highestFitnessText.SetObserver(new TypeTimerObserver(m_checkpointMap->getHighestFitness(), 0.1));
	m_highestFitnessOverallText.SetObserver(new TypeTimerObserver(m_checkpointMap->getHighestFitnessOverall(), 0.5));*/

	CoreLogger::PrintSuccess("State \"Training\" dependencies loaded correctly");
	return true;
}

void StateTraining::Draw()
{
	for (auto& vehicle : m_vehicleFactory)
	{
		vehicle.first->DrawBody();
		if(!vehicle.second)
			continue;
		vehicle.first->DrawBeams();
	}

	m_edgeManager->Draw();

	m_populationText.Draw();
	m_generationText.Draw();
	m_highestFitnessText.Draw();
	m_highestFitnessOverallText.Draw();
}
