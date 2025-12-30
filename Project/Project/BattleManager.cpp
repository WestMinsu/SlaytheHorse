#include "BattleManager.h"
#include "Card.h"
#include "GameState.h"
#include "ObjectManager.h"
#include <algorithm>
#include <random>
#include <iostream>

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
    std::vector<CardData> cardTemplates =
    {
        { u8"빠른 말", u8"말이 빠르면 턴이 빨리 옵니다. 속사포처럼 쏟아내세요!" },
        { u8"성난 말", u8"화가 난 말은 발길질보다 무섭습니다. 강력한 한 방!" },
        { u8"말차 한 잔", u8"따뜻한 말 한마디와 차 한 잔으로 기력을 회복합니다." },
        { u8"히히힝", u8"말문이 막히게 만드는 소리입니다. 적을 1턴간 기절시킵니다." },
        { u8"발차기", u8"말의 본능적인 공격입니다. 가장 기본이 되는 기술이죠." }
    };

    for (const auto& data : cardTemplates)
    {
        CardData wrappedData = data;
        wrappedData.description = WrapText(data.description, 18);

        Card* newCard = CreateCard(context, wrappedData);
        if (newCard)
        {
            deck.push_back(newCard);
        }
    }

    std::shuffle(deck.begin(), deck.end(), std::mt19937(std::random_device()()));
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