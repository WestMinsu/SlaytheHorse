#include "BattleState.h"
#include "Card.h"
#include "InputManager.h"
#include "Player.h"
#include "Enemy.h"
#include "MainMenu.h"
#include <random>
#include "FloatingText.h"

void BattleState::Init(const EngineContext& engineContext)
{
    engineContext.renderManager->RegisterTexture("[Texture]Button", "Textures/test1.png");
    engineContext.renderManager->RegisterMaterial("[Material]Button", "[EngineShader]default_texture", { {"u_Texture","[Texture]Button"} });

    engineContext.renderManager->RegisterTexture("[Texture]Player", "Textures/Horse.png");
    engineContext.renderManager->RegisterMaterial("[Material]Player", "[EngineShader]default_texture", { {"u_Texture","[Texture]Player"} });

    engineContext.renderManager->RegisterTexture("[Texture]Enemy", "Textures/Horse.png");
    engineContext.renderManager->RegisterMaterial("[Material]Enemy", "[EngineShader]default_texture", { {"u_Texture","[Texture]Enemy"} });

    engineContext.soundManager->LoadSound("Card1SFX", "TTS/Card1.mp3", false);
    engineContext.soundManager->LoadSound("Card2SFX", "TTS/Card2.mp3", false);
    engineContext.soundManager->LoadSound("Card3SFX", "TTS/Card3.mp3", false);
    engineContext.soundManager->LoadSound("Card4SFX", "TTS/Card4.mp3", false);
    engineContext.soundManager->LoadSound("Card5SFX", "TTS/Card5.mp3", false);
    engineContext.soundManager->LoadSound("Card6SFX", "TTS/Card6.mp3", false);
    engineContext.soundManager->LoadSound("Card7SFX", "TTS/Card7.mp3", false);
    engineContext.soundManager->LoadSound("Card8SFX", "TTS/Card8.mp3", false);
    engineContext.soundManager->LoadSound("Card9SFX", "TTS/Card9.mp3", false);
    engineContext.soundManager->LoadSound("Card10SFX", "TTS/Card10.mp3", false);


    engineContext.soundManager->LoadSound("HealSFX", "TTS/Heal.mp3", false);
    engineContext.soundManager->LoadSound("HitSFX", "TTS/Hit.mp3", false);
    engineContext.soundManager->LoadSound("DeathSFX", "TTS/Death.mp3", false);
    engineContext.soundManager->LoadSound("EnemyHitSFX", "TTS/EnemyHit.mp3", false);
    engineContext.soundManager->LoadSound("BossHitSFX", "TTS/BossHit.mp3", false);

    battleManager = new BattleManager();
    battleManager->SetupDeck(engineContext);

    auto playerObj = objectManager.AddObject(std::make_unique<Player>(glm::vec2(-300.f, 0.f), glm::vec2(128.f, 128.f)), "[Object]Player");
    player = static_cast<Player*>(playerObj);

    SpawnNextEnemy(engineContext);

    auto inputFieldObj = objectManager.AddObject(std::make_unique<InputField>(glm::vec2(0, 250.f), glm::vec2(300.0f, 150.0f)), "[Object]InputField");
    inputField = static_cast<InputField*>(inputFieldObj);

    inputField->onCommit = [this](const std::string& text, const EngineContext& context)
        {
            this->OnProcessInput(text, context);
        };

    battleManager->DrawCard(engineContext, player->drawCardCnt);

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

    auto turnNoticeObj = std::make_unique<TextObject>(font, u8"", TextAlignH::Center, TextAlignV::Middle);
    turnNoticeText = turnNoticeObj.get();
    turnNoticeText->GetTransform2D().SetPosition({ 0.0f, 250.0f });
    turnNoticeText->SetRenderLayer("[Layer]UIText");
    turnNoticeText->GetTransform2D().SetScale({ 1.5f, 1.5f });
    objectManager.AddObject(std::move(turnNoticeObj));
}

