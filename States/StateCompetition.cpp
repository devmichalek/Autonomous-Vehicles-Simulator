#pragma once
#include "StateCompetition.hpp"
#include "CoreLogger.hpp"
#include "FilenameText.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "TypeEventObserver.hpp"
#include "FunctionEventObserver.hpp"
#include "FunctionTimerObserver.hpp"
#include "SimulatedWorld.hpp"
#include "MapPrototype.hpp"
#include "FitnessSystem.hpp"
#include "DrawableCheckpoint.hpp"

StateCompetition::StateCompetition() :
	m_numberOfVehicles(0),
	m_currentVehicle(0),
	m_maxNumberOfVehicles(10),
	m_enableUserVehicle(false, true, true, false),
	m_enableCheckpoints(false, true, true, false),
	m_zoom(1.f, 4.f, 0.3f, 1.f),
	m_zoomThreshold(m_zoom.Max()),
	m_viewTimer(1.0, 0.1),
	m_viewMovementOffset(3.0),
	m_pressedKeyTimer(0.0, 1.0, 5000)
{
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_modeStrings[PAUSED_MODE] = "Paused";
	m_mode = STOPPED_MODE;

	m_fileFormatStrings[MAP_FILE_FORMAT] = "Map";
	m_fileFormatStrings[ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT] = "Artificial Neural Network";
	m_fileFormatStrings[VEHICLE_FILE_FORMAT] = "Vehicle";
	m_fileFormat = MAP_FILE_FORMAT;

	m_parameterTypesStrings[SWITCH_VEHICLE] = "Switch vehicle";
	m_parameterTypesStrings[NUMBER_OF_BOT_VEHICLES] = "Number of bot vehicles";
	m_parameterTypesStrings[ENABLE_USER_VEHICLE] = "Enable user vehicle";
	m_parameterTypesStrings[ENABLE_CHECKPOINTS] = "Enable checkpoints";
	m_parameterType = SWITCH_VEHICLE;

	m_controlKeys.insert(std::pair(sf::Keyboard::M, CHANGE_MODE));
	m_controlKeys.insert(std::pair(sf::Keyboard::F, CHANGE_FILENAME_TYPE));
	m_controlKeys.insert(std::pair(sf::Keyboard::P, CHANGE_PARAMETER));
	m_controlKeys.insert(std::pair(sf::Keyboard::Add, INCREASE_PARAMETER));
	m_controlKeys.insert(std::pair(sf::Keyboard::Subtract, DECREASE_PARAMETER));
	m_controlKeys.insert(std::pair(sf::Keyboard::Multiply, INCREASE_ZOOM));
	m_controlKeys.insert(std::pair(sf::Keyboard::Divide, DECREASE_ZOOM));
	m_controlKeys.insert(std::pair(sf::Keyboard::Left, USER_VEHICLE_TURN_LEFT));
	m_controlKeys.insert(std::pair(sf::Keyboard::Right, USER_VEHICLE_TURN_RIGHT));
	m_controlKeys.insert(std::pair(sf::Keyboard::Up, USER_VEHICLE_DRIVE_FORWARD));
	m_controlKeys.insert(std::pair(sf::Keyboard::Down, USER_VEHICLE_DRIVE_BACKWARD));

	for (auto& pressedKey : m_pressedKeys)
		pressedKey = false;

	m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED] = "Error: One of artificial neural network is not specified!";
	m_internalErrorsStrings[ERROR_NO_MAP_SPECIFIED] = "Error: No map is specified!";
	m_internalErrorsStrings[ERROR_USER_VEHICLE_NOT_SPECIFIED] = "Error: User vehicle is not specified!";
	m_internalErrorsStrings[ERROR_ONE_OF_BOT_VEHICLES_IS_UNSPECIFIED] = "Error: One of bot vehicles is not fully specified!";
	m_internalErrorsStrings[ERROR_USER_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN] = "Error: User vehicle is in a collision with edges chain!";
	m_internalErrorsStrings[ERROR_BOT_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN] = "Error: Bot vehicle is in a collision with edges chain!";
	m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH] = "Error: One of artificial neural network's number of input neurons mismatches number of vehicle sensors!";
	m_internalErrorsStrings[ERROR_USER_VEHICLE_NOT_ENABLED] = "Error: User vehicle is not enabled!";
	m_internalErrorsStrings[ERROR_CANNOT_LOAD_ANN_FOR_USER_VEHICLE] = "Error: Cannot load artificial neural network model for user vehicle!";
	m_internalErrorsStrings[ERROR_NO_BOT_VEHICLES] = "Error: There are no bot vehicles! Please add first bot vehicle to continue.";

	m_pressedKeyTimer.MakeTimeout();
	m_simulatedWorld = nullptr;
	m_fitnessSystem = nullptr;
	m_mapPrototype = nullptr;

	// Initialize user vehicle with dummy vehicle
	m_vehicleBuilder.CreateDummy();
	m_userVehicle = nullptr;
	m_dummyVehiclePrototype = m_vehicleBuilder.Get();
	m_userVehiclePrototype = nullptr;
	m_vehicleBuilder.Clear();
	m_defaultUserVehicleTorque = VehicleBuilder::GetDefaultTorque();

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateCompetition::~StateCompetition()
{
	delete m_simulatedWorld;
	delete m_fitnessSystem;
	delete m_mapPrototype;
	delete m_dummyVehiclePrototype;
	delete m_userVehiclePrototype;
	for (const auto& vehiclePrototype : m_vehiclePrototypes)
		delete vehiclePrototype;
	for (const auto& ann : m_artificialNeuralNetworks)
		delete ann;
	for (const auto& text : m_texts)
		delete text;
	for (const auto& observer : m_textObservers)
		delete observer;
}

