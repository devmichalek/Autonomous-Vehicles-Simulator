#pragma once
#include "StateTraining.hpp"
#include "TypeObserver.hpp"

StateTraining::~StateTraining()
{
	delete m_evolution;
	delete m_manager;
	for (auto& i : m_carFactory)
		delete std::get<0>(i);
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
		m_manager->calculateFitness(m_carFactory, m_fitnessVector, m_carTimers);
		if (!m_evolution->iterate(m_fitnessVector))
		{
			// Finish

		}
		++m_generationNumber;

		for (size_t i = 0; i < m_brains.size(); ++i)
			m_brains[i].setData(m_evolution->getIndividual(i));

		for (size_t i = 0; i < m_carFactory.size(); ++i)
		{
			delete m_carFactory[i].first;
			m_carFactory[i].first = m_builder.getDrawableCar();
			m_carFactory[i].second = true;
			m_fitnessVector[i] = 0;
			m_previousFitnessVector[i] = 0;
			m_carTimers[i] = 0;
		}
	}
	else
	{
		double elapsedTime = CoreWindow::getElapsedTime();
		for (auto &timer : m_carTimers)
			timer += elapsedTime;

		if (m_viewTimer.increment())
		{
			for (size_t i = 0; i < m_carFactory.size(); ++i)
			{
				m_fitnessVector[i] = 0;
				if (!m_carFactory[i].second)
					continue;
				m_manager->calculateFitness(m_carFactory[i], m_fitnessVector[i]);
				m_carFactory[i].first->setFollowerColor();
			}

			auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
			auto index = std::distance(m_fitnessVector.begin(), iterator);
			m_viewCenter = m_carFactory[index].first->getCenter();
			m_carFactory[index].first->setLeaderColor();
		}

		if (m_waveTimer.increment())
		{
			// Check if car has made improvement
			// If car has made improvement then it is not punished
			for (size_t i = 0; i < m_carFactory.size(); ++i)
			{
				if (!m_carFactory[i].second)
					continue;
				m_manager->calculateFitness(m_carFactory[i], m_fitnessVector[i]);
				Fitness requiredFitness = m_previousFitnessVector[i];
				requiredFitness += static_cast<Fitness>(double(m_manager->getMaxFitness()) * m_meanFitnessConst);
				if (requiredFitness > m_fitnessVector[i])
					m_carFactory[i].second = false;
				m_previousFitnessVector[i] = requiredFitness;
			}
		}
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
}

void StateTraining::load()
{
	m_populationText.setConsistentText("Population size:");
	m_generationText.setConsistentText("Generation:");
	m_populationText.setPosition(0.0039, 0.078, 0.005);
	m_generationText.setPosition(0.0039, 0.055, 0.023);
	m_populationText.setVariableText(std::to_string(m_populationSize));
	m_generationText.setObserver(new TypeObserver(m_generationNumber, 0.2, "", "/" + std::to_string(m_numberOfGenerations)));

	if (m_builder.load())
	{
		m_manager = m_builder.getDrawableManager();
		
		for (size_t i = 0; i < m_populationSize; ++i)
		{
			DrawableCar* car = m_builder.getDrawableCar();
			m_carFactory.push_back(std::pair(car, true));
		}
		m_fitnessVector.resize(m_populationSize, 0);
		m_previousFitnessVector.resize(m_populationSize, 0);
		m_carTimers.resize(m_populationSize, 0);

		std::vector<size_t> hiddenLayersSizes = { 5, 4 };
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
}

void StateTraining::draw()
{
	for (auto& car : m_carFactory)
	{
		if (!car.second)
			continue;
		car.first->draw();
	}

	m_manager->drawFinishLine();
	m_manager->drawEdges();

	m_populationText.draw();
	m_generationText.draw();
}
