#pragma once
#include "StateTraining.hpp"

void StateTraining::updateTextPositions()
{
	sf::Vector2f viewOffset = CoreWindow::getViewOffset();
	float textW = float(CoreWindow::getSize().x) / 256;
	float textX = viewOffset.x + textW;
	float textY = viewOffset.y + textW;
	m_generationText.setPosition(textX, textY);
	m_generationActiveText.setPosition(textX + float(CoreWindow::getSize().x) / 18, textY);
}

StateTraining::StateTraining()
{
	m_evolution = nullptr;
	m_manager = nullptr;
	m_generationNumber = 0;
	m_waveTimer = 0;
	m_viewTimer = m_viewTimerConst + 1;
}

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
		m_manager->calculateFitness(m_carFactory, m_fitnessPoints, m_carTimers);
		if (!m_evolution->iterate(m_fitnessPoints))
		{
			// Finish

		}
		m_generationActiveText.setString(std::to_string(++m_generationNumber));

		for (size_t i = 0; i < m_brains.size(); ++i)
			m_brains[i].setData(m_evolution->getIndividual(i));

		for (size_t i = 0; i < m_carFactory.size(); ++i)
		{
			delete m_carFactory[i].first;
			m_carFactory[i].first = m_builder.getDrawableCar();
			m_carFactory[i].second = true;
			m_fitnessPoints[i] = 0;
			m_previousFitnessPoints[i] = 0;
			m_carTimers[i] = 0;
		}
	}
	else
	{
		double elapsedTime = CoreWindow::getElapsedTime();
		m_viewTimer += elapsedTime;
		m_waveTimer += elapsedTime;
		for (auto &timer : m_carTimers)
			timer += elapsedTime;

		if (m_viewTimer > m_viewTimerConst)
		{
			m_viewTimer = 0;
			for (size_t i = 0; i < m_carFactory.size(); ++i)
			{
				m_fitnessPoints[i] = 0;
				if (!m_carFactory[i].second)
					continue;
				m_manager->calculateFitness(m_carFactory[i], m_fitnessPoints[i]);
				m_carFactory[i].first->setFollowerColor();
			}

			auto iterator = std::max_element(m_fitnessPoints.begin(), m_fitnessPoints.end());
			auto index = std::distance(m_fitnessPoints.begin(), iterator);
			m_viewCenter = m_carFactory[index].first->getCenter();
			m_carFactory[index].first->setLeaderColor();
		}

		if (m_waveTimer > m_waveTimerConst)
		{
			m_waveTimer = 0;

			// Check if car has made improvement
			// If car has made improvement then it is not punished
			for (size_t i = 0; i < m_carFactory.size(); ++i)
			{
				if (!m_carFactory[i].second)
					continue;
				m_manager->calculateFitness(m_carFactory[i], m_fitnessPoints[i]);
				FitnessPoint requiredFitness = m_previousFitnessPoints[i];
				requiredFitness += (m_manager->getMaxFitness() * m_meanFitnessConst);
				if (requiredFitness > m_fitnessPoints[i])
					m_carFactory[i].second = false;
				m_previousFitnessPoints[i] = requiredFitness;
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
	updateTextPositions();
}

void StateTraining::load()
{
	if (m_font.loadFromFile("Data/consola.ttf"))
	{
		m_generationText.setFont(m_font);
		m_generationActiveText.setFont(m_font);

		auto activeColor = sf::Color(0xC0, 0xC0, 0xC0, 0xFF);
		m_generationActiveText.setFillColor(activeColor);

		unsigned int characterSize = CoreWindow::getSize().x / 116;
		m_generationText.setCharacterSize(characterSize);
		m_generationActiveText.setCharacterSize(characterSize);

		m_generationText.setString("Generation:");
		m_generationActiveText.setString(std::to_string(m_generationNumber));

		updateTextPositions();
	}

	if (m_builder.load())
	{
		m_manager = m_builder.getDrawableManager();
		
		for (size_t i = 0; i < m_populationSize; ++i)
		{
			DrawableCar* car = m_builder.getDrawableCar();
			m_carFactory.push_back(std::pair(car, true));
		}
		m_fitnessPoints.resize(m_populationSize, 0);
		m_previousFitnessPoints.resize(m_populationSize, 0);
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

	CoreWindow::getRenderWindow().draw(m_generationText);
	CoreWindow::getRenderWindow().draw(m_generationActiveText);
}
