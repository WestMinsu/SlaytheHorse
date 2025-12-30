#include "Enemy.h"
#include "ObjectManager.h"
#include "StateManager.h"
#include "GameState.h"
#include "RenderManager.h"
#include <cmath>
#include "Player.h"

Enemy::Enemy(const std::string& name, const glm::vec2& pos, EnemyType type_)
    : enemyName(name), type(type_), isBoss(false)
{
    GetTransform2D().SetPosition(pos);
}

Enemy::Enemy(const glm::vec2& pos, const glm::vec2& size)
    : enemyName(u8"최종 보스 말"), isBoss(true)
{
    GetTransform2D().SetPosition(pos);
    GetTransform2D().SetScale(size);
}

void Enemy::Init(const EngineContext& engineContext)
{
    if (isBoss)
    {
        SetMesh(engineContext, "[EngineMesh]default");
        SetMaterial(engineContext, "[Material]Enemy");
        SetFlipUV_X(true);
    }
    else
    {
        auto font = engineContext.renderManager->GetFontByTag("[Font]default");
        if (font)
        {
            auto textObj = std::make_unique<TextObject>(font, enemyName, TextAlignH::Center, TextAlignV::Middle);
            nameDisplay = textObj.get();
            nameDisplay->GetTransform2D().SetPosition(GetTransform2D().GetPosition());
            nameDisplay->GetTransform2D().SetDepth(0.1f);

            nameDisplay->SetRenderLayer("[Layer]UIText");
            nameDisplay->GetTransform2D().SetScale({ 2.0f, 2.0f }); 
            nameDisplay->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });

            auto currentState = engineContext.stateManager->GetCurrentState();
            if (currentState)
                currentState->GetObjectManager().AddObject(std::move(textObj));
        }
    }
}

void Enemy::Update(float dt, const EngineContext& engineContext)
{
    totalTime += dt;

    if (nameDisplay)
    {
        float hoverOffset = sin(totalTime * 5.0f) * 20.0f;
        glm::vec2 basePos = GetTransform2D().GetPosition();

        nameDisplay->GetTransform2D().SetPosition({ basePos.x, basePos.y + hoverOffset });
    }
}

void Enemy::Attack(Player* player, float& currentTurnTime, const EngineContext& context)
{
    if (!player) 
        return;

    switch (type)
    {
    case EnemyType::Fast:
        player->ModifyHealth(-1, context);
        currentTurnTime -= 2.0f;
        JIN_LOG(u8"빠른 말이 시간을 훔쳤습니다!");
        break;

    case EnemyType::Angry:
        player->ModifyHealth(-3, context);
        JIN_LOG(u8"성난 말이 강력하게 발길질합니다!");
        break;

    case EnemyType::Boss:
        player->ModifyHealth(-5, context);
        currentTurnTime -= 5.0f;
        JIN_LOG(u8"최종 보스가 포효합니다!");
        break;

    default:
        player->ModifyHealth(-2, context);
        JIN_LOG(u8"말이 공격했습니다.");
        break;
    }
}
