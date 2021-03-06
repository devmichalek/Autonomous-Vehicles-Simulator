#pragma once
#include "StateSimulation.hpp"
#include "FitnessSystem.hpp"
#include "GeneticAlgorithm.hpp"
#include "FunctionEventObserver.hpp"
#include "TypeEventObserver.hpp"
#include "FilenameText.hpp"
#include "CoreLogger.hpp"
#include "DrawableCheckpoint.hpp"
#include "SimulatedWorld.hpp"
#include "MapPrototype.hpp"

StateSimulation::StateSimulation() :
	m_population(12, 75, 1, 30),
	m_generation(1, 300, 1, 60),
	m_deathOnEdgeContact(false, true, true, true),
	m_crossoverType(UNIFORM_CROSSOVER, TWO_POINT_CROSSOVER, 1, UNIFORM_CROSSOVER),
	m_repeatCrossoverPerIndividual(false, true, true, true),
	m_mutationProbability(0.01, 0.99, 0.01, 0.05),
	m_decreaseMutationProbabilityOverGenerations(false, true, true, false),
	m_numberOfParents(1, 10, 1, 2),
	m_requiredFitnessImprovement(0.01, 0.2, 0.01, 0.05),
	m_requiredFitnessImprovementRise(1.0, 15.0, 0.5, 3.0),
	m_zoom(1.f, 4.f, 0.3f, 1.f),
	m_zoomThreshold(m_zoom.Max()),
	m_viewMovementOffset(3.0),
	m_viewTimer(1.0, 0.2),
	m_pressedKeyTimer(0.0, 1.0, 5000),
	m_requiredFitnessImprovementRiseTimer(0.0, 0.0)
{
	// Initialize modes
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_modeStrings[PAUSED_MODE] = "Paused";
	m_mode = STOPPED_MODE;

	// Initialize file formats
	m_fileFormatStrings[MAP_FILE_FORMAT] = "Map";
	m_fileFormatStrings[ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT] = "Artificial Neural Network";
	m_fileFormatStrings[VEHICLE_FILE_FORMAT] = "Vehicle";
	m_fileFormat = MAP_FILE_FORMAT;

	// Initialize file formats in paused mode
	m_fileFormatPausedStrings[ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED] = "Artificial Neural Network";
	m_fileFormatPausedStrings[STATISTICS_FILE_FORMAT_PAUSED] = "Statistics";
	m_fileFormatPaused = ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED;

	// Initialize parameter types
	m_parameterTypesStrings[POPULATION_SIZE] = "Population size";
	m_parameterTypesStrings[NUMBER_OF_GENERATIONS] = "Number of generations";
	m_parameterTypesStrings[DEATH_ON_EDGE_CONTACT] = "Death on edge contact";
	m_parameterTypesStrings[CROSSOVER_TYPE] = "Crossover type";
	m_parameterTypesStrings[REPEAT_CROSSOVER_PER_INDIVIDUAL] = "Repeat crossover per individual";
	m_parameterTypesStrings[MUTATION_PROBABILITY] = "Mutation probability";
	m_parameterTypesStrings[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS] = "Decrease mutation probability over generations";
	m_parameterTypesStrings[NUMBER_OF_PARENTS] = "Number of parents";
	m_parameterTypesStrings[REQUIRED_FITNESS_IMPROVEMENT_RISE] = "Required fitness improvement rise";
	m_parameterTypesStrings[REQUIRED_FITNESS_IMPROVEMENT] = "Required fitness improvement";
	m_parameterType = POPULATION_SIZE;

	// Initialize control keys
	m_controlKeys.insert(std::pair(sf::Keyboard::M, CHANGE_MODE));
	m_controlKeys.insert(std::pair(sf::Keyboard::F, CHANGE_FILENAME_TYPE));
	m_controlKeys.insert(std::pair(sf::Keyboard::P, CHANGE_SIMULATION_PARAMETER));
	m_controlKeys.insert(std::pair(sf::Keyboard::Add, INCREASE_PARAMETER));
	m_controlKeys.insert(std::pair(sf::Keyboard::Subtract, DECREASE_PARAMETER));
	m_controlKeys.insert(std::pair(sf::Keyboard::Multiply, INCREASE_ZOOM));
	m_controlKeys.insert(std::pair(sf::Keyboard::Divide, DECREASE_ZOOM));

	for (auto & pressedKey : m_pressedKeys)
		pressedKey = false;

	// Initialize internal errors
	m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED] = "Error: No artificial neural network is specified!";
	m_internalErrorsStrings[ERROR_NO_MAP_SPECIFIED] = "Error: No map is specified!";
	m_internalErrorsStrings[ERROR_NO_VEHICLE_SPECIFIED] = "Error: No vehicle is specified!";
	m_internalErrorsStrings[ERROR_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN] = "Error: Vehicle is in a collision with edges chain!";
	m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH] = "Error: Artificial neural network number of input neurons mismatches number of vehicle sensors!";

	// Initialize timers
	m_pressedKeyTimer.MakeTimeout();
	m_requiredFitnessImprovementRiseTimer.SetTimeout(m_requiredFitnessImprovementRise);

	// Initialize objects of environment
	m_geneticAlgorithm = nullptr;
	m_artificialNeuralNetworks.resize(m_population, nullptr);
	m_simulatedWorld = nullptr;
	m_fitnessSystem = nullptr;

	// Initialize prototypes
	m_artificialNeuralNetworkPrototype = nullptr;
	m_vehiclePrototype = nullptr;
	m_mapPrototype = nullptr;

	// Initialize text and their observers
	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateSimulation::~StateSimulation()
{
	delete m_geneticAlgorithm;
	for (const auto& artificialNeuralNetwork : m_artificialNeuralNetworks)
		delete artificialNeuralNetwork;
	delete m_simulatedWorld;
	delete m_fitnessSystem;
	delete m_artificialNeuralNetworkPrototype;
	delete m_vehiclePrototype;
	delete m_mapPrototype;
	for (const auto& text : m_texts)
		delete text;
	for (const auto& observer : m_textObservers)
		delete observer;
}

