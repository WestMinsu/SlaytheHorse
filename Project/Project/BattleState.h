#pragma once
#include "GameState.h"
#include "BattleManager.h"

class BattleState : public GameState
{
public:
    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;

private:
    BattleManager battleManager; 
};