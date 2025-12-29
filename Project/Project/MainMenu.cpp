#include "MainMenu.h"
#include "Debug.h"
#include "Button.h"

void MainMenu::Load(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] load called");
}

void MainMenu::Init(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] init called");

	auto startButton = objectManager.AddObject(std::make_unique<Button>(), "[Object]button");
	
}

void MainMenu::LateInit(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] late init called");
}

void MainMenu::Update(float dt, const EngineContext& engineContext)
{
	objectManager.UpdateAll(dt, engineContext);
}

void MainMenu::LateUpdate(float dt, const EngineContext& engineContext)
{
}

void MainMenu::Draw(const EngineContext& engineContext)
{
	objectManager.DrawAll(engineContext);
}

void MainMenu::Free(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] free update called");
}

void MainMenu::Unload(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] unload update called");
}
