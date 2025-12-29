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

	engineContext.renderManager->RegisterTexture("[Texture]Button", "Textures/test1.png");
	engineContext.renderManager->RegisterMaterial("[Material]Button", "[EngineShader]default_texture", { {"u_Texture","[Texture]Button"} });
	mainText = static_cast<TextObject*>(objectManager.AddObject(std::make_unique<TextObject>(engineContext.renderManager->GetFontByTag("[Font]default"), "Slay The Horse", TextAlignH::Center, TextAlignV::Middle)));

	mainText->GetTransform2D().SetPosition({ 0, 300.0f });

	auto startButton = objectManager.AddObject(std::make_unique<Button>(glm::vec2(0, 0), glm::vec2(200.0f, 60.0f), "Start Game"), "[Object]button");

	if (Button* startButtonPtr = static_cast<Button*>(startButton))
	{
		startButtonPtr->onClick = [this](const EngineContext& ctx) {
			this->OnStartButtonClick(ctx);
			};
	}

	engineContext.windowManager->SetBackgroundColor({0.2f, 0.2f, 0.2f, 0.2f });
	
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

void MainMenu::OnStartButtonClick(const EngineContext& context)
{
	JIN_LOG("Start Button Clicked!");

	//context.stateManager->ChangeState(std::make_unique<PlayState>());
}

