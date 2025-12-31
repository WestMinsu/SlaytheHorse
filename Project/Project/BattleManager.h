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
    { u8"발차기", u8"말의 본능적인 공격입니다. 적에게 2 데미지를 줍니다." },
    { u8"말차 한 잔", u8"따뜻한 말 한마디와 차 한 잔으로 체력을 2 회복합니다." },
    { u8"강한 발차기", u8"화가 난 말은 발길질보다 무섭습니다. 적에게 4 데미지를 줍니다." },
    { u8"양말", u8"양말은 정말 따뜻합니다. 말의 힘이 1 증가합니다." },
    { u8"히히힝", u8"말문이 막히게 만드는 소리입니다. 카드 2장을 드로우합니다." },
    { u8"말 달리자", u8"말이 더 열심히 달립니다. 타이핑 시간을 2초 연장하고, 카드를 1장 드로우합니다." },
    { u8"내가 그린 기린 그림", u8"기린은 말의 베스트 프렌드입니다. 적에게 6 데미지를 줍니다." },
    { u8"슬 레이 더 홀  스", u8"반드시 끊어서 읽어야 합니다. 체력을 6 회복합니다." },
    { u8"           ", u8"띄어쓰기가 숨겨져 있습니다. 적에게 10 데미지를 줍니다." },
    { u8"맘마미아", u8"엄마 말이 없어졌습니다. 분노한 말은 체력을 1 깎고 힘이 3 증가합니다." }
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
    void DrawCard(const EngineContext& context, int count);
    void AddCard(const CardData& data, const EngineContext& context);

    const std::vector<Card*>& GetHand() const
    {
        return hand;
    }

    void ClearAllCards();
    void DiscardCard(Card* card);
    void DiscardAllCardFromHand();
private:
    Card* CreateCard(const EngineContext& context, const CardData& data);

    std::vector<Card*> deck;        
    std::vector<Card*> hand;       
    std::vector<Card*> discardPile; 
};  