void StateCompetition::Reload()
{
	// Reset states
	m_mode = STOPPED_MODE;
	m_fileFormat = MAP_FILE_FORMAT;
	m_parameterType = SWITCH_VEHICLE;

	// Reset pressed keys
	for (size_t i = 0; i < m_pressedKeys.size(); ++i)
		m_pressedKeys[i] = false;

	// Reset competition parameters
	m_numberOfVehicles = 0;
	m_currentVehicle = 0;
	m_enableUserVehicle.ResetValue();
	m_enableCheckpoints.ResetValue();
	m_zoom.ResetValue();
	m_zoomThreshold = m_zoom.Max();
	m_viewTimer.Reset();
	m_pressedKeyTimer.MakeTimeout();

	// Reset objects of environment
	delete m_simulatedWorld;
	m_simulatedWorld = nullptr;
	delete m_fitnessSystem;
	m_fitnessSystem = nullptr;
	delete m_mapPrototype;
	m_mapPrototype = nullptr;
	m_userVehicle = nullptr;
	delete m_userVehiclePrototype;
	m_userVehiclePrototype = nullptr;
	m_defaultUserVehicleTorque = VehicleBuilder::GetDefaultTorque();
	m_userVehicleFilename.clear();
	m_simulatedVehicles.clear();
	for (const auto& vehiclePrototype : m_vehiclePrototypes)
		delete vehiclePrototype;
	m_vehiclePrototypes.clear();
	for (const auto& artificialNeuralNetwork : m_artificialNeuralNetworks)
		delete artificialNeuralNetwork;
	m_artificialNeuralNetworks.clear();
	m_botVehicleFilenames.clear();
	m_botArtificialNeuralNetworkFilenames.clear();

	// Clear builders
	m_mapBuilder.Clear();
	m_vehicleBuilder.Clear();
	m_artificialNeuralNetworkBuilder.Clear();

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}

	CoreWindow::Reset();
}