void StateSimulation::Reload()
{
	// Reset states
	m_mode = STOPPED_MODE;
	m_fileFormat = MAP_FILE_FORMAT;
	m_fileFormatPaused = ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED;
	m_parameterType = POPULATION_SIZE;

	// Reset pressed keys
	for (size_t i = 0; i < m_pressedKeys.size(); ++i)
		m_pressedKeys[i] = false;

	// Reset simulation properties
	m_population.ResetValue();
	m_generation.ResetValue();
	m_crossoverType.ResetValue();
	m_repeatCrossoverPerIndividual.ResetValue();
	m_mutationProbability.ResetValue();
	m_decreaseMutationProbabilityOverGenerations.ResetValue();
	m_numberOfParents.ResetValue();
	m_requiredFitnessImprovement.ResetValue();
	m_requiredFitnessImprovementRise.ResetValue();
	m_zoom.ResetValue();
	m_zoomThreshold = m_zoom.Max();

	// Reset timers
	m_viewTimer.Reset();
	m_pressedKeyTimer.MakeTimeout();
	m_requiredFitnessImprovementRiseTimer.SetTimeout(m_requiredFitnessImprovementRise);
	m_requiredFitnessImprovementRiseTimer.Reset();

	// Reset objects of environment
	delete m_geneticAlgorithm;
	m_geneticAlgorithm = nullptr;
	for (auto& artificialNeuralNetwork : m_artificialNeuralNetworks)
	{
		delete artificialNeuralNetwork;
		artificialNeuralNetwork = nullptr;
	}
	m_artificialNeuralNetworks.resize(m_population, nullptr);
	delete m_simulatedWorld;
	m_simulatedWorld = nullptr;
	delete m_fitnessSystem;
	m_fitnessSystem = nullptr;
	m_simulatedVehicles.clear();

	// Reset prototypes
	delete m_artificialNeuralNetworkPrototype;
	m_artificialNeuralNetworkPrototype = nullptr;
	delete m_vehiclePrototype;
	m_vehiclePrototype = nullptr;
	delete m_mapPrototype;
	m_mapPrototype = nullptr;

	// Reset builders
	m_artificialNeuralNetworkBuilder.Clear();
	m_mapBuilder.Clear();
	m_vehicleBuilder.Clear();
	m_statisticsBuilder.Clear();

	CoreWindow::Reset();

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}
}

