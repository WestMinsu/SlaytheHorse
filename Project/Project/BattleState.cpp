#include "BattleState.h"
#include "Card.h"

void BattleState::Init(const EngineContext& engineContext)
{
    battleManager.SetupDeck(engineContext);
    battleManager.DrawCard(3);
}

void BattleState::Update(float dt, const EngineContext& engineContext)
{
    GameState::Update(dt, engineContext);

    const auto& hand = battleManager.GetHand();
    for (size_t i = 0; i < hand.size(); ++i)
    {
        hand[i]->GetTransform2D().SetPosition({ -300.0f + (i * 200.0f), -200.0f });
    }
}