void StateCompetition::Capture()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (!filenameText->IsRenaming())
	{
		switch (m_mode)
		{
			case STOPPED_MODE:
			{
				filenameText->Capture();
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					const auto eventKey = CoreWindow::GetEvent().key.code;
					const auto iterator = m_controlKeys.find(eventKey);
					if (iterator != m_controlKeys.end() && !m_pressedKeys[iterator->second])
					{
						m_pressedKeys[iterator->second] = true;
						switch (iterator->second)
						{
							case CHANGE_MODE:
							{
								StatusText* modeText = static_cast<StatusText*>(m_texts[MODE_TEXT]);
								if (!m_mapPrototype)
								{
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_MAP_SPECIFIED]);
									break;
								}

								if (m_enableUserVehicle)
								{
									if (!m_userVehiclePrototype)
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_USER_VEHICLE_NOT_SPECIFIED]);
										break;
									}

									if (m_mapPrototype->IsCollision(m_userVehiclePrototype))
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_USER_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN]);
										break;
									}
								}

								bool error = false;
								for (size_t i = 0; i < m_numberOfVehicles; ++i)
								{
									if (!m_vehiclePrototypes[i])
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ONE_OF_BOT_VEHICLES_IS_UNSPECIFIED]);
										error = true;
										break;
									}

									m_vehiclePrototypes[i]->SetCenter(m_dummyVehiclePrototype->GetCenter());
									m_vehiclePrototypes[i]->SetAngle(m_dummyVehiclePrototype->GetAngle());
									m_vehiclePrototypes[i]->Update();

									if (m_mapPrototype->IsCollision(m_vehiclePrototypes[i]))
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_BOT_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN]);
										error = true;
										break;
									}

									if (!m_artificialNeuralNetworks[i])
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED]);
										error = true;
										break;
									}

									if (m_artificialNeuralNetworks[i]->GetNumberOfInputNeurons() != m_vehiclePrototypes[i]->GetNumberOfSensors())
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH]);
										error = true;
										break;
									}
								}

								if (error)
									break;

								// Calculate total number of simulated vehicles
								size_t totalNumberOfSimulatedVehicles = m_numberOfVehicles;
								if (m_enableUserVehicle)
									++totalNumberOfSimulatedVehicles;

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

								// Init simulated world
								delete m_simulatedWorld;
								m_simulatedWorld = new SimulatedWorld;
								m_simulatedWorld->AddMap(m_mapPrototype);
								DrawableCheckpoint::SetVisibility(m_enableCheckpoints);
								delete m_fitnessSystem;
								m_fitnessSystem = new FitnessSystem(totalNumberOfSimulatedVehicles, m_mapPrototype->GetNumberOfCheckpoints(), 0.0);
								m_simulatedWorld->AddBeginContactFunction(m_fitnessSystem->GetBeginContactFunction());

								// Add bot vehicles to the world
								m_simulatedVehicles.resize(totalNumberOfSimulatedVehicles, nullptr);
								for (size_t i = 0; i < m_numberOfVehicles; ++i)
									m_simulatedVehicles[i] = m_simulatedWorld->AddVehicle(m_vehiclePrototypes[i]);

								// Prepare user vehicle
								m_userVehicle = nullptr;
								if (m_enableUserVehicle)
								{
									m_userVehicle = m_simulatedWorld->AddVehicle(m_userVehiclePrototype);
									m_simulatedVehicles.back() = m_userVehicle;
								}

								m_mode = RUNNING_MODE;
								m_textObservers[MODE_TEXT]->Notify();
								break;
							}
							case CHANGE_FILENAME_TYPE:
							{
								++m_fileFormat;
								if (m_fileFormat >= FILE_FORMATS_COUNT)
									m_fileFormat = MAP_FILE_FORMAT;
								m_textObservers[FILE_FORMAT_TEXT]->Notify();
								break;
							}
							case CHANGE_PARAMETER:
							{
								++m_parameterType;
								if (m_parameterType >= PARAMETERS_COUNT)
									m_parameterType = SWITCH_VEHICLE;
								m_textObservers[PARAMETER_TYPE_TEXT]->Notify();
								break;
							}
							case INCREASE_PARAMETER:
							{
								switch (m_parameterType)
								{
									case SWITCH_VEHICLE:
										if (m_numberOfVehicles > 1)
										{
											++m_currentVehicle;
											if (m_currentVehicle >= m_numberOfVehicles)
												m_currentVehicle = 0;
											m_textObservers[BOT_VEHICLE_TEXT]->Notify();
										}
										break;
									case NUMBER_OF_BOT_VEHICLES:
										OnAddVehicle();
										break;
									case ENABLE_USER_VEHICLE:
										if (!m_enableUserVehicle)
										{
											m_enableUserVehicle.Increase();
											m_textObservers[ENABLE_USER_VEHICLE_TEXT]->Notify();
											m_textObservers[USER_VEHICLE_TEXT]->Notify();
										}
										break;
									case ENABLE_CHECKPOINTS:
										if (!m_enableCheckpoints)
										{
											m_enableCheckpoints.Increase();
											m_textObservers[ENABLE_CHECKPOINTS_TEXT]->Notify();
										}
										break;
								}
								
								break;
							}
							case DECREASE_PARAMETER:
							{
								switch (m_parameterType)
								{
									case SWITCH_VEHICLE:
										if (m_numberOfVehicles > 1)
										{
											if (m_currentVehicle == 0)
												m_currentVehicle = m_numberOfVehicles - 1;
											else
												--m_currentVehicle;
											m_textObservers[BOT_VEHICLE_TEXT]->Notify();
										}
										break;
									case NUMBER_OF_BOT_VEHICLES:
										OnRemoveVehicle();
										break;
									case ENABLE_USER_VEHICLE:
										if (m_enableUserVehicle)
										{
											delete m_userVehiclePrototype;
											m_userVehiclePrototype = nullptr;
											m_defaultUserVehicleTorque = VehicleBuilder::GetDefaultTorque();
											m_userVehicleFilename.clear();
											m_enableUserVehicle.Decrease();
											m_textObservers[ENABLE_USER_VEHICLE_TEXT]->Notify();
											m_textObservers[USER_VEHICLE_TEXT]->Notify();
										}
										break;
									case ENABLE_CHECKPOINTS:
										if (m_enableCheckpoints)
										{
											m_enableCheckpoints.Decrease();
											m_textObservers[ENABLE_CHECKPOINTS_TEXT]->Notify();
										}
										break;
								}

								break;
							}
							case INCREASE_ZOOM:
							case DECREASE_ZOOM:
							case USER_VEHICLE_TURN_LEFT:
							case USER_VEHICLE_TURN_RIGHT:
							case USER_VEHICLE_DRIVE_FORWARD:
							case USER_VEHICLE_DRIVE_BACKWARD:
								break;
						}
					}
				}

				break;
			}
			case RUNNING_MODE:
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
							CoreWindow::SetViewCenter(m_dummyVehiclePrototype->GetCenter());
							break;
						}
						case CHANGE_FILENAME_TYPE:
							break;
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
						case USER_VEHICLE_TURN_LEFT:
						case USER_VEHICLE_TURN_RIGHT:
						case USER_VEHICLE_DRIVE_FORWARD:
						case USER_VEHICLE_DRIVE_BACKWARD:
							break;
						case PAUSED_CHANGE_MODE:
						{
							if (m_pressedKeys[iterator->second])
								break;
							m_mode = PAUSED_MODE;
							m_textObservers[MODE_TEXT]->Notify();
							break;
						}
						default:
							break;
					}

					m_pressedKeys[iterator->second] = true;
				}
				break;
			case PAUSED_MODE:
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
							CoreWindow::SetViewCenter(m_dummyVehiclePrototype->GetCenter());
							break;
						}
						case CHANGE_FILENAME_TYPE:
						case INCREASE_PARAMETER:
						case DECREASE_PARAMETER:
						case INCREASE_ZOOM:
						case DECREASE_ZOOM:
						case USER_VEHICLE_TURN_LEFT:
						case USER_VEHICLE_TURN_RIGHT:
						case USER_VEHICLE_DRIVE_FORWARD:
						case USER_VEHICLE_DRIVE_BACKWARD:
							break;
						case PAUSED_CHANGE_MODE:
						{
							if (m_pressedKeys[iterator->second])
								break;
							m_mode = RUNNING_MODE;
							m_textObservers[MODE_TEXT]->Notify();
							break;
						}
						default:
							break;
					}

					m_pressedKeys[iterator->second] = true;
				}
				break;
			default:
				break;
		}
	}
	else
		filenameText->Capture();

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
				case CHANGE_PARAMETER:
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

