#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
public:
    Enemy(const glm::vec2& pos, const glm::vec2& size);
    ~Enemy() override = default;

    void Init(const EngineContext& engineContext) override;
};