#pragma once
#include "StateAbstract.hpp"
#include "GeneticAlgorithm.hpp"
#include "DrawableManager.hpp"
#include "DrawableBuilder.hpp"
#include <SFML/Graphics/Text.hpp>

class StateTraining : public StateAbstract
{
	GeneticAlgorithmNeuron* m_evolution;
	std::vector<ArtificialNeuralNetwork> m_brains;
	DrawableBuilder m_builder;
	DrawableManager* m_manager;
	DrawableCarFactory m_carFactory;
	FitnessPoints m_fitnessPoints;
	FitnessPoints m_previousFitnessPoints;
	const double m_meanFitnessConst = 0.02;

	const size_t m_populationSize = 10;
	const size_t m_numberOfGenerations = 30;
	size_t m_generationNumber;

	// Wave
	std::vector<double> m_carTimers;
	const double m_waveTimerConst = 4.0;
	double m_waveTimer;
	
	// View
	const double m_viewTimerConst = 0.1; // Update view every 0.1 of a second
	double m_viewTimer;
	sf::Vector2f m_viewCenter;
	const float m_viewMovementConst = 500.0f; // Update view every 0.25 of a second

	// Texts
	sf::Font m_font;
	sf::Text m_generationText;
	sf::Text m_generationActiveText;

	void updateTextPositions();

public:
	StateTraining(StateTraining&) = delete;

	StateTraining();

	~StateTraining();

	void update();

	void load();

	void draw();
};