void StateCompetition::Update()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
			if (filenameText->IsReading())
			{
				switch (m_fileFormat)
				{
					case MAP_FILE_FORMAT:
					{
						const bool success = m_mapBuilder.Load(filenameText->GetFilename());
						const auto status = m_mapBuilder.GetLastOperationStatus();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Prepare map prototype
						delete m_mapPrototype;
						m_mapPrototype = m_mapBuilder.Get();
						m_mapPrototype->CalculateProperties();
						DrawableCheckpoint::SetVisibility(m_enableCheckpoints);

						// Set dummy vehicle
						m_dummyVehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
						m_dummyVehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
						m_dummyVehiclePrototype->Update();

						// Set user vehicle
						if (m_userVehiclePrototype)
						{
							m_userVehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
							m_userVehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
							m_userVehiclePrototype->Update();
						}

						// Set bot vehicles
						for (auto& vehiclePrototype : m_vehiclePrototypes)
						{
							if (!vehiclePrototype)
								continue;
							vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
							vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
							vehiclePrototype->Update();
						}

						// Reset view so that the center is vehicle starting position
						CoreWindow::SetViewCenter(m_dummyVehiclePrototype->GetCenter());
						break;
					}
					case ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT:
					{
						if (m_parameterType == ENABLE_USER_VEHICLE)
						{
							filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_CANNOT_LOAD_ANN_FOR_USER_VEHICLE]);
							break;
						}
						else
						{
							if (m_numberOfVehicles == 0)
							{
								filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_BOT_VEHICLES]);
								break;
							}

							const bool success = m_artificialNeuralNetworkBuilder.Load(filenameText->GetFilename());
							const auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();
							if (!success)
							{
								filenameText->SetErrorStatusText(status.second);
								break;
							}
							filenameText->SetSuccessStatusText(status.second);

							// Update specified artificial neural network
							delete m_artificialNeuralNetworks[m_currentVehicle];
							m_artificialNeuralNetworks[m_currentVehicle] = m_artificialNeuralNetworkBuilder.Get();
							m_artificialNeuralNetworks[m_currentVehicle]->SetFromRawData(m_artificialNeuralNetworkBuilder.GetRawNeuronData());
							m_botArtificialNeuralNetworkFilenames[m_currentVehicle] = filenameText->GetFilename();
							m_textObservers[BOT_VEHICLE_TEXT]->Notify();
						}
						break;
					}
					case VEHICLE_FILE_FORMAT:
					{
						if (m_parameterType == ENABLE_USER_VEHICLE)
						{
							if (!m_enableUserVehicle)
							{
								filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_USER_VEHICLE_NOT_ENABLED]);
								break;
							}

							const bool success = m_vehicleBuilder.Load(filenameText->GetFilename());
							const auto status = m_vehicleBuilder.GetLastOperationStatus();
							if (!success)
							{
								filenameText->SetErrorStatusText(status.second);
								break;
							}
							filenameText->SetSuccessStatusText(status.second);

							// Set user vehicle
							delete m_userVehiclePrototype;
							m_userVehiclePrototype = m_vehicleBuilder.Get();
							m_userVehiclePrototype->SetCenter(m_dummyVehiclePrototype->GetCenter());
							m_userVehiclePrototype->SetAngle(m_dummyVehiclePrototype->GetAngle());
							m_userVehiclePrototype->Update();
							const auto inertiaRatio = VehicleBuilder::CalculateInertia(m_userVehiclePrototype->GetBodyPoints()) / VehicleBuilder::GetMaxInertia();
							m_defaultUserVehicleTorque = VehicleBuilder::GetDefaultTorque() * inertiaRatio;
							m_userVehicleFilename = filenameText->GetFilename();
							m_textObservers[USER_VEHICLE_TEXT]->Notify();
						}
						else
						{
							if (m_numberOfVehicles == 0)
							{
								filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_BOT_VEHICLES]);
								break;
							}

							const bool success = m_vehicleBuilder.Load(filenameText->GetFilename());
							const auto status = m_vehicleBuilder.GetLastOperationStatus();
							if (!success)
							{
								filenameText->SetErrorStatusText(status.second);
								break;
							}
							filenameText->SetSuccessStatusText(status.second);

							// Update specified bot vehicle
							delete m_vehiclePrototypes[m_currentVehicle];
							m_vehiclePrototypes[m_currentVehicle] = m_vehicleBuilder.Get();
							m_vehiclePrototypes[m_currentVehicle]->SetCenter(m_dummyVehiclePrototype->GetCenter());
							m_vehiclePrototypes[m_currentVehicle]->SetAngle(m_dummyVehiclePrototype->GetAngle());
							m_vehiclePrototypes[m_currentVehicle]->Update();
							m_botVehicleFilenames[m_currentVehicle] = filenameText->GetFilename();
							m_textObservers[BOT_VEHICLE_TEXT]->Notify();
						}
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
			const auto currentLeaderindex = m_fitnessSystem->MarkLeader(m_simulatedVehicles);
			if (m_userVehicle)
			{
				m_userVehicle->Update(m_simulatedWorld->GetEdgesWorld());

				if (m_userVehicle->IsActive())
				{
					if (m_pressedKeys[USER_VEHICLE_TURN_LEFT])
					{
						m_userVehicle->Turn(VehicleBuilder::GetDefaultTorque() - m_defaultUserVehicleTorque);
					}
					else if (m_pressedKeys[USER_VEHICLE_TURN_RIGHT])
					{
						m_userVehicle->Turn(VehicleBuilder::GetDefaultTorque() + m_defaultUserVehicleTorque);
					}

					if (m_pressedKeys[USER_VEHICLE_DRIVE_FORWARD])
					{
						m_userVehicle->DriveForward(1.0);
					}

					if (m_pressedKeys[USER_VEHICLE_DRIVE_BACKWARD])
					{
						m_userVehicle->DriveBackward(1.0);
					}
				}

				if (m_zoom < m_zoomThreshold)
					CoreWindow::SetViewCenter(m_userVehicle->GetCenter());
			}
			else if (m_zoom < m_zoomThreshold && m_viewTimer.Update())
			{
				sf::Vector2f m_viewCenter = m_dummyVehiclePrototype->GetCenter();
				if (m_numberOfVehicles)
					m_viewCenter = m_simulatedVehicles[currentLeaderindex]->GetCenter();

				// Update view
				auto viewCenter = CoreWindow::GetViewCenter();
				const auto distance = MathContext::Distance(viewCenter, m_viewCenter);
				const auto angle = MathContext::DifferenceVectorAngle(viewCenter, m_viewCenter);
				viewCenter = MathContext::GetEndPoint(viewCenter, angle, float(-distance * m_viewMovementOffset * CoreWindow::GetElapsedTime()));
				CoreWindow::SetViewCenter(viewCenter);
			}

			// Iterate over all bot vehicles, remember that in simulated vehicles
			// the last vehicle is user vehicle
			for (size_t i = 0; i < m_numberOfVehicles; ++i)
			{
				m_simulatedVehicles[i]->Update(m_simulatedWorld->GetEdgesWorld());
				if (!m_simulatedVehicles[i]->IsActive())
					continue;
				const NeuronLayer& input = m_simulatedVehicles[i]->ProcessOutput();
				const NeuronLayer& output = m_artificialNeuralNetworks[i]->Update(input);
				m_simulatedVehicles[i]->ProcessInput(output);
			}

			m_simulatedWorld->Update();
			break;
		}
		case PAUSED_MODE:
		default:
			break;
	}

	for (auto& text : m_texts)
		text->Update();
}

