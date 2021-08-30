#pragma once
#include "StateTraining.hpp"
#include "DrawableMap.hpp"
#include "GeneticAlgorithm.hpp"
#include "FunctionEventObserver.hpp"
#include "TypeEventObserver.hpp"
#include "FunctionTimerObserver.hpp"
#include "DrawableFilenameText.hpp"
#include "CoreLogger.hpp"

StateTraining::StateTraining() :
	m_pressedKeyTimer(0.0, 1.0, 5000),
	m_waveTimer(0.0, 4.0),
	m_viewTimer(1.0, 0.1),
	m_viewMovementOffset(500.0),
	m_minPopulationSize(4),
	m_maxPopulationSize(60),
	m_populationSizeResetValue(30),
	m_minNumberOfGenerations(5),
	m_maxNumberOfGenerations(300),
	m_numberOfGenerationsResetValue(60),
	m_minCrossoverProbability(0.01),
	m_maxCrossoverProbability(1.00),
	m_crossoverProbabilityResetValue(0.5),
	m_crossoverProbabilityOffset(0.01),
	m_minMutationProbability(0.01),
	m_maxMutationProbability(0.99),
	m_mutationProbabilityResetValue(0.05),
	m_mutationProbabilityOffset(0.01),
	m_minRequiredFitnessImprovement(0.01),
	m_maxRequiredFitnessImprovement(0.2),
	m_requiredFitnessImprovementResetValue(0.05),
	m_requiredFitnessImprovementOffset(0.01)
{
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_mode = STOPPED_MODE;

	m_filenameTypeStrings[MAP_FILENAME_TYPE] = "Map";
	m_filenameTypeStrings[ANN_FILENAME_TYPE] = "ANN";
	m_filenameTypeStrings[VEHICLE_FILENAME_TYPE] = "Vehicle";
	m_filenameType = MAP_FILENAME_TYPE;

	m_parameterTypesStrings[POPULATION_SIZE] = "Population size";
	m_parameterTypesStrings[NUMBER_OF_GENERATIONS] = "Number of generations";
	m_parameterTypesStrings[CROSSOVER_PROBABILITY] = "Crossover probability";
	m_parameterTypesStrings[MUTATION_PROBABILITY] = "Mutation probability";
	m_parameterTypesStrings[DECREASE_MUTATION_OVER_GENERATIONS] = "Decrease mutation over generations";
	m_parameterTypesStrings[SINGLE_POINT_CROSSOVER] = "Single-point crossover";
	m_parameterTypesStrings[REQUIRED_FITNESS_IMPROVEMENT] = "Required fitness improvement";
	m_parameterType = POPULATION_SIZE;

	m_controlKeys[sf::Keyboard::M] = CHANGE_MODE;
	m_controlKeys[sf::Keyboard::F] = CHANGE_FILENAME_TYPE;
	m_controlKeys[sf::Keyboard::P] = CHANGE_SIMULATION_PARAMETER;
	m_controlKeys[sf::Keyboard::Add] = INCREASE_PARAMETER;
	m_controlKeys[sf::Keyboard::Subtract] = DECREASE_PARAMETER;

	for (auto & pressedKey : m_pressedKeys)
		pressedKey = false;

	m_internalErrorsStrings[NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED] = "Error: No artificial neural network is specified!";
	m_internalErrorsStrings[NO_DRAWABLE_MAP_SPECIFIED] = "Error: No drawable map is specified!";
	m_internalErrorsStrings[NO_DRAWABLE_VEHICLE_SPECIFIED] = "Error: No drawable vehicle is specified!";
	m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH] = "Error: Artificial neural network number of input neurons mismatches number of vehicle sensors!";
	m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH] = "Error: Artificial neural network number of output neurons mismatches number of vehicle (3) inputs!";
	m_internalErrorsStrings[SAVE_IS_ALLOWED_ONLY_FOR_ANN] = "Error: Save mode is allowed only for artificial neural network!";

	m_populationSize = m_populationSizeResetValue;
	m_numberOfGenerations = m_numberOfGenerationsResetValue;
	m_population = m_populationSize;
	m_generation = 1;
	m_crossoverProbability = m_crossoverProbabilityResetValue;
	m_mutationProbability = m_mutationProbabilityResetValue;
	m_decreaseMutationOverGenerations = false;
	m_singlePointCrossover = false;
	m_requiredFitnessImprovement = m_requiredFitnessImprovementResetValue;
	m_meanRequiredFitnessImprovement = 0.0;

	m_geneticAlgorithm = nullptr;
	m_artificialNeuralNetworks.resize(m_populationSizeResetValue, nullptr);
	m_drawableMap = nullptr;
	m_drawableVehicleFactory.resize(m_populationSizeResetValue, nullptr);

	m_artificialNeuralNetworkBackup = nullptr;
	m_drawableVehicleBackup = nullptr;
	m_drawableMapBackup = nullptr;

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateTraining::~StateTraining()
{
	delete m_geneticAlgorithm;
	for (auto& ann : m_artificialNeuralNetworks)
		delete ann;
	delete m_drawableMap;
	for (auto& vehicle : m_drawableVehicleFactory)
		delete vehicle;
	delete m_artificialNeuralNetworkBackup;
	delete m_drawableVehicleBackup;
	delete m_drawableMapBackup;
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateTraining::Reload()
{
	// Reset states
	m_mode = STOPPED_MODE;
	m_filenameType = MAP_FILENAME_TYPE;
	m_parameterType = POPULATION_SIZE;

	// Reset pressed keys
	for (size_t i = 0; i < m_pressedKeys.size(); ++i)
		m_pressedKeys[i] = false;

	// Reset simulation parameters
	m_populationSize = m_populationSizeResetValue;
	m_numberOfGenerations = m_numberOfGenerationsResetValue;
	m_population = m_populationSize;
	m_generation = 1;
	m_crossoverProbability = m_crossoverProbabilityResetValue;
	m_mutationProbability = m_mutationProbabilityResetValue;
	m_decreaseMutationOverGenerations = false;
	m_singlePointCrossover = false;
	m_requiredFitnessImprovement = m_requiredFitnessImprovementResetValue;
	m_meanRequiredFitnessImprovement = 0.0;

	// Reset objects of environment
	delete m_geneticAlgorithm;
	m_geneticAlgorithm = nullptr;
	for (auto& ann : m_artificialNeuralNetworks)
	{
		delete ann;
		ann = nullptr;
	}
	m_artificialNeuralNetworks.resize(m_populationSizeResetValue, nullptr);
	delete m_drawableMap;
	m_drawableMap = nullptr;
	for (auto& vehicle : m_drawableVehicleFactory)
	{
		delete vehicle;
		vehicle = nullptr;
	}
	m_drawableVehicleFactory.resize(m_populationSizeResetValue, nullptr);

	// Reset backups
	delete m_artificialNeuralNetworkBackup;
	m_artificialNeuralNetworkBackup = nullptr;
	delete m_drawableVehicleBackup;
	m_drawableVehicleBackup = nullptr;
	delete m_drawableMapBackup;
	m_drawableMapBackup = nullptr;

	// Reset builders
	m_artificialNeuralNetworkBuilder.Clear();
	m_drawableMapBuilder.Clear();
	m_drawableVehicleBuilder.Clear();

	// Reset timers
	m_pressedKeyTimer.Reset();
	m_waveTimer.Reset();
	m_viewTimer.Reset();

	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}
}

