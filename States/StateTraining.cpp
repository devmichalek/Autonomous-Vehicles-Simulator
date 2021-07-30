#pragma once
#include "StateTraining.hpp"
#include "TypeObserver.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "DrawableManager.hpp"
#include "DrawableCheckpointMap.hpp"
#include "GeneticAlgorithm.hpp"

StateTraining::~StateTraining()
{
	delete m_evolution;
	delete m_manager;
	for (auto& i : m_carFactory)
		delete std::get<0>(i);
	delete m_checkpointMap;
}

void StateTraining::update()
{
	bool activity = false;
	for (size_t i = 0; i < m_carFactory.size(); ++i)
	{
		if (!m_carFactory[i].second)
			continue;
		activity = true;
		NeuronLayer output = m_brains[i].calculate(m_carFactory[i].first->processOutput());
		m_carFactory[i].first->processInput(output);
		m_carFactory[i].first->update();
	}

	m_manager->intersect(m_carFactory);

	if (!activity)
	{
		// Calculate fitness and highest fitness overall
		m_checkpointMap->iterate(m_carFactory, m_manager->getFinishLine());

		// Generate new generation
		if (m_evolution->iterate(m_checkpointMap->getFitnessVector()))
		{
			++m_generationNumber;

			for (size_t i = 0; i < m_brains.size(); ++i)
				m_brains[i].setData(m_evolution->getIndividual(i));

			for (size_t i = 0; i < m_carFactory.size(); ++i)
			{
				delete m_carFactory[i].first;
				m_carFactory[i].first = m_builder.getDrawableCar();
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

		if (m_viewTimer.increment())
		{
			auto index = m_checkpointMap->markLeader(m_carFactory, m_manager->getFinishLine());
			m_viewCenter = m_carFactory[index].first->getCenter();
		}

		if (m_waveTimer.increment())
			m_checkpointMap->punish(m_carFactory, m_manager->getFinishLine());
	}

	auto& view = CoreWindow::getView();
	auto currentViewCenter = view.getCenter();
	auto distance = Distance(currentViewCenter, m_viewCenter);
	auto angle = Angle(currentViewCenter, m_viewCenter);
	auto newCenter = GetEndPoint(currentViewCenter, angle, -distance / m_viewMovementConst);
	view.setCenter(newCenter);
	CoreWindow::getRenderWindow().setView(view);

	m_populationText.update();
	m_generationText.update();
	m_highestFitnessText.update();
	m_highestFitnessOverallText.update();
}

void StateTraining::load()
{
	if (m_builder.load())
	{
		m_checkpointMap = m_builder.getDrawableCheckpointMap();
		m_checkpointMap->restart(m_populationSize, 0.02);

		m_manager = m_builder.getDrawableManager();
		
		for (size_t i = 0; i < m_populationSize; ++i)
		{
			DrawableCar* car = m_builder.getDrawableCar();
			m_carFactory.push_back(std::pair(car, true));
		}

		std::vector<size_t> hiddenLayersSizes = { 6, 6 };
		ArtificialNeuralNetwork ann(CAR_NUMBER_OF_SENSORS, CAR_NUMBER_OF_INPUTS, hiddenLayersSizes);
		ann.setBiasVector({ 0.25, 0.1, 0.05 });
		ann.setActivationVector({ activationLeakyrelu, activationTanh, activationRelu });
		m_brains.resize(m_populationSize, ann);

		const size_t chromosomeLength = ann.getDataUnitsCount();
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
	}

	m_populationText.setConsistentText("Population size:");
	m_generationText.setConsistentText("Generation:");
	m_highestFitnessText.setConsistentText("Highest fitness:");
	m_highestFitnessOverallText.setConsistentText("Highest fitness overall:");
	m_populationText.setPosition(0.0039, 0.078, 0.005);
	m_generationText.setPosition(0.0039, 0.055, 0.025);
	m_highestFitnessText.setPosition(0.0039, 0.078, 0.045);
	m_highestFitnessOverallText.setPosition(0.0039, 0.12, 0.065);
	m_populationText.setVariableText(std::to_string(m_populationSize));
	m_generationText.setObserver(new TypeObserver(m_generationNumber, 0.2, "", "/" + std::to_string(m_numberOfGenerations)));
	m_highestFitnessText.setObserver(new TypeObserver(m_checkpointMap->getHighestFitness(), 0.1));
	m_highestFitnessOverallText.setObserver(new TypeObserver(m_checkpointMap->getHighestFitnessOverall(), 0.5));
}

void StateTraining::draw()
{
	for (auto& car : m_carFactory)
	{
		car.first->drawBody();
		if(!car.second)
			continue;
		car.first->drawBeams();
	}

	m_manager->drawFinishLine();
	m_manager->drawEdges();

	m_populationText.draw();
	m_generationText.draw();
	m_highestFitnessText.draw();
	m_highestFitnessOverallText.draw();
}
