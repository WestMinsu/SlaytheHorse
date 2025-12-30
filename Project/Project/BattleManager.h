#pragma once
#include <vector>
#include <string>
#include <memory>

class Card;
struct EngineContext;

struct CardData
{
    std::string name;
    std::string description;
};

class BattleManager
{
public:
    void SetupDeck(const EngineContext& context);
    void DrawCard(int count);

    const std::vector<Card*>& GetHand() const
    {
        return hand;
    }

    void RemoveCard(Card* usedCard);
    void ClearAllCards();
private:
    Card* CreateCard(const EngineContext& context, const CardData& data);

    std::vector<Card*> deck;        
    std::vector<Card*> hand;       
    std::vector<Card*> discardPile; 
};  