void StateTraining::Capture()
{
	auto* filenameText = static_cast<DrawableFilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	filenameText->Capture();

	if (!filenameText->IsRenaming())
	{
		switch (m_mode)
		{
			case STOPPED_MODE:
			{
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					auto eventKey = CoreWindow::GetEvent().key.code;
					auto iterator = m_controlKeys.find(eventKey);
					if (iterator != m_controlKeys.end())
					{
						switch (iterator->second)
						{
							case CHANGE_MODE:
								if (m_pressedKeys[iterator->second])
									break;
								m_pressedKeys[iterator->second] = true;
								++m_mode;
								if (m_mode >= MODES_COUNT)
									m_mode = STOPPED_MODE;
								else if (m_mode == RUNNING_MODE)
								{
									DrawableStatusText* activeModeText = static_cast<DrawableStatusText*>(m_texts[ACTIVE_MODE_TEXT]);
									activeModeText->ShowStatusText();
									if (!m_artificialNeuralNetworkBackup)
									{
										activeModeText->SetErrorStatusText(m_internalErrorsStrings[NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED]);
										m_mode = STOPPED_MODE;
										break;
									}

									if (!m_drawableVehicleBackup)
									{
										activeModeText->SetErrorStatusText(m_internalErrorsStrings[NO_DRAWABLE_VEHICLE_SPECIFIED]);
										m_mode = STOPPED_MODE;
										break;
									}

									if (!m_drawableMapBackup)
									{
										activeModeText->SetErrorStatusText(m_internalErrorsStrings[NO_DRAWABLE_MAP_SPECIFIED]);
										m_mode = STOPPED_MODE;
										break;
									}

									if (m_artificialNeuralNetworkBackup->GetNumberOfInputNeurons() != m_drawableVehicleBackup->GetNumberOfOutputs())
									{
										activeModeText->SetErrorStatusText(m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH]);
										m_mode = STOPPED_MODE;
										break;
									}

									if (m_artificialNeuralNetworkBackup->GetNumberOfOutputNeurons() != m_drawableVehicleBackup->GetNumberOfInputs())
									{
										activeModeText->SetErrorStatusText(m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH]);
										m_mode = STOPPED_MODE;
										break;
									}

									// Create artificial neural networks
									for (auto& ann : m_artificialNeuralNetworks)
										delete ann;
									m_artificialNeuralNetworks.resize(m_populationSize);
									for (auto& ann : m_artificialNeuralNetworks)
										ann = ArtificialNeuralNetworkBuilder::Copy(m_artificialNeuralNetworkBackup);

									// Create drawable map
									delete m_drawableMap;
									m_drawableMap = DrawableMapBuilder::Copy(m_drawableMapBackup);
									m_drawableMap->Init(m_populationSize, m_requiredFitnessImprovement);

									// Create drawable vehicles
									for (auto& vehicle : m_drawableVehicleFactory)
										delete vehicle;
									m_drawableVehicleFactory.resize(m_populationSize);
									for (auto& vehicle : m_drawableVehicleFactory)
									{
										vehicle = DrawableVehicleBuilder::Copy(m_drawableVehicleBackup);
									}

									// Reset population size and generation number
									m_population = m_populationSize;
									m_generation = 1;
									m_textObservers[POPULATION_TEXT]->Notify();
									m_textObservers[GENERATION_TEXT]->Notify();

									// Create genetic algorithm
									delete m_geneticAlgorithm;
									m_geneticAlgorithm = new GeneticAlgorithmNeuron(m_numberOfGenerations,
										m_artificialNeuralNetworkBackup->GetNumberOfWeights(),
										m_populationSize,
										m_crossoverProbability,
										m_mutationProbability,
										m_decreaseMutationOverGenerations,
										m_singlePointCrossover,
										1000,
										std::pair(-1.0, 1.0));

									// Reset artificial neural networks
									for (size_t i = 0; i < m_artificialNeuralNetworks.size(); ++i)
										m_artificialNeuralNetworks[i]->SetFromRawData(m_geneticAlgorithm->GetIndividual(i));
								}

								m_textObservers[ACTIVE_MODE_TEXT]->Notify();
								break;
							case CHANGE_FILENAME_TYPE:
								if (m_pressedKeys[iterator->second])
									break;
								m_pressedKeys[iterator->second] = true;
								++m_filenameType;
								if (m_filenameType >= FILENAME_TYPES_COUNT)
									m_filenameType = MAP_FILENAME_TYPE;
								m_textObservers[FILENAME_TYPE_TEXT]->Notify();
								break;
							case CHANGE_SIMULATION_PARAMETER:
								if (m_pressedKeys[iterator->second])
									break;
								m_pressedKeys[iterator->second] = true;
								++m_parameterType;
								if (m_parameterType >= PARAMETERS_COUNT)
									m_parameterType = POPULATION_SIZE;
								m_textObservers[PARAMETER_TYPE_TEXT]->Notify();
								break;
							case INCREASE_PARAMETER:
							{
								if (m_pressedKeyTimer.Update())
								{
									switch (m_parameterType)
									{
										case POPULATION_SIZE:
											++m_populationSize;
											if (m_populationSize > m_maxPopulationSize)
												m_populationSize = m_maxPopulationSize;
											m_textObservers[POPULATION_SIZE_TEXT]->Notify();
											break;
										case NUMBER_OF_GENERATIONS:
											++m_numberOfGenerations;
											if (m_numberOfGenerations > m_maxNumberOfGenerations)
												m_numberOfGenerations = m_maxNumberOfGenerations;
											m_textObservers[NUMBER_OF_GENERATIONS_TEXT]->Notify();
											break;
										case CROSSOVER_PROBABILITY:
											m_crossoverProbability += m_crossoverProbabilityOffset;
											if (m_crossoverProbability > m_maxCrossoverProbability)
												m_crossoverProbability = m_maxCrossoverProbability;
											m_textObservers[CROSSOVER_PROBABILITY_TEXT]->Notify();
											break;
										case MUTATION_PROBABILITY:
											m_mutationProbability += m_mutationProbabilityOffset;
											if (m_mutationProbability > m_maxMutationProbability)
												m_mutationProbability = m_maxMutationProbability;
											m_textObservers[MUTATION_PROBABILITY_TEXT]->Notify();
											break;
										case DECREASE_MUTATION_OVER_GENERATIONS:
											m_decreaseMutationOverGenerations = true;
											m_textObservers[DECREASE_MUTATION_OVER_GENERATION_TEXT]->Notify();
											break;
										case SINGLE_POINT_CROSSOVER:
											m_singlePointCrossover = true;
											m_textObservers[SINGLE_POINT_CROSSOVER_TEXT]->Notify();
											break;
										case REQUIRED_FITNESS_IMPROVEMENT:
											m_requiredFitnessImprovement += m_requiredFitnessImprovementOffset;
											if (m_requiredFitnessImprovement > m_maxRequiredFitnessImprovement)
												m_requiredFitnessImprovement = m_maxRequiredFitnessImprovement;
											m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
											break;
									}
								}
								
								break;
							}
							case DECREASE_PARAMETER:
							{
								if (m_pressedKeyTimer.Update())
								{
									switch (m_parameterType)
									{
										case POPULATION_SIZE:
											--m_populationSize;
											if (m_populationSize < m_minPopulationSize)
												m_populationSize = m_minPopulationSize;
											m_textObservers[POPULATION_SIZE_TEXT]->Notify();
											break;
										case NUMBER_OF_GENERATIONS:
											--m_numberOfGenerations;
											if (m_numberOfGenerations < m_minNumberOfGenerations)
												m_numberOfGenerations = m_minNumberOfGenerations;
											m_textObservers[NUMBER_OF_GENERATIONS_TEXT]->Notify();
											break;
										case CROSSOVER_PROBABILITY:
											m_crossoverProbability -= m_crossoverProbabilityOffset;
											if (m_crossoverProbability < m_minCrossoverProbability)
												m_crossoverProbability = m_minCrossoverProbability;
											m_textObservers[CROSSOVER_PROBABILITY_TEXT]->Notify();
											break;
										case MUTATION_PROBABILITY:
											m_mutationProbability -= m_mutationProbabilityOffset;
											if (m_mutationProbability < m_minMutationProbability)
												m_mutationProbability = m_minMutationProbability;
											m_textObservers[MUTATION_PROBABILITY_TEXT]->Notify();
											break;
										case DECREASE_MUTATION_OVER_GENERATIONS:
											m_decreaseMutationOverGenerations = false;
											m_textObservers[DECREASE_MUTATION_OVER_GENERATION_TEXT]->Notify();
											break;
										case SINGLE_POINT_CROSSOVER:
											m_singlePointCrossover = false;
											m_textObservers[SINGLE_POINT_CROSSOVER_TEXT]->Notify();
											break;
										case REQUIRED_FITNESS_IMPROVEMENT:
											m_requiredFitnessImprovement -= m_requiredFitnessImprovementOffset;
											if (m_requiredFitnessImprovement < m_minRequiredFitnessImprovement)
												m_requiredFitnessImprovement = m_minRequiredFitnessImprovement;
											m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Notify();
											break;
									}
								}

								break;
							}
						}
					}
				}
				break;
			}
			case RUNNING_MODE:
			{
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					auto eventKey = CoreWindow::GetEvent().key.code;
					auto iterator = m_controlKeys.find(eventKey);
					if (iterator == m_controlKeys.end())
						break;

					switch (iterator->second)
					{
						case CHANGE_MODE:
						{
							if (m_pressedKeys[iterator->second])
								break;
							m_pressedKeys[iterator->second] = true;
							++m_mode;
							if (m_mode >= MODES_COUNT)
								m_mode = STOPPED_MODE;

							// Reset view
							auto& view = CoreWindow::GetView();
							auto viewOffset = CoreWindow::GetViewOffset();
							view.move(-viewOffset);
							CoreWindow::GetRenderWindow().setView(view);
							break;
						}
						case CHANGE_FILENAME_TYPE:
						case CHANGE_SIMULATION_PARAMETER:
						case INCREASE_PARAMETER:
						case DECREASE_PARAMETER:
						default:
							break;
					}
				}
				break;
			}
			default:
				break;
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

