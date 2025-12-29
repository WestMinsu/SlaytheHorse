#pragma once
#include "GameState.h"
#include "Card.h"
#include <vector>

class BattleState : public GameState
{
public:
    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;

    void DrawCard(int count, const EngineContext& engineContext);

private:
    std::vector<Card*> deck;
    std::vector<Card*> hand;
};