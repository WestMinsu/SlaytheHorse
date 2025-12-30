#include "MainMenu.h"
#include "Debug.h"
#include "Button.h"
#include "BattleState.h"
#include "StateManager.h"

void MainMenu::Load(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] load called");
}

void MainMenu::Init(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] init called");

	engineContext.soundManager->LoadSound("GameStartSFX", "TTS/GameStart.mp3", false);
	engineContext.soundManager->LoadSound("GameExitSFX", "TTS/GameExit.mp3", false);

	engineContext.renderManager->RegisterTexture("[Texture]Button", "Textures/test1.png");
	engineContext.renderManager->RegisterMaterial("[Material]Button", "[EngineShader]default_texture", { {"u_Texture","[Texture]Button"} });

	mainText = static_cast<TextObject*>(objectManager.AddObject(std::make_unique<TextObject>(engineContext.renderManager->GetFontByTag("[Font]default"), "Slay The Horse", TextAlignH::Center, TextAlignV::Middle)));

	mainText->GetTransform2D().SetPosition({ 0, 300.0f });

	auto startButton = objectManager.AddObject(std::make_unique<Button>(glm::vec2(0, -100.f), glm::vec2(200.0f, 60.0f), "Start Game"), "[Object]button");
	auto exitButton = objectManager.AddObject(std::make_unique<Button>(glm::vec2(0, -200.f), glm::vec2(200.0f, 60.0f), "Exit"), "[Object]button");

	if (Button* startButtonPtr = static_cast<Button*>(startButton))
	{
		startButtonPtr->onClick = [this](const EngineContext& ctx) {
			this->OnStartButtonClick(ctx);
			};
	}

	if (Button* exitButtonPtr = static_cast<Button*>(exitButton))
	{
		exitButtonPtr->onClick = [this](const EngineContext& ctx) {
			this->OnExitButtonClick(ctx);
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
	if (isExiting)
	{
		exitTimer += dt;

		if (exitTimer >= 1.3f)
		{
			engineContext.engine->RequestQuit(); 
		}
	}

	if (isStarting)
	{
		startTimer += dt;

		if (startTimer >= 1.f)
		{
			if (engineContext.stateManager != nullptr)
				engineContext.stateManager->ChangeState(std::make_unique<BattleState>());
		}
	}

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
	objectManager.FreeAll(engineContext);

	engineContext.renderManager->UnregisterTexture("[Texture]Button", engineContext);
	engineContext.renderManager->UnregisterMaterial("[Material]Button", engineContext);
}

void MainMenu::Unload(const EngineContext& engineContext)
{
	JIN_LOG("[MainMenu] unload update called");
}

void MainMenu::OnStartButtonClick(const EngineContext& context)
{
	JIN_LOG("Start Button Clicked! Transition to BattleState.");
	context.soundManager->Play("GameStartSFX");
	isStarting = true;
	startTimer = 0.0f;
}

void MainMenu::OnExitButtonClick(const EngineContext& context)
{
	JIN_LOG("Exit Button Clicked!");
	context.soundManager->Play("GameExitSFX");
	isExiting = true;
	exitTimer = 0.0f;
}