void StateSimulation::Capture()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			auto* filenameText = static_cast<FilenameText<true, false>*>(m_texts[FILENAME_STOPPED_TEXT]);
			if (!filenameText->IsRenaming())
			{
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					const auto eventKey = CoreWindow::GetEvent().key.code;
					const auto iterator = m_controlKeys.find(eventKey);
					if (iterator != m_controlKeys.end())
					{
						switch (iterator->second)
						{
							case CHANGE_MODE:
							{
								if (m_pressedKeys[iterator->second])
									break;

								StatusText* modeText = static_cast<StatusText*>(m_texts[MODE_TEXT]);
								if (!m_artificialNeuralNetworkPrototype)
								{
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED]);
									break;
								}

								if (!m_mapPrototype)
								{
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_MAP_SPECIFIED]);
									break;
								}

								if (!m_vehiclePrototype)
								{
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_VEHICLE_SPECIFIED]);
									break;
								}

								if (m_mapPrototype->IsCollision(m_vehiclePrototype))
								{
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN]);
									break;
								}

								if (m_artificialNeuralNetworkPrototype->GetNumberOfInputNeurons() != m_vehiclePrototype->GetNumberOfSensors())
								{
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH]);
									break;
								}

								// Create artificial neural networks
								for (const auto& artificialNeuralNetwork : m_artificialNeuralNetworks)
									delete artificialNeuralNetwork;
								m_artificialNeuralNetworks.resize(m_population);
								for (auto& artificialNeuralNetwork : m_artificialNeuralNetworks)
									artificialNeuralNetwork = ArtificialNeuralNetworkBuilder::Copy(m_artificialNeuralNetworkPrototype);

								// Calculate zoom threshold
								const auto& mapSize = m_mapPrototype->GetSize();
								const auto& windowSize = CoreWindow::GetWindowSize();
								if (mapSize.x < windowSize.x && mapSize.y < windowSize.y)
								{
									m_zoomThreshold = m_zoom.Min();
									CoreWindow::SetViewCenter(m_mapPrototype->GetCenter());
								}
								else
								{
									const float horizontalRatio = mapSize.x / windowSize.x;
									const float verticalRatio = mapSize.y / windowSize.y;
									const float highestRatio = horizontalRatio > verticalRatio ? horizontalRatio : verticalRatio;
									m_zoomThreshold = highestRatio < m_zoom.Max() ? highestRatio : m_zoom.Max();
								}

								// Create simulated world
								delete m_simulatedWorld;
								m_simulatedWorld = new SimulatedWorld;
								m_simulatedWorld->AddMap(m_mapPrototype);
								if (m_deathOnEdgeContact)
									m_simulatedWorld->EnableDeathOnEdgeContact();
								DrawableCheckpoint::SetVisibility(false);

								// Initialize fitness system
								delete m_fitnessSystem;
								m_fitnessSystem = new FitnessSystem(m_population, m_mapPrototype->GetNumberOfCheckpoints(), m_requiredFitnessImprovement);
								m_textObservers[HIGHEST_FITNESS_TEXT]->Notify();
								m_textObservers[HIGHEST_FITNESS_OVERALL_TEXT]->Notify();
								m_textObservers[BEST_TIME_TEXT]->Notify();
								m_textObservers[BEST_TIME_OVERALL_TEXT]->Notify();
								m_textObservers[CURRENT_POPULATION_TEXT]->Notify();
								m_simulatedWorld->AddBeginContactFunction(m_fitnessSystem->GetBeginContactFunction());

								// Create vehicles
								m_simulatedVehicles.resize(m_population, nullptr);
								for (auto& vehicle : m_simulatedVehicles)
									vehicle = m_simulatedWorld->AddVehicle(m_vehiclePrototype);

								// Reset file format
								m_fileFormat = MAP_FILE_FORMAT;
								m_textObservers[FILE_FORMAT_STOPPED_TEXT]->Notify();

								// Reset file format paused
								m_fileFormatPaused = ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED;
								m_textObservers[FILE_FORMAT_PAUSED_TEXT]->Notify();

								// Reset parameter type
								m_parameterType = POPULATION_SIZE;
								m_textObservers[PARAMETER_TYPE_TEXT]->Notify();

								// Create genetic algorithm
								delete m_geneticAlgorithm;
								m_geneticAlgorithm = new GeneticAlgorithmNeuron(
									m_generation,
									m_artificialNeuralNetworkPrototype->GetNumberOfWeights(),
									m_population,
									m_crossoverType,
									m_repeatCrossoverPerIndividual,
									m_mutationProbability,
									m_decreaseMutationProbabilityOverGenerations,
									m_numberOfParents,
									1000,
									std::pair(-ArtificialNeuralNetworkBuilder::GetMaxNeuronValue(), ArtificialNeuralNetworkBuilder::GetMaxNeuronValue())
								);
								m_textObservers[CURRENT_GENERATION_TEXT]->Notify();

								// Set first individual in genetic algorithm (this one may be already optimized)
								m_artificialNeuralNetworks[0]->GetRawData(m_geneticAlgorithm->GetIndividualGenes(0));

								// Reset artificial neural networks except first one
								for (size_t i = 1; i < m_artificialNeuralNetworks.size(); ++i)
									m_artificialNeuralNetworks[i]->SetFromRawData(m_geneticAlgorithm->GetIndividualGenes(i));

								// Reset required fitness improvement rise timer
								m_requiredFitnessImprovementRiseTimer.Reset();
								m_textObservers[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
								m_mode = RUNNING_MODE;
								m_textObservers[MODE_TEXT]->Notify();

								// Prepare statistics builder
								m_statisticsBuilder.ExtractStatic(m_geneticAlgorithm,
									m_fitnessSystem,
									m_deathOnEdgeContact,
									m_requiredFitnessImprovementRise);
								break;
							}
							case CHANGE_FILENAME_TYPE:
								if (m_pressedKeys[iterator->second])
									break;
								++m_fileFormat;
								if (m_fileFormat >= FILE_FORMATS_COUNT)
									m_fileFormat = MAP_FILE_FORMAT;
								m_textObservers[FILE_FORMAT_STOPPED_TEXT]->Notify();
								break;
							case CHANGE_SIMULATION_PARAMETER:
								if (m_pressedKeys[iterator->second])
									break;
								++m_parameterType;
								if (m_parameterType >= PARAMETERS_COUNT)
									m_parameterType = POPULATION_SIZE;
								m_textObservers[PARAMETER_TYPE_TEXT]->Notify();
								break;
							case INCREASE_PARAMETER:
							{
								if (!m_pressedKeyTimer.Update())
									break;

								switch (m_parameterType)
								{
									case POPULATION_SIZE:
										m_population.Increase();
										m_textObservers[POPULATION_SIZE_TEXT]->Notify();
										break;
									case NUMBER_OF_GENERATIONS:
										m_generation.Increase();
										m_textObservers[NUMBER_OF_GENERATIONS_TEXT]->Notify();
										break;
									case DEATH_ON_EDGE_CONTACT:
										m_deathOnEdgeContact.Increase();
										m_textObservers[DEATH_ON_EDGE_CONTACT_TEXT]->Notify();
										break;
									case CROSSOVER_TYPE:
										m_crossoverType.Increase();
										m_textObservers[CROSSOVER_TYPE_TEXT]->Notify();
										break;
									case REPEAT_CROSSOVER_PER_INDIVIDUAL:
										m_repeatCrossoverPerIndividual.Increase();
										m_textObservers[REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT]->Notify();
										break;
									case MUTATION_PROBABILITY:
										m_mutationProbability.Increase();
										m_textObservers[MUTATION_PROBABILITY_TEXT]->Notify();
										break;
									case DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS:
										m_decreaseMutationProbabilityOverGenerations.Increase();
										m_textObservers[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT]->Notify();
										break;
									case NUMBER_OF_PARENTS:
										m_numberOfParents.Increase();
										m_textObservers[NUMBER_OF_PARENTS_TEXT]->Notify();
										break;
									case REQUIRED_FITNESS_IMPROVEMENT:
										m_requiredFitnessImprovement.Increase();
										m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
										break;
									case REQUIRED_FITNESS_IMPROVEMENT_RISE:
										m_requiredFitnessImprovementRise.Increase();
										m_requiredFitnessImprovementRiseTimer.SetTimeout(m_requiredFitnessImprovementRise);
										m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT]->Notify();
										break;
								}

								break;
							}
							case DECREASE_PARAMETER:
							{
								if (!m_pressedKeyTimer.Update())
									break;

								switch (m_parameterType)
								{
									case POPULATION_SIZE:
										m_population.Decrease();
										m_textObservers[POPULATION_SIZE_TEXT]->Notify();
										break;
									case NUMBER_OF_GENERATIONS:
										m_generation.Decrease();
										m_textObservers[NUMBER_OF_GENERATIONS_TEXT]->Notify();
										break;
									case DEATH_ON_EDGE_CONTACT:
										m_deathOnEdgeContact.Decrease();
										m_textObservers[DEATH_ON_EDGE_CONTACT_TEXT]->Notify();
										break;
									case CROSSOVER_TYPE:
										m_crossoverType.Decrease();
										m_textObservers[CROSSOVER_TYPE_TEXT]->Notify();
										break;
									case REPEAT_CROSSOVER_PER_INDIVIDUAL:
										m_repeatCrossoverPerIndividual.Decrease();
										m_textObservers[REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT]->Notify();
										break;
									case MUTATION_PROBABILITY:
										m_mutationProbability.Decrease();
										m_textObservers[MUTATION_PROBABILITY_TEXT]->Notify();
										break;
									case DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS:
										m_decreaseMutationProbabilityOverGenerations.Decrease();
										m_textObservers[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT]->Notify();
										break;
									case NUMBER_OF_PARENTS:
										m_numberOfParents.Decrease();
										m_textObservers[NUMBER_OF_PARENTS_TEXT]->Notify();
										break;
									case REQUIRED_FITNESS_IMPROVEMENT:
										m_requiredFitnessImprovement.Decrease();
										m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
										break;
									case REQUIRED_FITNESS_IMPROVEMENT_RISE:
										m_requiredFitnessImprovementRise.Decrease();
										m_requiredFitnessImprovementRiseTimer.SetTimeout(m_requiredFitnessImprovementRise);
										m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT]->Notify();
										break;
								}

								break;
							}
							case INCREASE_ZOOM:
							case DECREASE_ZOOM:
								break;
						}

						m_pressedKeys[iterator->second] = true;
					}
				}
			}
			
			if (m_mode == STOPPED_MODE)
				filenameText->Capture();
			break;
		}
		case PAUSED_MODE:
		{
			auto* filenameText = static_cast<FilenameText<false, true>*>(m_texts[FILENAME_PAUSED_TEXT]);
			if (!filenameText->IsRenaming())
			{
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					const auto eventKey = CoreWindow::GetEvent().key.code;
					const auto iterator = m_controlKeys.find(eventKey);
					if (iterator != m_controlKeys.end())
					{
						switch (iterator->second)
						{
							case CHANGE_MODE:
							{
								if (m_pressedKeys[iterator->second])
									break;
								m_mode = STOPPED_MODE;
								m_textObservers[MODE_TEXT]->Notify();

								// Reset view so that the center is vehicle starting position
								m_zoom.ResetValue();
								m_textObservers[ZOOM_TEXT]->Notify();
								CoreWindow::Reset();
								CoreWindow::SetViewCenter(m_vehiclePrototype->GetCenter());
								break;
							}
							case CHANGE_FILENAME_TYPE:
							{
								if (m_pressedKeys[iterator->second])
									break;

								++m_fileFormatPaused;
								if (m_fileFormatPaused >= FILE_FORMATS_PAUSED_COUNT)
									m_fileFormatPaused = ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED;
								m_textObservers[FILE_FORMAT_PAUSED_TEXT]->Notify();
								break;
							}
							case PAUSED_CHANGE_MODE:
							{
								if (!m_geneticAlgorithm)
									break; // If there are more generations left
								if (m_pressedKeys[iterator->second])
									break;
								m_mode = RUNNING_MODE;
								m_textObservers[MODE_TEXT]->Notify();
								m_fileFormatPaused = ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED;
								m_textObservers[FILE_FORMAT_PAUSED_TEXT]->Notify();
								break;
							}
							case INCREASE_PARAMETER:
							case DECREASE_PARAMETER:
							case INCREASE_ZOOM:
							case DECREASE_ZOOM:
							default:
								break;
						}

						m_pressedKeys[iterator->second] = true;
					}
				}
			}

			if (m_mode == PAUSED_MODE)
				filenameText->Capture();
			break;
		}
		case RUNNING_MODE:
		{
			if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
			{
				const auto eventKey = CoreWindow::GetEvent().key.code;
				const auto iterator = m_controlKeys.find(eventKey);
				if (iterator == m_controlKeys.end())
					break;

				switch (iterator->second)
				{
					case CHANGE_MODE:
					{
						if (m_pressedKeys[iterator->second])
							break;
						m_mode = STOPPED_MODE;
						m_textObservers[MODE_TEXT]->Notify();

						// Reset view so that the center is vehicle starting position
						m_zoom.ResetValue();
						m_textObservers[ZOOM_TEXT]->Notify();
						CoreWindow::Reset();
						CoreWindow::SetViewCenter(m_vehiclePrototype->GetCenter());
						break;
					}
					case CHANGE_FILENAME_TYPE:
						break;
					case PAUSED_CHANGE_MODE:
					{
						if (m_pressedKeys[iterator->second])
							break;
						m_mode = PAUSED_MODE;
						m_textObservers[MODE_TEXT]->Notify();
						break;
					}
					case INCREASE_PARAMETER:
					case DECREASE_PARAMETER:
						break;
					case INCREASE_ZOOM:
						if (m_pressedKeyTimer.Update())
						{
							m_zoom.Increase();
							CoreWindow::SetViewZoom(m_zoom);
							m_textObservers[ZOOM_TEXT]->Notify();

							if (m_zoom >= m_zoomThreshold)
							{
								// Set view center as map center
								CoreWindow::SetViewCenter(m_mapPrototype->GetCenter());
							}
						}
						break;
					case DECREASE_ZOOM:
						if (m_pressedKeyTimer.Update())
						{
							m_zoom.Decrease();
							CoreWindow::SetViewZoom(m_zoom);
							m_textObservers[ZOOM_TEXT]->Notify();
						}
						break;
					default:
						break;
				}

				m_pressedKeys[iterator->second] = true;
			}
			break;
		}
		default:
			break;
	}

	if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
	{
		const auto eventKey = CoreWindow::GetEvent().key.code;
		const auto iterator = m_controlKeys.find(eventKey);
		if (iterator != m_controlKeys.end())
		{
			m_pressedKeys[iterator->second] = false;
			switch (iterator->second)
			{
				case CHANGE_MODE:
				case CHANGE_FILENAME_TYPE:
				case CHANGE_SIMULATION_PARAMETER:
					break;
				case INCREASE_PARAMETER:
				case DECREASE_PARAMETER:
					m_pressedKeyTimer.MakeTimeout();
					break;
				default:
					break;
			}
		}
	}
}

