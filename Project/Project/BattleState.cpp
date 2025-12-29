#include "BattleState.h"
#include <algorithm>
#include <random>
#include <iostream>

void BattleState::Init(const EngineContext& engineContext)
{
    std::vector<std::string> names = {
        "빠른 말", "성난 말", "말차 한 잔", "히히힝", "발차기",
        "당근 먹기", "질주", "내가 그린 기린 그림", "말도 안 돼", "말자막"
    };

    for (const auto& name : names)
    {
        auto cardObj = std::make_unique<Card>();
        cardObj->SetCardName(name);

        deck.push_back(cardObj.get());
        objectManager.AddObject(std::move(cardObj));
    }

    std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device()()));
    DrawCard(3, engineContext);
}

void BattleState::Update(float dt, const EngineContext& engineContext)
{
    GameState::Update(dt, engineContext);
}

void BattleState::DrawCard(int count, const EngineContext& engineContext)
{
    for (int i = 0; i < count; ++i)
    {
        if (deck.empty())
            break;

        Card* drawnCard = deck.back();
        deck.pop_back();
        hand.push_back(drawnCard);

        std::cout << "카드 뽑음: " << drawnCard->GetCardName() << std::endl;

        drawnCard->GetTransform2D().SetPosition({ 100.0f + (hand.size() * 150.0f), 500.0f });
    }
}