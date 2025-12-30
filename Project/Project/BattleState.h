#pragma once
#include "GameState.h"
#include "BattleManager.h"
#include "InputField.h"
#include "Player.h"
#include "Enemy.h"

enum class TurnState
{
    PlayerTurn,
    EnemyTurn,
    Transition  
};

class BattleState : public GameState
{
public:
    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;
    void Free(const EngineContext& engineContext) override;
    void ReturnToMainMenu(const EngineContext& context);

    InputField* inputField = nullptr;

    Player* player = nullptr;

    BattleManager* battleManager = nullptr;;
private:
    void OnProcessInput(const std::string& text, const EngineContext& context);

    TextObject* timerText = nullptr;
    GameObject* timerBar = nullptr;

    float maxTurnTime = 10.0f;
    float currentTurnTime = 10.0f;

    const glm::vec2 barBaseScale = { 400.0f, 20.0f };
    const glm::vec2 barBasePos = { 0.0f, 120.0f };

    TurnState currentState = TurnState::PlayerTurn;
    Enemy* enemy = nullptr;
    float transitionTimer = 0.0f;
    TextObject* turnNoticeText = nullptr;
};