#include "BattleManager.h"
#include "Card.h"
#include "GameState.h"
#include "ObjectManager.h"
#include <algorithm>
#include <random>
#include <iostream>
#include "BattleState.h"

static std::string WrapText(const std::string& text, size_t maxLineLength)
{
    std::stringstream ss(text);
    std::string word;
    std::string result = "";
    std::string currentLine = "";

    while (ss >> word)
    {
        if (currentLine.length() + word.length() > maxLineLength)
        {
            result += currentLine + "\n";
            currentLine = word;
        }
        else
        {
            if (!currentLine.empty()) currentLine += " ";
            currentLine += word;
        }
    }
    result += currentLine;
    return result;
}

void BattleManager::SetupDeck(const EngineContext& context)
{
    AddCard(GlobalCardTemplates[GetCardIndex(u8"발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"말차 한 잔")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"말차 한 잔")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"말차 한 잔")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"강한 발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"발차기")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"말차 한 잔")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"말차 한 잔")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"말차 한 잔")], context);
    AddCard(GlobalCardTemplates[GetCardIndex(u8"강한 발차기")], context);

    std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device()()));
}

void BattleManager::DiscardCard(Card* card)
{
    std::cout << u8"[BattleManager] 카드 버림: " << card->GetCardName() << std::endl;

    card->SetVisibilityAll(false);
    hand.erase(std::remove(hand.begin(), hand.end(), card), hand.end());
    discardPile.push_back(card);
}

void BattleManager::DiscardAllCardFromHand()
{
    while (!hand.empty())
    {
        DiscardCard(hand[0]);
    }
}

Card* BattleManager::CreateCard(const EngineContext& context, const CardData& data)
{
    auto cardObj = std::make_unique<Card>();

    cardObj->SetCardName(data.name);
    cardObj->SetCardDescription(data.description);

    Card* ptr = cardObj.get();

    auto currentState = context.stateManager->GetCurrentState();
    if (currentState)
        currentState->GetObjectManager().AddObject(std::move(cardObj));

    return ptr;
}

void BattleManager::DrawCard(const EngineContext& context, int count)
{
    BattleState* BS = static_cast<BattleState*>(context.stateManager->GetCurrentState());

    if (count + hand.size() > BS->player->maxHandCount)
    {
        count = BS->player->maxHandCount - hand.size();
    }

    for (int i = 0; i < count; ++i)
    {
        if (deck.empty())
        {
            deck = discardPile;

            discardPile.clear();

            std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device()()));

            std::cout << u8"[BattleManager] 버린 카드 더미를 섞어 덱을 충전했습니다." << std::endl;
        }
        if (!deck.empty())
        {
            Card* drawnCard = deck.back();
            deck.pop_back();
            hand.push_back(drawnCard);
            drawnCard->SetVisibilityAll(true);
            std::cout << u8"[BattleManager] 카드 뽑음: " << drawnCard->GetCardName() << std::endl;
        }
    }
}

void BattleManager::AddCard(const CardData& data, const EngineContext& context)
{
    CardData wrappedData = data;
    wrappedData.description = WrapText(data.description, 18);

    Card* newCard = CreateCard(context, wrappedData);
    if (newCard)
    {
        deck.push_back(newCard);
    }
    newCard->SetVisibilityAll(false);
}
