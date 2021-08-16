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
	for (auto& i : m_carFactory)
		delete std::get<0>(i);
	delete m_checkpointMap;
}

void StateTraining::Reload()
{

}

void StateTraining::Capture()
{

}

void StateTraining::Update()
{
	bool activity = false;
	for (size_t i = 0; i < m_carFactory.size(); ++i)
	{
		if (!m_carFactory[i].second)
			continue;
		activity = true;
		NeuronLayer output = m_brains[i].Update(m_carFactory[i].first->processOutput());
		m_carFactory[i].first->processInput(output);
		m_carFactory[i].first->update();
	}

	m_edgeManager->Intersect(m_carFactory);

	if (!activity)
	{
		// Calculate fitness and highest fitness overall
		m_checkpointMap->iterate(m_carFactory);

		// Generate new generation
		if (m_evolution->iterate(m_checkpointMap->getFitnessVector()))
		{
			++m_generationNumber;

			for (size_t i = 0; i < m_brains.size(); ++i)
				m_brains[i].SetFromRawData(m_evolution->getIndividual(i));

			for (size_t i = 0; i < m_carFactory.size(); ++i)
			{
				delete m_carFactory[i].first;
				m_carFactory[i].first = m_builder.GetDrawableCar();
				m_carFactory[i].first->init(m_annNumberOfInputs);
				m_carFactory[i].second = true;
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
			auto index = m_checkpointMap->markLeader(m_carFactory);
			m_viewCenter = m_carFactory[index].first->getCenter();
		}

		if (m_waveTimer.Increment())
			m_checkpointMap->punish(m_carFactory);
	}

	auto& view = CoreWindow::GetView();
	auto currentViewCenter = view.getCenter();
	auto distance = Distance(currentViewCenter, m_viewCenter);
	auto angle = DifferenceVectorAngle(currentViewCenter, m_viewCenter);
	auto newCenter = GetEndPoint(currentViewCenter, angle, float(-distance / m_viewMovementConst));
	view.setCenter(newCenter);
	CoreWindow::GetRenderWindow().setView(view);

	m_populationText.Update();
	m_generationText.Update();
	m_highestFitnessText.Update();
	m_highestFitnessOverallText.Update();
}

bool StateTraining::Load()
{
	/*if (!m_builder.Load())
	{

		return false;
	}

	m_checkpointMap = m_builder.GetDrawableCheckpointMap();
	m_checkpointMap->restart(m_populationSize, 0.02);

	m_edgeManager = m_builder.GetDrawableManager();
	
	for (size_t i = 0; i < m_populationSize; ++i)
	{
		DrawableCar* car = m_builder.GetDrawableCar();
		car->init(m_annNumberOfInputs);
		m_carFactory.push_back(std::pair(car, true));
	}

	std::vector<size_t> hiddenLayersSizes = { 12, 12 };
	ArtificialNeuralNetwork ann(m_annNumberOfInputs, CAR_NUMBER_OF_INPUTS, hiddenLayersSizes);
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
	for (auto& car : m_carFactory)
	{
		car.first->drawBody();
		if(!car.second)
			continue;
		car.first->drawBeams();
	}

	m_edgeManager->Draw();

	m_populationText.Draw();
	m_generationText.Draw();
	m_highestFitnessText.Draw();
	m_highestFitnessOverallText.Draw();
}