bool StateCompetition::Load()
{
	// Create texts
	m_texts[MODE_TEXT] = new StatusText({ "Mode:", "", "| [M] [P]" });
	m_texts[FILE_FORMAT_TEXT] = new TripleText({ "File format:", "", "| [F]" });
	m_texts[FILENAME_TEXT] = new FilenameText<true, false>("map.bin");
	m_texts[PARAMETER_TYPE_TEXT] = new TripleText({ "Parameter type:", "", "| [P] [+] [-]" });
	m_texts[USER_VEHICLE_TEXT] = new DoubleText({ "User vehicle:" });
	m_texts[BOT_VEHICLE_TEXT] = new DoubleText({ "Bot vehicle:" });
	m_texts[NUMBER_OF_BOT_VEHICLES_TEXT] = new DoubleText({ m_parameterTypesStrings[NUMBER_OF_BOT_VEHICLES] + ":" });
	m_texts[ENABLE_CHECKPOINTS_TEXT] = new DoubleText({ m_parameterTypesStrings[ENABLE_CHECKPOINTS] + ":" });
	m_texts[ENABLE_USER_VEHICLE_TEXT] = new DoubleText({ m_parameterTypesStrings[ENABLE_USER_VEHICLE] + ":" });
	m_texts[USER_FITNESS_TEXT] = new DoubleText({ "User fitness:" });
	m_texts[ZOOM_TEXT] = new TripleText({ "Zoom:", "", "| [/] [*]" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[FILE_FORMAT_TEXT] = new FunctionEventObserver<std::string>([&] { return m_fileFormatStrings[m_fileFormat]; });
	m_textObservers[PARAMETER_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_parameterTypesStrings[m_parameterType]; });
	m_textObservers[USER_VEHICLE_TEXT] = new FunctionEventObserver<std::string>([&] { return GetUserVehicleName(); });
	m_textObservers[BOT_VEHICLE_TEXT] = new FunctionEventObserver<std::string>([&] { return GetBotVehicleName(); });
	m_textObservers[NUMBER_OF_BOT_VEHICLES_TEXT] = new TypeEventObserver<size_t>(m_numberOfVehicles);
	m_textObservers[ENABLE_CHECKPOINTS_TEXT] = new FunctionEventObserver<bool>([&] { return m_enableCheckpoints; });
	m_textObservers[ENABLE_USER_VEHICLE_TEXT] = new FunctionEventObserver<bool>([&] { return m_enableUserVehicle; });
	m_textObservers[USER_FITNESS_TEXT] = new FunctionTimerObserver<std::string>([&]{ return !m_userVehicle ? "Unknown" : std::to_string(size_t(m_fitnessSystem->ToFitnessRatio(m_userVehicle->GetFitness()))) + "%"; }, 0.5);
	m_textObservers[ZOOM_TEXT] = new FunctionEventObserver<float>([&] { return m_zoom; });

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		((DoubleText*)m_texts[i])->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {8}, {14} });
	m_texts[FILE_FORMAT_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {8} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {8}, {14} });
	m_texts[PARAMETER_TYPE_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {5}, {10} });
	m_texts[USER_VEHICLE_TEXT]->SetPosition({ FontContext::Component(2, true), {14}, {17} });
	m_texts[BOT_VEHICLE_TEXT]->SetPosition({ FontContext::Component(1, true), {14}, {17} });
	m_texts[NUMBER_OF_BOT_VEHICLES_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {5} });
	m_texts[ENABLE_CHECKPOINTS_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {5} });
	m_texts[ENABLE_USER_VEHICLE_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {5} });
	m_texts[USER_FITNESS_TEXT]->SetPosition({ FontContext::Component(1, true), {7}, {10} });
	m_texts[ZOOM_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {8} });

	CoreLogger::PrintSuccess("StateCompetition dependencies loaded correctly");
	return true;
}

