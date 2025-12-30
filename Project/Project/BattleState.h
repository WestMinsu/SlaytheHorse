#pragma once
#include "GameState.h"
#include "BattleManager.h"
#include "InputField.h"

class BattleState : public GameState
{
public:
    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;
    void Free(const EngineContext& engineContext) override;

private:
    BattleManager battleManager;

    void OnProcessInput(const std::string& text);

    InputField* inputField = nullptr;
};