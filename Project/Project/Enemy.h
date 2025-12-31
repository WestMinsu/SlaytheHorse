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
    void KillAll(const EngineContext& context);
    void ModifyHealth(int amount, const EngineContext& context);
    void SetAsBoss(const EngineContext& context);

    int GetHealth() const
    {
        return hp;
    }

private:
    std::string enemyName;
    EnemyType type;
    bool isBoss = false;
    TextObject* nameDisplay = nullptr;
    float totalTime = 0.0f;
    TextObject* attackDisplay = nullptr; 
    float attackDisplayTimer = 0.0f;
    int hp = 10;
    int maxHP = 10;
    GameObject* hpBar = nullptr;
    TextObject* hpBarText = nullptr;
    float oscillationTime = 0.0f;
    glm::vec2 basePosition = { 0.0f, 0.0f };
};