void StateTraining::Update()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			auto* filenameText = static_cast<DrawableFilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
			if (filenameText->IsReading())
			{
				switch (m_filenameType)
				{
					case MAP_FILENAME_TYPE:
					{
						bool success = m_drawableMapBuilder.Load(filenameText->GetFilename());
						auto status = m_drawableMapBuilder.GetLastOperationStatus();

						// Set filename text
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Prepare drawable map
						delete m_drawableMapBackup;
						m_drawableMapBackup = m_drawableMapBuilder.Get();
						m_drawableMapBackup->Init(0, 0.0);

						if (!m_drawableVehicleBackup)
						{
							// Drawable vehicle hasn't been created yet, in this case create dummy
							m_drawableVehicleBuilder.CreateDummy();
							m_drawableVehicleBackup = m_drawableVehicleBuilder.Get();
							m_drawableMapBuilder.UpdateVehicle(m_drawableVehicleBackup);
						}

						break;
					}
					case ANN_FILENAME_TYPE:
					{
						bool success = m_artificialNeuralNetworkBuilder.Load(filenameText->GetFilename());
						auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();

						// Set filename text
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						delete m_artificialNeuralNetworkBackup;
						m_artificialNeuralNetworkBackup = m_artificialNeuralNetworkBuilder.Get();
						m_artificialNeuralNetworkBackup->SetFromRawData(m_artificialNeuralNetworkBuilder.GetRawNeuronData());

						break;
					}
					case VEHICLE_FILENAME_TYPE:
					{
						bool success = m_drawableVehicleBuilder.Load(filenameText->GetFilename());
						auto status = m_drawableVehicleBuilder.GetLastOperationStatus();

						// Set filename text
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						if (!m_drawableMapBackup)
						{
							// Drawable map hasn't been created yet, create dummy
							m_drawableMapBuilder.CreateDummy();
							m_drawableMapBackup = m_drawableMapBuilder.Get();
							m_drawableMapBackup->Init(0, 0.0);
						}

						delete m_drawableVehicleBackup;
						m_drawableVehicleBackup = m_drawableVehicleBuilder.Get();
						m_drawableMapBuilder.UpdateVehicle(m_drawableVehicleBackup);
						break;
					}
					default:
						break;
				}
			}
			else if (filenameText->IsWriting())
			{
				switch (m_filenameType)
				{
					case ANN_FILENAME_TYPE:
					{
						//bool success = m_artificialNeuralNetworkBuilder.Load(filenameText->GetFilename());
						//auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();

						//// Set filename text
						//filenameText->ShowStatusText();
						//if (!success)
						//{
						//	filenameText->SetErrorStatusText(status.second);
						//	break;
						//}
						//filenameText->SetSuccessStatusText(status.second);
						break;
					}
					case VEHICLE_FILENAME_TYPE:
					case MAP_FILENAME_TYPE:
						filenameText->ShowStatusText();
						filenameText->SetErrorStatusText(m_internalErrorsStrings[SAVE_IS_ALLOWED_ONLY_FOR_ANN]);
						break;
					default:
						break;
				}
			}
			else if (!filenameText->IsRenaming())
			{

			}
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
				m_drawableMap->Iterate(m_drawableVehicleFactory);
				m_textObservers[HIGHEST_FITNESS_OVERALL_TEXT]->Notify();

				// Generate new generation
				if (m_geneticAlgorithm->Iterate(m_drawableMap->GetFitnessVector()))
				{
					++m_generation;
					m_textObservers[GENERATION_TEXT]->Notify();

					for (size_t i = 0; i < m_artificialNeuralNetworks.size(); ++i)
						m_artificialNeuralNetworks[i]->SetFromRawData(m_geneticAlgorithm->GetIndividual(i));

					for (size_t i = 0; i < m_drawableVehicleFactory.size(); ++i)
					{
						delete m_drawableVehicleFactory[i];
						m_drawableVehicleFactory[i] = m_drawableVehicleBuilder.Get();
						m_drawableMapBuilder.UpdateVehicle(m_drawableVehicleFactory[i]);
					}

					m_drawableMap->Reset();
				}
				else
				{

				}
			}
			else
			{
				m_drawableMap->UpdateTimers();

				if (m_viewTimer.Update())
				{
					auto index = m_drawableMap->MarkLeader(m_drawableVehicleFactory);
					m_textObservers[HIGHEST_FITNESS_TEXT]->Notify();
					m_textObservers[HIGHEST_FITNESS_OVERALL_TEXT]->Notify();
					m_viewCenter = m_drawableVehicleFactory[index]->GetCenter();
				}

				if (m_waveTimer.Update())
				{
					std::tie(m_population, m_meanRequiredFitnessImprovement) = m_drawableMap->Punish(m_drawableVehicleFactory);
					m_textObservers[POPULATION_TEXT]->Notify();
					m_textObservers[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->Notify();
				}
			}

			// Update view
			auto& view = CoreWindow::GetView();
			auto currentViewCenter = view.getCenter();
			auto distance = DrawableMath::Distance(currentViewCenter, m_viewCenter);
			auto angle = DrawableMath::DifferenceVectorAngle(currentViewCenter, m_viewCenter);
			auto newCenter = DrawableMath::GetEndPoint(currentViewCenter, angle, float(-distance / m_viewMovementOffset));
			view.setCenter(newCenter);
			CoreWindow::GetRenderWindow().setView(view);
			break;
		}
		default:
			break;
	}

	for (const auto& text : m_texts)
		text->Update();
}