void StateCompetition::Draw()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
			if (m_mapPrototype)
			{
				if (m_enableCheckpoints)
					m_mapPrototype->DrawCheckpoints();
				m_mapPrototype->DrawEdges();

				if (m_enableUserVehicle)
				{
					if (m_userVehiclePrototype)
					{
						m_userVehiclePrototype->DrawBody();
						m_userVehiclePrototype->DrawBeams();
						m_userVehiclePrototype->DrawSensors();
					}
				}

				for (size_t i = 0; i < m_numberOfVehicles; ++i)
				{
					if (!m_vehiclePrototypes[i])
						continue;
					m_vehiclePrototypes[i]->DrawBody();
					if (m_artificialNeuralNetworks[i])
						m_vehiclePrototypes[i]->DrawBeams();
					m_vehiclePrototypes[i]->DrawSensors();
				}
			}

			m_texts[FILE_FORMAT_TEXT]->Draw();
			m_texts[FILENAME_TEXT]->Draw();
			m_texts[PARAMETER_TYPE_TEXT]->Draw();
			m_texts[USER_VEHICLE_TEXT]->Draw();
			m_texts[BOT_VEHICLE_TEXT]->Draw();
			m_texts[NUMBER_OF_BOT_VEHICLES_TEXT]->Draw();
			break;
		case RUNNING_MODE:
			m_simulatedWorld->Draw();
			if (m_enableUserVehicle)
				m_texts[USER_FITNESS_TEXT]->Draw();
			m_texts[ZOOM_TEXT]->Draw();
			break;
		case PAUSED_MODE:
			m_simulatedWorld->Draw();
			if (m_enableUserVehicle)
				m_texts[USER_FITNESS_TEXT]->Draw();
			break;
		default:
			break;
	}
	
	m_texts[MODE_TEXT]->Draw();
	m_texts[ENABLE_CHECKPOINTS_TEXT]->Draw();
	m_texts[ENABLE_USER_VEHICLE_TEXT]->Draw();
}