void BattleState::Update(float dt, const EngineContext& context)
{
    GameState::Update(dt, context);

    std::vector<Object*> enemies;
    objectManager.FindByTag("[Object]Enemy", enemies);

    if (enemies.empty())
    {
        if (currentState != TurnState::NextStageWait && currentRound < 4)
        {
            PrepareNextStageTransition(context);
        }
    }
    else
    {
        Enemy* enemy = static_cast<Enemy*>(enemies[0]);
        if (enemy->GetHealth() <= 0)
        {
            enemy->KillAll();
        }
    }

    if (currentState == TurnState::NextStageWait)
    {
        if (!player->IsDead())
            currentTurnTime -= dt;

        if (currentTurnTime <= 0.0f)
        {
            player->ModifyHealth(-1, context);
            nextStageTargetText = GenerateRandomSpacedText();
            if (nextStageCard)
            {
                nextStageCard->SetCardName(nextStageTargetText);
            }

            currentTurnTime = 2.5f;

            JIN_LOG(u8"실패! 다시 입력: " << nextStageTargetText);

            return;
        }
    }

    if (currentState == TurnState::PlayerTurn)
    {
        if (turnNoticeText)
            turnNoticeText->SetText(u8"");

        if (currentTurnTime > 0.0f && !player->IsDead())
            currentTurnTime -= dt;

        if (currentTurnTime <= 0.0f)
        {
            currentTurnTime = 0.0f;
            if (inputField)
                inputField->SetInteractable(false);

            battleManager->DiscardAllCardFromHand();
            currentState = TurnState::EnemyTurn;
            transitionTimer = 1.5f;

            JIN_LOG(u8"적의 턴 시작!");
        }
    }
    else if (currentState == TurnState::EnemyTurn)
    {
        if (turnNoticeText)
            turnNoticeText->SetText(u8"적 턴 입니다.");

        transitionTimer -= dt;

        if (transitionTimer <= 0.0f)
        {
            currentTurnTime = maxTurnTime;

            std::vector<Object*> enemyObjects;
            objectManager.FindByTag("[Object]Enemy", enemyObjects);

            if (!enemyObjects.empty())
            {
                Enemy* currentEnemy = static_cast<Enemy*>(enemyObjects[0]);
                currentEnemy->Attack(player, currentTurnTime, context);
            }

            currentState = TurnState::PlayerTurn;
            if (inputField)
            {
                inputField->SetInteractable(true);
                inputField->SetFocus(true);
            }
            battleManager->DrawCard(context, player->drawCardCnt);
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
    glm::vec2 mousePos = context.inputManager->GetMouseWorldPos(activeCam);

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
    if (currentState == TurnState::NextStageWait)
    {
        if (text == nextStageTargetText)
        {
            if (nextStageCard)
            {
                nextStageCard->KillAll();
            }

            nextStageCard = nullptr;
            SpawnNextEnemy(context);
            battleManager->DrawCard(context, 5);

            maxTurnTime = 10.0f;
            currentTurnTime = maxTurnTime;
            currentState = TurnState::PlayerTurn;

            return;
        }
        else
        {
            player->ModifyHealth(-1, context);

            nextStageTargetText = GenerateRandomSpacedText();
            if (nextStageCard)
            {
                nextStageCard->SetCardName(nextStageTargetText);
            }

            currentTurnTime = 2.5f;

            JIN_LOG(u8"오타! 체력이 1 깎였습니다: " << nextStageTargetText);

            return; 
        }
    }

    JIN_LOG("Player typed: " << text);

    Font* font = context.renderManager->GetFontByTag("[Font]default");

    glm::vec2 spawnPos = { 0.0f, 50.0f };

    for (const auto& card : battleManager->GetHand())
    {
        if (card->GetCardName() == text)
        {
            JIN_LOG("Commit Success: " << text);

            card->UseCard(context);

            if (font)
            {
                std::string msg = u8"사용 성공!\n" + card->GetCardName();
                auto floatText = std::make_unique<FloatingText>(
                    font, msg, spawnPos, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)
                );

                GetObjectManager().AddObject(std::move(floatText));
            }
            return;
        }
    }

    player->ModifyHealth(-1, context);

    if (font)
    {
        std::string msg = u8"사용 실패!";

        auto floatText = std::make_unique<FloatingText>(
            font, msg, spawnPos, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
        );
        GetObjectManager().AddObject(std::move(floatText));
    }
}

void BattleState::SpawnNextEnemy(const EngineContext& context)
{
    currentRound++;

    std::unique_ptr<Enemy> newEnemy;
    switch (currentRound)
    {
    case 1:
        newEnemy = std::make_unique<Enemy>(u8"말", glm::vec2(300.f, 0.f), EnemyType::Normal);
        break;
    case 2:
        newEnemy = std::make_unique<Enemy>(u8"성난 말", glm::vec2(300.f, 0.f), EnemyType::Angry);
        break;
    case 3: 
        newEnemy = std::make_unique<Enemy>(u8"빠른 말", glm::vec2(300.f, 0.f), EnemyType::Fast);
        break;
    case 4:
        newEnemy = std::make_unique<Enemy>(glm::vec2(300.f, 0.f), glm::vec2(256.f, 256.f));
        break;
    default:
        return;
    }

    objectManager.AddObject(std::move(newEnemy), "[Object]Enemy");
}

void BattleState::PrepareNextStageTransition(const EngineContext& context)
{
    if (battleManager != nullptr)
    {
        battleManager->DiscardAllCardFromHand();
    }

    currentState = TurnState::NextStageWait;
    maxTurnTime = 2.5f;
    currentTurnTime = maxTurnTime;

    nextStageTargetText = GenerateRandomSpacedText();

    auto cardObj = std::make_unique<Card>();
    cardObj->SetCardName(nextStageTargetText);
    cardObj->SetCardDescription(u8"다음 단계로 이동합니다");
    cardObj->GetTransform2D().SetPosition({ 0.0f, 0.0f });

    nextStageCard = cardObj.get();
    objectManager.AddObject(std::move(cardObj));

    if (inputField)
    {
        inputField->SetInteractable(true);
        inputField->SetFocus(true);
    }
}

void BattleState::ModifyCurrentTurnTime(int amount)
{
    currentTurnTime += amount;
}

std::string BattleState::GenerateRandomSpacedText()
{

    std::vector<std::string> syllables = { u8"다", u8"음", u8"단", u8"계", u8"로" };

    std::string result = syllables[0];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (size_t i = 1; i < syllables.size(); ++i)
    {
        if (dis(gen))
            result += " ";

        result += syllables[i];
    }

    return result;
}