bool StateTraining::Load()
{
	// Create texts
	m_texts[ACTIVE_MODE_TEXT] = new DrawableStatusText({ "Mode:", "", "| [M]" });
	m_texts[FILENAME_TEXT] = new DrawableFilenameText<true, true>;
	m_texts[FILENAME_TYPE_TEXT] = new DrawableTripleText({ "Filename type:", "", "| [F]"});
	m_texts[PARAMETER_TYPE_TEXT] = new DrawableTripleText({ "Parameter type:", "", "| [P] [+] [-]" });
	m_texts[POPULATION_SIZE_TEXT] = new DrawableDoubleText({ "Population size:" });
	m_texts[NUMBER_OF_GENERATIONS_TEXT] = new DrawableDoubleText({ "Number of generations:" });
	m_texts[CROSSOVER_PROBABILITY_TEXT] = new DrawableDoubleText({ "Crossover probability:" });
	m_texts[MUTATION_PROBABILITY_TEXT] = new DrawableDoubleText({ "Mutation probability:" });
	m_texts[DECREASE_MUTATION_OVER_GENERATION_TEXT] = new DrawableDoubleText({ "Decrease mutation over generations:" });
	m_texts[SINGLE_POINT_CROSSOVER_TEXT] = new DrawableDoubleText({ "Single-point crossover:" });
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new DrawableDoubleText({ "Required fitness improvement:" });
	m_texts[POPULATION_TEXT] = new DrawableDoubleText({ "Current population size:" });
	m_texts[GENERATION_TEXT] = new DrawableDoubleText({ "Current generation:" });
	m_texts[HIGHEST_FITNESS_TEXT] = new DrawableDoubleText({ "Highest fitness:" });
	m_texts[HIGHEST_FITNESS_OVERALL_TEXT] = new DrawableDoubleText({ "Highest fitness overall:" });
	m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new DrawableDoubleText({ "Raising required fitness improvement in:" });
	m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT] = new DrawableDoubleText({ "Mean required fitness improvement:" });

	// Create observers
	m_textObservers[ACTIVE_MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[FILENAME_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_filenameTypeStrings[m_filenameType]; });
	m_textObservers[PARAMETER_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_parameterTypesStrings[m_parameterType]; });
	m_textObservers[POPULATION_SIZE_TEXT] = new TypeEventObserver<size_t>(m_populationSize);
	m_textObservers[NUMBER_OF_GENERATIONS_TEXT] = new TypeEventObserver<size_t>(m_numberOfGenerations);
	m_textObservers[CROSSOVER_PROBABILITY_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(size_t(m_crossoverProbability * 100.0)) + "%"; });
	m_textObservers[MUTATION_PROBABILITY_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(size_t(m_mutationProbability * 100.0)) + "%"; });
	m_textObservers[DECREASE_MUTATION_OVER_GENERATION_TEXT] = new FunctionEventObserver<std::string>([&] { return m_decreaseMutationOverGenerations ? "True" : "False"; });
	m_textObservers[SINGLE_POINT_CROSSOVER_TEXT] = new FunctionEventObserver<std::string>([&] { return m_singlePointCrossover ? "True" : "False"; });
	m_textObservers[REQUIRED_FITNESS_IMPROVEMENT_TEXT]= new FunctionEventObserver<std::string>([&] { return std::to_string(size_t(m_requiredFitnessImprovement * 100.0)) + "%"; });
	m_textObservers[POPULATION_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(m_population) + "/" + std::to_string(m_populationSize); });
	m_textObservers[GENERATION_TEXT] = new FunctionEventObserver<std::string>([&] { return std::to_string(m_generation) + "/" + std::to_string(m_numberOfGenerations); });
	m_textObservers[HIGHEST_FITNESS_TEXT] = new FunctionEventObserver<size_t>([&] { return !m_drawableMap ? 0 : m_drawableMap->GetHighestFitness(); });
	m_textObservers[HIGHEST_FITNESS_OVERALL_TEXT] = new FunctionEventObserver<size_t>([&] { return !m_drawableMap ? 0 : m_drawableMap->GetHighestFitnessOverall(); });
	m_textObservers[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT] = new FunctionTimerObserver<std::string>([&] { return std::to_string(m_waveTimer.GetTimeout() - m_waveTimer.Value()) + " seconds"; }, 0.3);
	m_textObservers[MEAN_REQUIRED_FITNESS_IMPROVEMENT] = new TypeEventObserver<double>(m_meanRequiredFitnessImprovement);

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		m_texts[i]->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[ACTIVE_MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {7}, {16} });
	m_texts[FILENAME_TYPE_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {7} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {7}, {16} });
	m_texts[PARAMETER_TYPE_TEXT]->SetPosition({ FontContext::Component(8, true), {0}, {8}, {15} });
	m_texts[POPULATION_SIZE_TEXT]->SetPosition({ FontContext::Component(7, true), {0}, {8} });
	m_texts[NUMBER_OF_GENERATIONS_TEXT]->SetPosition({ FontContext::Component(6, true), {0}, {8} });
	m_texts[CROSSOVER_PROBABILITY_TEXT]->SetPosition({ FontContext::Component(5, true), {0}, {8} });
	m_texts[MUTATION_PROBABILITY_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {8} });
	m_texts[DECREASE_MUTATION_OVER_GENERATION_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {8} });
	m_texts[SINGLE_POINT_CROSSOVER_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {8} });
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {8} });
	m_texts[POPULATION_TEXT]->SetPosition({ FontContext::Component(7, true), {0}, {8} });
	m_texts[GENERATION_TEXT]->SetPosition({ FontContext::Component(6, true), {0}, {8} });
	m_texts[HIGHEST_FITNESS_TEXT]->SetPosition({ FontContext::Component(4, true), {10}, {19} });
	m_texts[HIGHEST_FITNESS_OVERALL_TEXT]->SetPosition({ FontContext::Component(3, true), {10}, {19} });
	m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->SetPosition({ FontContext::Component(2, true), {10}, {19} });
	m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->SetPosition({ FontContext::Component(1, true), {10}, {19} });

	CoreLogger::PrintSuccess("State \"Training\" dependencies loaded correctly");
	return true;
}