void StateCompetition::OnAddVehicle()
{
	if (m_numberOfVehicles < m_maxNumberOfVehicles)
	{
		if (m_numberOfVehicles)
		{
			VehiclePrototypes vehiclePrototypesTemp(m_numberOfVehicles + 1, nullptr);
			ArtificialNeuralNetworks artificialNeuralNetworksTemp(m_numberOfVehicles + 1, nullptr);
			std::vector<std::string> botVehicleFilenames(m_numberOfVehicles + 1, "");
			std::vector<std::string> botArtificialNeuralNetworkFilenames(m_numberOfVehicles + 1, "");

			for (size_t i = 0; i <= m_currentVehicle; ++i)
			{
				vehiclePrototypesTemp[i] = m_vehiclePrototypes[i];
				artificialNeuralNetworksTemp[i] = m_artificialNeuralNetworks[i];
				botVehicleFilenames[i] = m_botVehicleFilenames[i];
				botArtificialNeuralNetworkFilenames[i] = m_botArtificialNeuralNetworkFilenames[i];
			}

			vehiclePrototypesTemp[m_currentVehicle + 1] = nullptr;
			artificialNeuralNetworksTemp[m_currentVehicle + 1] = nullptr;
			botVehicleFilenames[m_currentVehicle + 1] = "";
			botArtificialNeuralNetworkFilenames[m_currentVehicle + 1] = "";

			for (size_t i = m_currentVehicle + 1; i < m_numberOfVehicles; ++i)
			{
				vehiclePrototypesTemp[i + 1] = m_vehiclePrototypes[i];
				artificialNeuralNetworksTemp[i + 1] = m_artificialNeuralNetworks[i];
				botVehicleFilenames[i + 1] = m_botVehicleFilenames[i];
				botArtificialNeuralNetworkFilenames[i + 1] = m_botArtificialNeuralNetworkFilenames[i];
			}

			m_vehiclePrototypes = vehiclePrototypesTemp;
			m_artificialNeuralNetworks = artificialNeuralNetworksTemp;
			m_botVehicleFilenames = botVehicleFilenames;
			m_botArtificialNeuralNetworkFilenames = botArtificialNeuralNetworkFilenames;
			++m_numberOfVehicles;
		}
		else
		{
			++m_numberOfVehicles;
			m_vehiclePrototypes.resize(m_numberOfVehicles, nullptr);
			m_artificialNeuralNetworks.resize(m_numberOfVehicles, nullptr);
			m_botVehicleFilenames.resize(m_numberOfVehicles, "");
			m_botArtificialNeuralNetworkFilenames.resize(m_numberOfVehicles, "");
		}

		m_textObservers[BOT_VEHICLE_TEXT]->Notify();
		m_textObservers[NUMBER_OF_BOT_VEHICLES_TEXT]->Notify();
	}
}

