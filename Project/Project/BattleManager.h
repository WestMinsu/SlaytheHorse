#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

class Card;
struct EngineContext;

struct CardData
{
    std::string name;
    std::string description;
};

inline const std::vector<CardData> GlobalCardTemplates =
{
    { u8"빠른 말", u8"말이 빠르면 턴이 빨리 옵니다. 속사포처럼 쏟아내세요!" },
    { u8"강한 발차기", u8"화가 난 말은 발길질보다 무섭습니다. 적에게 4 데미지를 줍니다." },
    { u8"말차 한 잔", u8"따뜻한 말 한마디와 차 한 잔으로 체력을 1 회복합니다." },
    { u8"히히힝", u8"말문이 막히게 만드는 소리입니다. 적을 1턴간 기절시킵니다." },
    { u8"발차기", u8"말의 본능적인 공격입니다. 적에게 2 데미지를 줍니다." }
};

inline int GetCardIndex(const std::string& name)
{
    static const std::unordered_map<std::string, int> nameToIndex = []()
        {
            std::unordered_map<std::string, int> map;
            for (size_t i = 0; i < GlobalCardTemplates.size(); ++i)
            {
                map[GlobalCardTemplates[i].name] = static_cast<int>(i);
            }
            return map;
        }();

    auto it = nameToIndex.find(name);
    if (it != nameToIndex.end())
    {
        return it->second;
    }

    return -1;
}

class BattleManager
{
public:
    void SetupDeck(const EngineContext& context);
    void DrawCard(int count);
    void AddCard(const CardData& data, const EngineContext& context);

    const std::vector<Card*>& GetHand() const
    {
        return hand;
    }

    void DiscardCard(Card* card);
    void DiscardAllCardFromHand();
private:
    Card* CreateCard(const EngineContext& context, const CardData& data);

    std::vector<Card*> deck;        
    std::vector<Card*> hand;       
    std::vector<Card*> discardPile; 
};