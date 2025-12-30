#pragma once
#include "GameObject.h"
#include "TextObject.h"

enum class EnemyType
{
    Normal,   
    Fast,    
    Angry,    
    Boss     
};

class Player;

class Enemy : public GameObject
{
public:
    Enemy(const std::string& name, const glm::vec2& pos, EnemyType type = EnemyType::Normal);
    Enemy(const glm::vec2& pos, const glm::vec2& size); 

    ~Enemy() override = default;

    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;
    void Attack(Player* player, float& currentTurnTime, const EngineContext& context);

private:
    std::string enemyName;
    EnemyType type;
    bool isBoss = false;
    TextObject* nameDisplay = nullptr;
    float totalTime = 0.0f;
    TextObject* attackDisplay = nullptr; 
    float attackDisplayTimer = 0.0f;
};