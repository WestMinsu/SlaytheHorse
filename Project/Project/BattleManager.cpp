#include "BattleManager.h"
#include "Card.h"
#include "GameState.h"
#include "ObjectManager.h"
#include <algorithm>
#include <random>
#include <iostream>

void BattleManager::SetupDeck(const EngineContext& context)
{
    std::vector<std::string> names = { u8"빠른 말", u8"성난 말", u8"말차 한 잔", u8"히히힝", u8"발차기" };

    for (const auto& name : names)
    {
        auto cardObj = std::make_unique<Card>();
        cardObj->SetCardName(name);

        Card* ptr = cardObj.get();
        context.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(cardObj));

        deck.push_back(ptr);
    }
    std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device()()));
}

void BattleManager::DrawCard(int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (deck.empty())
            break;

        Card* drawnCard = deck.back();
        deck.pop_back();
        hand.push_back(drawnCard);

        std::cout << u8"[BattleManager] 카드 뽑음: " << drawnCard->GetCardName() << std::endl;
    }
}