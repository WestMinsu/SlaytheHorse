#pragma once
#include "GameState.h"
#include "BattleManager.h"
#include "InputField.h"
#include "Player.h"

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
};