void StateTraining::Draw()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			if (m_drawableVehicleBackup)
			{
				m_drawableVehicleBackup->DrawBody();
				if (m_artificialNeuralNetworkBackup)
					m_drawableVehicleBackup->DrawBeams();
			}

			if (m_drawableMapBackup)
				m_drawableMapBackup->Draw();

			m_texts[FILENAME_TYPE_TEXT]->Draw();
			m_texts[FILENAME_TEXT]->Draw();
			m_texts[PARAMETER_TYPE_TEXT]->Draw();
			m_texts[POPULATION_SIZE_TEXT]->Draw();
			m_texts[NUMBER_OF_GENERATIONS_TEXT]->Draw();
			break;
		}
		case RUNNING_MODE:
		{
			for (auto& vehicle : m_drawableVehicleFactory)
			{
				vehicle->DrawBody();

				if (!vehicle->IsActive())
					continue;

				vehicle->DrawBeams();
			}

			m_drawableMap->Draw();

			m_texts[POPULATION_TEXT]->Draw();
			m_texts[GENERATION_TEXT]->Draw();
			m_texts[HIGHEST_FITNESS_TEXT]->Draw();
			m_texts[HIGHEST_FITNESS_OVERALL_TEXT]->Draw();
			m_texts[RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Draw();
			m_texts[MEAN_REQUIRED_FITNESS_IMPROVEMENT]->Draw();
			break;
		}
		default:
			break;
	}

	m_texts[ACTIVE_MODE_TEXT]->Draw();
	m_texts[CROSSOVER_PROBABILITY_TEXT]->Draw();
	m_texts[MUTATION_PROBABILITY_TEXT]->Draw();
	m_texts[DECREASE_MUTATION_OVER_GENERATION_TEXT]->Draw();
	m_texts[SINGLE_POINT_CROSSOVER_TEXT]->Draw();
	m_texts[REQUIRED_FITNESS_IMPROVEMENT_TEXT]->Draw();
}
