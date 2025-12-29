#include "BattleManager.h"

void BattleManager::SetupDeck()
{
    std::vector<std::string> names =
    {
        "빠른 말", "성난 말", "울부짖는 말", "잠자는 말", "말차 한 잔",
        "내가 그린 기린 그림", "뒷발 차기", "말 안 듣는 말", "정중한 인사", "히히힝"
    };

    for (const auto& name : names)
    {
        auto card = std::make_unique<Card>();
        card->SetCardName(name);
        deck.push_back(std::move(card));
    }
}

void BattleManager::ShowAllCardsInDeck()
{
    std::cout << "--- 현재 덱 목록 ---" << std::endl;
    for (const auto& card : deck)
        std::cout << "[" << card->GetCardName() << "]" << std::endl;
}