void StateSimulation::Update()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			auto* filenameText = static_cast<FilenameText<true, false>*>(m_texts[FILENAME_STOPPED_TEXT]);
			if (filenameText->IsReading())
			{
				switch (m_fileFormat)
				{
					case MAP_FILE_FORMAT:
					{
						bool success = m_mapBuilder.Load(filenameText->GetFilename());
						auto status = m_mapBuilder.GetLastOperationStatus();

						// Set filename text
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							delete m_mapPrototype;
							m_mapPrototype = nullptr;
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Prepare map prototype
						delete m_mapPrototype;
						m_mapPrototype = m_mapBuilder.Get();
						m_mapPrototype->CalculateProperties();

						if (!m_vehiclePrototype)
						{
							// Vehicle prototype hasn't been created yet, in this case create dummy
							m_vehicleBuilder.CreateDummy();
							m_vehiclePrototype = m_vehicleBuilder.Get();
						}

						// Update vehicle starting position
						m_vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
						m_vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
						m_vehiclePrototype->Update();

						// Reset view so that the center is vehicle starting position
						CoreWindow::SetViewCenter(m_vehiclePrototype->GetCenter());
						break;
					}
					case ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT:
					{
						bool success = m_artificialNeuralNetworkBuilder.Load(filenameText->GetFilename());
						auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();

						// Set filename text
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						delete m_artificialNeuralNetworkPrototype;
						m_artificialNeuralNetworkPrototype = m_artificialNeuralNetworkBuilder.Get();
						m_artificialNeuralNetworkPrototype->SetFromRawData(m_artificialNeuralNetworkBuilder.GetRawNeuronData());
						break;
					}
					case VEHICLE_FILE_FORMAT:
					{
						bool success = m_vehicleBuilder.Load(filenameText->GetFilename());
						auto status = m_vehicleBuilder.GetLastOperationStatus();

						// Set filename text
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							delete m_vehiclePrototype;
							m_vehiclePrototype = nullptr;
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						if (!m_mapPrototype)
						{
							// Map hasn't been created yet, create dummy
							m_mapBuilder.CreateDummy();
							m_mapPrototype = m_mapBuilder.Get();
							m_mapPrototype->CalculateProperties();
						}

						// Remove old vehicle prototype and prepare new one
						delete m_vehiclePrototype;
						m_vehiclePrototype = m_vehicleBuilder.Get();

						// Update vehicle starting position
						m_vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
						m_vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
						m_vehiclePrototype->Update();

						// Reset view so that the center is vehicle starting position
						CoreWindow::SetViewCenter(m_vehiclePrototype->GetCenter());
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		case RUNNING_MODE:
		{
			bool activity = false;
			for (size_t i = 0; i < m_population; ++i)
			{
				m_simulatedVehicles[i]->Update(m_simulatedWorld->GetEdgesWorld());
				if (!m_simulatedVehicles[i]->IsActive())
					continue;
				activity = true;
				const NeuronLayer& input = m_simulatedVehicles[i]->ProcessOutput();
				const NeuronLayer& output = m_artificialNeuralNetworks[i]->Update(input);
				m_simulatedVehicles[i]->ProcessInput(output);
			}

			m_simulatedWorld->Update();

			sf::Vector2f m_viewCenter;
			if (!activity)
			{
				// Set highest fitness overall
				m_fitnessSystem->Iterate(m_simulatedVehicles);
				m_textObservers[HIGHEST_FITNESS_OVERALL_TEXT]->Notify();
				m_textObservers[BEST_TIME_OVERALL_TEXT]->Notify();

				// Generate new generation
				if (m_geneticAlgorithm->Iterate(m_fitnessSystem->GetFitnessVector()))
				{
					m_textObservers[CURRENT_GENERATION_TEXT]->Notify();
					m_textObservers[CURRENT_POPULATION_TEXT]->Notify();
					m_textObservers[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->Notify();

					// Set artificial neural networks new raw data
					for (size_t i = 0; i < m_artificialNeuralNetworks.size(); ++i)
						m_artificialNeuralNetworks[i]->SetFromRawData(m_geneticAlgorithm->GetIndividualGenes(i));

					// Reset simulated world
					delete m_simulatedWorld;
					m_simulatedWorld = new SimulatedWorld;
					m_simulatedWorld->AddMap(m_mapPrototype);
					if (m_deathOnEdgeContact)
						m_simulatedWorld->EnableDeathOnEdgeContact();
					DrawableCheckpoint::SetVisibility(false);
					m_fitnessSystem->Reset();
					m_simulatedWorld->AddBeginContactFunction(m_fitnessSystem->GetBeginContactFunction());

					// Reset vehicles
					for (auto& vehicle : m_simulatedVehicles)
						vehicle = m_simulatedWorld->AddVehicle(m_vehiclePrototype);

					// Reset required fitness improvement rise timer
					m_requiredFitnessImprovementRiseTimer.Reset();
					m_textObservers[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
				}
				else
				{
					m_mode = PAUSED_MODE;
					m_textObservers[MODE_TEXT]->Notify();
					delete m_geneticAlgorithm;
					m_geneticAlgorithm = nullptr;
				}
			}
			else
			{
				if (m_viewTimer.Update())
				{
					const auto index = m_fitnessSystem->MarkLeader(m_simulatedVehicles);
					m_viewCenter = m_simulatedVehicles[index]->GetCenter();
					m_textObservers[HIGHEST_FITNESS_TEXT]->Notify();
					m_textObservers[BEST_TIME_TEXT]->Notify();
				}

				if (m_requiredFitnessImprovementRiseTimer.Update())
				{
					m_fitnessSystem->Punish(m_simulatedVehicles);
					m_textObservers[CURRENT_POPULATION_TEXT]->Notify();
					m_textObservers[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->Notify();
					m_textObservers[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
				}
				else if (size_t(m_requiredFitnessImprovementRiseTimer.GetValue() * 100) % 25 == 0)
					m_textObservers[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify(); // Notify every 0.25 seconds

				m_fitnessSystem->UpdateTimers(m_simulatedVehicles);
			}

			// Update view
			if (m_zoom < m_zoomThreshold)
			{
				auto viewCenter = CoreWindow::GetViewCenter();
				const auto distance = MathContext::Distance(viewCenter, m_viewCenter);
				const auto angle = MathContext::DifferenceVectorAngle(viewCenter, m_viewCenter);
				viewCenter = MathContext::GetEndPoint(viewCenter, angle, float(-distance * m_viewMovementOffset * CoreWindow::GetElapsedTime()));
				CoreWindow::SetViewCenter(viewCenter);
			}
			break;
		}
		case PAUSED_MODE:
		{
			auto* filenameText = static_cast<FilenameText<false, true>*>(m_texts[FILENAME_PAUSED_TEXT]);
			if (filenameText->IsWriting())
			{
				switch (m_fileFormatPaused)
				{
					case ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED:
					{
						// Clear builder
						m_artificialNeuralNetworkBuilder.Clear();
					
						// Take the best artificial neural network
						m_artificialNeuralNetworkBuilder.Set(m_artificialNeuralNetworks[0]);

						const bool success = m_artificialNeuralNetworkBuilder.Save(filenameText->GetFilename());
						const auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();

						// Set filename text
						if (!success)
							filenameText->SetErrorStatusText(status.second);
						else
							filenameText->SetSuccessStatusText(status.second);
						break;
					}
					case STATISTICS_FILE_FORMAT_PAUSED:
					{
						m_statisticsBuilder.Extract(m_geneticAlgorithm ? m_geneticAlgorithm->GetCurrentGeneration() : m_generation,
													m_fitnessSystem);
						const bool success = m_statisticsBuilder.Save(filenameText->GetFilename());
						const auto status = m_statisticsBuilder.GetLastOperationStatus();

						// Set filename text
						if (!success)
							filenameText->SetErrorStatusText(status.second);
						else
							filenameText->SetSuccessStatusText(status.second);
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		default:
			break;
	}

	for (const auto& text : m_texts)
		text->Update();
}

bool StateSimulation::Load()
{
	// Create texts
	m_texts[MODE_TEXT] = new StatusText({ "Mode:", "", "| [M] [P]" });
	m_texts[FILE_FORMAT_STOPPED_TEXT] = new TripleText({ "File format:", "", "| [F]"});
	m_texts[FILE_FORMAT_PAUSED_TEXT] = new TripleText({ "File format:", "", "| [F]" });
	m_texts[FILENAME_STOPPED_TEXT] = new FilenameText<true, false>("map.bin");
	m_texts[FILENAME_PAUSED_TEXT] = new FilenameText<false, true>("ann.bin");
	m_texts[PARAMETER_TYPE_TEXT] = new TripleText({ "Parameter type:", "", "| [P] [+] [-]" });
	m_texts[POPULATION_SIZE_TEXT] = new DoubleText({ m_parameterTypesStrings[POPULATION_SIZE] + ":" });
	m_texts[NUMBER_OF_GENERATIONS_TEXT] = new DoubleText({ m_parameterTypesStrings[NUMBER_OF_GENERATIONS] + ":" });
	m_texts[DEATH_ON_EDGE_CONTACT_TEXT] = new DoubleText({ m_parameterTypesStrings[DEATH_ON_EDGE_CONTACT] + ":" });
	m_texts[CROSSOVER_TYPE_TEXT] = new DoubleText({ m_parameterTypesStrings[CROSSOVER_TYPE] + ":" });
	m_texts[REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT] = new DoubleText({ m_parameterTypesStrings[REPEAT_CROSSOVER_PER_INDIVIDUAL] + ":" });
	m_texts[MUTATION_PROBABILITY_TEXT] = new DoubleText({ m_parameterTypesStrings[MUTATION_PROBABILITY] + ":" });
	m_texts[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT] = new DoubleText({ m_parameterTypesStrings[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS] + ":" });
	m_texts[NUMBER_OF_PARENTS_TEXT] = new DoubleText({ m_parameterTypesStrings[NUMBER_OF_PARENTS] + ":" });
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT] = new DoubleText({ m_parameterTypesStrings[REQUIRED_FITNESS_IMPROVEMENT_RISE] + ":" });
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new DoubleText({ m_parameterTypesStrings[REQUIRED_FITNESS_IMPROVEMENT] + ":" });
	m_texts[CURRENT_POPULATION_TEXT] = new DoubleText({ "Current population size:" });
	m_texts[CURRENT_GENERATION_TEXT] = new DoubleText({ "Current generation:" });
	m_texts[HIGHEST_FITNESS_TEXT] = new DoubleText({ "Highest fitness:" });
	m_texts[HIGHEST_FITNESS_OVERALL_TEXT] = new DoubleText({ "Highest fitness overall:" });
	m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new DoubleText({ "Raising required fitness improvement in:" });
	m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT] = new DoubleText({ "Mean required fitness improvement:" });
	m_texts[ZOOM_TEXT] = new TripleText({ "Zoom:", "", "| [/] [*]" });
	m_texts[BEST_TIME_TEXT] = new DoubleText({ "Best time:" });
	m_texts[BEST_TIME_OVERALL_TEXT] = new DoubleText({ "Best time overall:" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[FILE_FORMAT_STOPPED_TEXT] = new FunctionEventObserver<std::string>([&] { return m_fileFormatStrings[m_fileFormat]; });
	m_textObservers[FILE_FORMAT_PAUSED_TEXT] = new FunctionEventObserver<std::string>([&] { return m_fileFormatPausedStrings[m_fileFormatPaused];});
	m_textObservers[PARAMETER_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_parameterTypesStrings[m_parameterType]; });
	m_textObservers[POPULATION_SIZE_TEXT] = new FunctionEventObserver<size_t>([&] { return m_population; });
	m_textObservers[NUMBER_OF_GENERATIONS_TEXT] = new FunctionEventObserver<size_t>([&] { return m_generation; });
	m_textObservers[DEATH_ON_EDGE_CONTACT_TEXT] = new FunctionEventObserver<bool>([&] { return m_deathOnEdgeContact; });
	m_textObservers[CROSSOVER_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return CrossoverTypeStrings[m_crossoverType]; });
	m_textObservers[REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT] = new FunctionEventObserver<bool>([&] { return m_repeatCrossoverPerIndividual; });
	m_textObservers[MUTATION_PROBABILITY_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(size_t(m_mutationProbability * 100.0)); }, "", "%");
	m_textObservers[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT] = new FunctionEventObserver<bool>([&] { return m_decreaseMutationProbabilityOverGenerations; });
	m_textObservers[NUMBER_OF_PARENTS_TEXT] = new FunctionEventObserver<size_t>([&] { return m_numberOfParents; });
	m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(m_requiredFitnessImprovementRiseTimer.GetTimeout()) + " seconds"; });
	m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(size_t(m_requiredFitnessImprovement * 100.0)); }, "", "%");
	m_textObservers[CURRENT_POPULATION_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(m_fitnessSystem ? (m_population - m_fitnessSystem->GetNumberOfPunishedVehicles()) : m_population) + "/" + std::to_string(m_population); });
	m_textObservers[CURRENT_GENERATION_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(m_geneticAlgorithm ? m_geneticAlgorithm->GetCurrentGeneration() : m_generation) + "/" + std::to_string(m_generation); });
	m_textObservers[HIGHEST_FITNESS_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(!m_fitnessSystem ? 0 : size_t(m_fitnessSystem->GetHighestFitnessRatio())); }, "", "%");
	m_textObservers[HIGHEST_FITNESS_OVERALL_TEXT] = new FunctionEventObserver<std::string>([&] { return !m_fitnessSystem || (m_geneticAlgorithm && m_geneticAlgorithm->GetCurrentGeneration() == 0) ? "Unknown" : std::to_string(size_t(m_fitnessSystem->GetHighestFitnessOverallRatio())) + "%"; });
	m_textObservers[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(m_requiredFitnessImprovementRiseTimer.GetTimeout() - m_requiredFitnessImprovementRiseTimer.GetValue()); }, "", " seconds");
	m_textObservers[MEAN_REQUIRED_FITNESS_IMPROVEMENT] = new FunctionEventObserver<std::string>([&] { return std::to_string(size_t((m_fitnessSystem ? m_fitnessSystem->GetMeanRequiredFitnessImprovementRatio() : 0))); }, "", "%");
	m_textObservers[ZOOM_TEXT] = new FunctionEventObserver<float>([&] { return m_zoom; });
	m_textObservers[BEST_TIME_TEXT] = new FunctionEventObserver<double>([&] { return !m_fitnessSystem ? 0.0 : m_fitnessSystem->GetBestTime(); }, "", " seconds");
	m_textObservers[BEST_TIME_OVERALL_TEXT] = new FunctionEventObserver<std::string>([&] { return !m_fitnessSystem || (m_geneticAlgorithm && m_geneticAlgorithm->GetCurrentGeneration() == 0) ? "Unknown" : std::to_string(m_fitnessSystem->GetBestTimeOverall()) + " seconds"; });

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		((DoubleText*)m_texts[i])->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {8}, {14} });
	m_texts[FILE_FORMAT_STOPPED_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {8} });
	m_texts[FILE_FORMAT_PAUSED_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {8} });
	m_texts[FILENAME_STOPPED_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {8}, {14} });
	m_texts[FILENAME_PAUSED_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {8}, {14} });
	m_texts[PARAMETER_TYPE_TEXT]->SetPosition({ FontContext::Component(11, true), {0}, {10}, {20} });
	m_texts[POPULATION_SIZE_TEXT]->SetPosition({ FontContext::Component(10, true), {0}, {10} });
	m_texts[NUMBER_OF_GENERATIONS_TEXT]->SetPosition({ FontContext::Component(9, true), {0}, {10} });
	m_texts[DEATH_ON_EDGE_CONTACT_TEXT]->SetPosition({ FontContext::Component(8, true), {0}, {10} });
	m_texts[CROSSOVER_TYPE_TEXT]->SetPosition({ FontContext::Component(7, true), {0}, {10} });
	m_texts[REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT]->SetPosition({ FontContext::Component(6, true), {0}, {10} });
	m_texts[MUTATION_PROBABILITY_TEXT]->SetPosition({ FontContext::Component(5, true), {0}, {10} });
	m_texts[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {10} });
	m_texts[NUMBER_OF_PARENTS_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {10} });
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {10} });
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {10} });
	m_texts[CURRENT_POPULATION_TEXT]->SetPosition({ FontContext::Component(6, true), {14}, {23} });
	m_texts[CURRENT_GENERATION_TEXT]->SetPosition({ FontContext::Component(5, true), {14}, {23} });
	m_texts[HIGHEST_FITNESS_TEXT]->SetPosition({ FontContext::Component(4, true), {14}, {23} });
	m_texts[HIGHEST_FITNESS_OVERALL_TEXT]->SetPosition({ FontContext::Component(3, true), {14}, {23} });
	m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->SetPosition({ FontContext::Component(2, true), {14}, {23} });
	m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->SetPosition({ FontContext::Component(1, true), {14}, {23} });
	m_texts[ZOOM_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {8} });
	m_texts[BEST_TIME_TEXT]->SetPosition({ FontContext::Component(2, true), {27}, {31} });
	m_texts[BEST_TIME_OVERALL_TEXT]->SetPosition({ FontContext::Component(1, true), {27}, {31} });

	CoreLogger::PrintSuccess("StateSimulation dependencies loaded correctly");
	return true;
}

