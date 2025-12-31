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

	engineContext.renderManager->RegisterTexture("[Texture]Title", "Textures/Title.png");
	engineContext.renderManager->RegisterMaterial("[Material]Title", "[EngineShader]default_texture", { {"u_Texture","[Texture]Title"} });

	auto titleObj = std::make_unique<GameObject>();
	titleObj->SetMesh(engineContext, "[EngineMesh]default");
	titleObj->SetMaterial(engineContext, "[Material]Title");

	// 3. 크기 및 위치 설정
	titleObj->GetTransform2D().SetScale({ 512.0f, 304.0f }); // 이미지 원본 크기
	titleObj->GetTransform2D().SetPosition({ 0.0f, 160.f });   // 화면 중앙 배치

	// 4. Depth 설정 (중요: 버튼보다 뒤에 가도록 음수 값 설정)
	// 버튼들의 기본 Depth는 0.0f이므로, 이보다 작으면 뒤에 그려집니다.
	titleObj->GetTransform2D().SetDepth(-0.1f);

	// 5. 오브젝트 등록
	objectManager.AddObject(std::move(titleObj), "[Object]TitleImage");

	mainText = static_cast<TextObject*>(objectManager.AddObject(std::make_unique<TextObject>(engineContext.renderManager->GetFontByTag("[Font]default"), "Slay The Horse", TextAlignH::Center, TextAlignV::Middle)));

	mainText->GetTransform2D().SetPosition({ 0, 300.0f });

	mainText->SetVisibility(false);

	auto startButton = objectManager.AddObject(std::make_unique<Button>(glm::vec2(0, -100.f), glm::vec2(200.0f, 60.0f), u8"게임 시작"), "[Object]button");
	auto exitButton = objectManager.AddObject(std::make_unique<Button>(glm::vec2(0, -200.f), glm::vec2(200.0f, 60.0f), u8"게임 나가기"), "[Object]button");

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

	engineContext.windowManager->SetBackgroundColor({0.2f, 0.2f, 0.2f, 1.f });

	engineContext.windowManager->SetWindowIcon("Textures/Icon.png");
	
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

	engineContext.renderManager->UnregisterTexture("[Texture]Title", engineContext);
	engineContext.renderManager->UnregisterMaterial("[Material]Title", engineContext);
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
