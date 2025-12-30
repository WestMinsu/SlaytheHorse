#include "BattleState.h"
#include "Card.h"
#include "InputManager.h"
#include "Player.h"
#include "Enemy.h"
#include "MainMenu.h"

void BattleState::Init(const EngineContext& engineContext)
{
    engineContext.renderManager->RegisterTexture("[Texture]Button", "Textures/test1.png");
    engineContext.renderManager->RegisterMaterial("[Material]Button", "[EngineShader]default_texture", { {"u_Texture","[Texture]Button"} });

    engineContext.renderManager->RegisterTexture("[Texture]Player", "Textures/Horse.png");
    engineContext.renderManager->RegisterMaterial("[Material]Player", "[EngineShader]default_texture", { {"u_Texture","[Texture]Player"} });

    engineContext.renderManager->RegisterTexture("[Texture]Enemy", "Textures/Horse.png");
    engineContext.renderManager->RegisterMaterial("[Material]Enemy", "[EngineShader]default_texture", { {"u_Texture","[Texture]Enemy"} });

    engineContext.soundManager->LoadSound("HealSFX", "TTS/Heal.mp3", false);
    engineContext.soundManager->LoadSound("HitSFX", "TTS/Hit.mp3", false);
    engineContext.soundManager->LoadSound("DeathSFX", "TTS/Death.mp3", false);

    battleManager = new BattleManager();

    battleManager->SetupDeck(engineContext);

    auto playerObj = objectManager.AddObject(std::make_unique<Player>(glm::vec2(-300.f, 0.f), glm::vec2(128.f, 128.f)), "[Object]Player");
    player = static_cast<Player*>(playerObj);

    auto normalEnemy = std::make_unique<Enemy>(u8"빠른 말", glm::vec2(300.f, 0.f), EnemyType::Fast);
    objectManager.AddObject(std::move(normalEnemy), "[Object]Enemy");

    /* // 예시 2: 보스 소환 시 (나중에 조건부로 호출)
    auto bossEnemy = std::make_unique<Enemy>(glm::vec2(300.f, 0.f), glm::vec2(256.f, 256.f));
    objectManager.AddObject(std::move(bossEnemy));
    */

    auto inputFieldObj = objectManager.AddObject(std::make_unique<InputField>(glm::vec2(0, 250.f), glm::vec2(300.0f, 150.0f)), "[Object]InputField");
    inputField = static_cast<InputField*>(inputFieldObj);

    inputField->onCommit = [this](const std::string& text, const EngineContext& context)
        {
            this->OnProcessInput(text, context);
        };

    battleManager->DrawCard(5);

    auto font = engineContext.renderManager->GetFontByTag("[Font]default");
    auto timerTextObj = std::make_unique<TextObject>(font, "Time: 10.0", TextAlignH::Center, TextAlignV::Middle);
    timerText = timerTextObj.get();
    timerText->GetTransform2D().SetPosition({ barBasePos.x, barBasePos.y + 40.0f });
    timerText->SetRenderLayer("[Layer]UIText");
    objectManager.AddObject(std::move(timerTextObj));

    auto timerBarObj = std::make_unique<GameObject>();
    timerBar = timerBarObj.get();
    timerBar->SetMesh(engineContext, "[EngineMesh]default");
    timerBar->SetMaterial(engineContext, "[Material]Button"); 
    timerBar->GetTransform2D().SetPosition(barBasePos);
    timerBar->GetTransform2D().SetScale(barBaseScale);
    timerBar->SetRenderLayer("[Layer]UIText");
    objectManager.AddObject(std::move(timerBarObj));

    currentTurnTime = maxTurnTime;
}

void BattleState::Update(float dt, const EngineContext& engineContext)
{
    GameState::Update(dt, engineContext);

    if (currentState == TurnState::PlayerTurn)
    {
        if (currentTurnTime > 0.0f)
            currentTurnTime -= dt;

        if (currentTurnTime <= 0.0f)
        {
            currentTurnTime = 0.0f;
            if (inputField)
                inputField->SetInteractable(false);

            currentState = TurnState::EnemyTurn;
            transitionTimer = 1.5f;
            JIN_LOG(u8"적의 턴 시작!");
        }
    }
    else if (currentState == TurnState::EnemyTurn)
    {
        transitionTimer -= dt;
        if (transitionTimer <= 0.0f)
        {
            currentTurnTime = maxTurnTime;

            std::vector<Object*> enemyObjects;
            objectManager.FindByTag("[Object]Enemy", enemyObjects);

            if (!enemyObjects.empty())
            {
                Enemy* currentEnemy = static_cast<Enemy*>(enemyObjects[0]);
                currentEnemy->Attack(player, currentTurnTime, engineContext);
            }

            currentState = TurnState::PlayerTurn;
            if (inputField)
            {
                inputField->SetInteractable(true);
                inputField->SetFocus(true);
            }
            battleManager->DrawCard(1);
        }
    }

    float ratio = currentTurnTime / maxTurnTime;

    glm::vec4 currentColor = (ratio <= 0.3f) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    if (timerText)
    {
        timerText->SetText("Time: " + std::to_string((int)currentTurnTime) + "." + std::to_string((int)(currentTurnTime * 10) % 10));
        timerText->SetColor(currentColor);
    }

    if (timerBar)
    {
        float newWidth = barBaseScale.x * ratio;
        timerBar->GetTransform2D().SetScale({ newWidth, barBaseScale.y });

        float offset = (barBaseScale.x - newWidth) / 2.0f;
        timerBar->GetTransform2D().SetPosition({ barBasePos.x - offset, barBasePos.y });
        timerBar->SetColor(currentColor); 
    }

    const auto& currentHand = battleManager->GetHand();
    float spacing = 170.0f;
    float totalWidth = (currentHand.size() - 1) * spacing;
    float startX = -totalWidth / 2.0f;

    Camera2D* activeCam = GetActiveCamera();
    glm::vec2 mousePos = engineContext.inputManager->GetMouseWorldPos(activeCam);

    for (size_t i = 0; i < currentHand.size(); ++i)
    {
        Card* card = currentHand[i];        
        float posX = startX + (i * spacing);
        card->SetBasePosition({ posX, -250.0f });

        glm::vec4 bounds = card->GetBoundingBox();
        bool isMouseOver = (mousePos.x >= bounds.x && mousePos.x <= bounds.y &&
            mousePos.y >= bounds.z && mousePos.y <= bounds.w);

        card->SetHoverState(isMouseOver);
    }
}

void BattleState::Free(const EngineContext& engineContext)
{
    objectManager.FreeAll(engineContext);

    engineContext.renderManager->UnregisterTexture("[Texture]Button", engineContext);
    engineContext.renderManager->UnregisterMaterial("[Material]Button", engineContext);

    /*if (battleManager != nullptr)
    {
        delete battleManager;
        battleManager = nullptr;
    }*/
}

void BattleState::ReturnToMainMenu(const EngineContext& context)
{
    if (context.stateManager != nullptr)
        context.stateManager->ChangeState(std::make_unique<MainMenu>());
}

void BattleState::OnProcessInput(const std::string& text, const EngineContext& context)
{
    JIN_LOG("Player typed: " << text);

    for (const auto& card : battleManager->GetHand())
    {
        if (card->GetCardName() == text)
        {
            JIN_LOG("Commit Success: " << text);

            card->UseCard(context);

            return;
        }
    }

    player->ModifyHealth(-1, context);
}
