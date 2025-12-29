#pragma once
#include <vector>
#include <string>
#include <memory>

class Card;
struct EngineContext;

class BattleManager
{
public:
    void SetupDeck(const EngineContext& context);
    void DrawCard(int count);

    const std::vector<Card*>& GetHand() const
    {
        return hand;
    }

private:
    std::vector<Card*> deck;        
    std::vector<Card*> hand;       
    std::vector<Card*> discardPile; 
};