void StateCompetition::OnRemoveVehicle()
{
	if (m_numberOfVehicles)
	{
		delete m_vehiclePrototypes[m_currentVehicle];
		delete m_artificialNeuralNetworks[m_currentVehicle];

		for (size_t i = m_currentVehicle + 1; i < m_numberOfVehicles; ++i)
		{
			m_vehiclePrototypes[i - 1] = m_vehiclePrototypes[i];
			m_artificialNeuralNetworks[i - 1] = m_artificialNeuralNetworks[i];
			m_botVehicleFilenames[i - 1] = m_botVehicleFilenames[i];
			m_botArtificialNeuralNetworkFilenames[i - 1] = m_botArtificialNeuralNetworkFilenames[i];
		}

		m_vehiclePrototypes.pop_back();
		m_artificialNeuralNetworks.pop_back();
		m_botVehicleFilenames.pop_back();
		m_botArtificialNeuralNetworkFilenames.pop_back();

		if (m_currentVehicle)
			--m_currentVehicle;

		--m_numberOfVehicles;
		m_textObservers[BOT_VEHICLE_TEXT]->Notify();
		m_textObservers[NUMBER_OF_BOT_VEHICLES_TEXT]->Notify();
	}
}

std::string StateCompetition::GetBotVehicleName() const
{
	if (m_numberOfVehicles == 0)
		return "None";

	std::string data = " | Body \"";
	data += m_botVehicleFilenames[m_currentVehicle];
	data += "\" | Artificial Neural Network \"";
	data += m_botArtificialNeuralNetworkFilenames[m_currentVehicle];
	data += "\"";
	return std::string("Bot Vehicle nr. ") + std::to_string(m_currentVehicle) + data;
}

std::string StateCompetition::GetUserVehicleName() const
{
	if (!m_enableUserVehicle)
		return "None";
	return std::string("Body \"") + m_userVehicleFilename + "\"";
}
