#include "Enemy.h"

Enemy::Enemy(const glm::vec2& pos, const glm::vec2& size)
{
    GetTransform2D().SetPosition(pos);
    GetTransform2D().SetScale(size);
}

void Enemy::Init(const EngineContext& engineContext)
{
    SetMesh(engineContext, "[EngineMesh]default");
    SetMaterial(engineContext, "[Material]Enemy");

    SetFlipUV_X(true);
}