void StateSimulation::Draw()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
			if (m_vehiclePrototype)
			{
				m_vehiclePrototype->DrawBody();
				if (m_artificialNeuralNetworkPrototype)
					m_vehiclePrototype->DrawBeams();
				m_vehiclePrototype->DrawSensors();
			}

			if (m_mapPrototype)
			{
				// We do not draw checkpoints in simulation state
				m_mapPrototype->DrawEdges();
			}

			m_texts[FILE_FORMAT_STOPPED_TEXT]->Draw();
			m_texts[FILENAME_STOPPED_TEXT]->Draw();
			m_texts[PARAMETER_TYPE_TEXT]->Draw();
			m_texts[POPULATION_SIZE_TEXT]->Draw();
			m_texts[NUMBER_OF_GENERATIONS_TEXT]->Draw();
			m_texts[DEATH_ON_EDGE_CONTACT_TEXT]->Draw();
			break;
		case PAUSED_MODE:
			m_simulatedWorld->Draw();
			m_texts[FILE_FORMAT_PAUSED_TEXT]->Draw();
			m_texts[FILENAME_PAUSED_TEXT]->Draw();
			m_texts[CURRENT_POPULATION_TEXT]->Draw();
			m_texts[CURRENT_GENERATION_TEXT]->Draw();
			m_texts[HIGHEST_FITNESS_TEXT]->Draw();
			m_texts[HIGHEST_FITNESS_OVERALL_TEXT]->Draw();
			m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Draw();
			m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->Draw();
			m_texts[BEST_TIME_TEXT]->Draw();
			m_texts[BEST_TIME_OVERALL_TEXT]->Draw();
			break;
		case RUNNING_MODE:
			m_simulatedWorld->Draw();
			m_texts[CURRENT_POPULATION_TEXT]->Draw();
			m_texts[CURRENT_GENERATION_TEXT]->Draw();
			m_texts[HIGHEST_FITNESS_TEXT]->Draw();
			m_texts[HIGHEST_FITNESS_OVERALL_TEXT]->Draw();
			m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Draw();
			m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->Draw();
			m_texts[ZOOM_TEXT]->Draw();
			m_texts[BEST_TIME_TEXT]->Draw();
			m_texts[BEST_TIME_OVERALL_TEXT]->Draw();
			break;
		default:
			break;
	}

	m_texts[MODE_TEXT]->Draw();
	m_texts[CROSSOVER_TYPE_TEXT]->Draw();
	m_texts[REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT]->Draw();
	m_texts[MUTATION_PROBABILITY_TEXT]->Draw();
	m_texts[DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT]->Draw();
	m_texts[NUMBER_OF_PARENTS_TEXT]->Draw();
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT]->Draw();
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Draw();
}
