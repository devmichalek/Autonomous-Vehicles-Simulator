#include "StateVehicleEditor.hpp"
#include "CoreLogger.hpp"
#include "CoreWindow.hpp"

StateVehicleEditor::StateVehicleEditor()
{
}

StateVehicleEditor::~StateVehicleEditor()
{
}

void StateVehicleEditor::Reload()
{
	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);
}

void StateVehicleEditor::Capture()
{

}

void StateVehicleEditor::Update()
{

}

bool StateVehicleEditor::Load()
{
	CoreLogger::PrintSuccess("State \"Vehicle Editor\" dependencies loaded correctly");
	return true;
}

void StateVehicleEditor::Draw()
{

}