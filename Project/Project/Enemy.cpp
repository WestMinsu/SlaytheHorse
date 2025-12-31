#include "Enemy.h"
#include "ObjectManager.h"
#include "StateManager.h"
#include "GameState.h"
#include "RenderManager.h"
#include <cmath>
#include "Player.h"
#include "FloatingText.h"
#include <random>

Enemy::Enemy(const std::string& name, const glm::vec2& pos, EnemyType type_)
    : enemyName(name), type(type_), isBoss(false)
{
    GetTransform2D().SetPosition(pos);
    hp = 10;
    maxHP = 10;
}

Enemy::Enemy(const glm::vec2& pos, const glm::vec2& size)
    : enemyName(u8"최종 보스 말"), isBoss(true), type(EnemyType::Boss)
{
    GetTransform2D().SetPosition(pos);
    GetTransform2D().SetScale(size);
    basePosition = pos;
    hp = 30;
    maxHP = 30;
}

void Enemy::Init(const EngineContext& engineContext)
{
    if (isBoss)
    {
        SetMesh(engineContext, "[EngineMesh]default");
        SetMaterial(engineContext, "[Material]Enemy");
        //SetFlipUV_X(true);
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

            if (type == EnemyType::Angry)
                nameDisplay->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f }); 
            else
                nameDisplay->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

            auto currentState = engineContext.stateManager->GetCurrentState();
            if (currentState)
                currentState->GetObjectManager().AddObject(std::move(textObj));
        }
    }

    auto font = engineContext.renderManager->GetFontByTag("[Font]default");
    if (font)
    {
        auto attackTextObj = std::make_unique<TextObject>(font, u8"", TextAlignH::Center, TextAlignV::Middle);
        attackDisplay = attackTextObj.get();
        attackDisplay->SetRenderLayer("[Layer]UIText");
        attackDisplay->GetTransform2D().SetDepth(0.1f);
        attackDisplay->GetTransform2D().SetScale({ 1.2f, 1.2f });
        attackDisplay->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

        engineContext.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(attackTextObj));

        auto hpBarObj = std::make_unique<GameObject>();
        hpBarObj->SetMesh(engineContext, "[EngineMesh]default");
        hpBarObj->SetMaterial(engineContext, "[Material]Card");
        hpBarObj->GetTransform2D().SetScale({ 128.f, 32.f });
        glm::vec2 pos = GetTransform2D().GetPosition();
        hpBarObj->GetTransform2D().SetPosition({ pos.x, pos.y - 100.f });
        hpBarObj->SetColor({ 0.f, 0.f, 0.f, 1.0f });
        hpBar = static_cast<GameObject*>(engineContext.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(hpBarObj)));

        auto hpTextObj = std::make_unique<TextObject>(font, std::to_string(hp), TextAlignH::Center, TextAlignV::Middle);
        hpBarText = hpTextObj.get();
        hpBarText->GetTransform2D().SetPosition(hpBar->GetTransform2D().GetPosition());
        hpBarText->GetTransform2D().SetDepth(hpBar->GetTransform2D().GetDepth() + 0.1f);
        hpBarText->SetColor({ 1.f, 1.f, 1.f, 1.f });
        engineContext.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(hpTextObj));
    }
}

void Enemy::Update(float dt, const EngineContext& engineContext)
{
    totalTime += dt;

    float speed = 5.0f;
    float amplitude = 15.0f;

    if (isBoss)
    {
        float healthRatio = (float)hp / maxHP;
        if (healthRatio < 0.0f)
            healthRatio = 0.0f;

        speed = 3.0f + (1.0f - healthRatio) * 15.0f;
        amplitude = 10.0f + (1.0f - healthRatio) * 35.0f;
    }
    else
    {
        speed = (type == EnemyType::Fast) ? 20.0f : 5.0f;
    }

    float hoverOffset = sin(totalTime * speed) * amplitude;
    glm::vec2 currentBasePos = isBoss ? basePosition : GetTransform2D().GetPosition();

    if (isBoss)
        GetTransform2D().SetPosition({ currentBasePos.x, currentBasePos.y + hoverOffset });

    float uiYOffset = isBoss ? 50.0f : 0.0f;

    if (nameDisplay)
        nameDisplay->GetTransform2D().SetPosition({ currentBasePos.x, currentBasePos.y + hoverOffset });

    if (attackDisplay)
        attackDisplay->GetTransform2D().SetPosition({ currentBasePos.x, currentBasePos.y + 85.0f + uiYOffset + hoverOffset });

    if (hpBar)
        hpBar->GetTransform2D().SetPosition({ currentBasePos.x, currentBasePos.y + 55.0f + uiYOffset + hoverOffset });

    if (hpBarText && hpBar)
        hpBarText->GetTransform2D().SetPosition(hpBar->GetTransform2D().GetPosition());

    if (attackDisplayTimer > 0.0f)
    {
        attackDisplayTimer -= dt;
        if (attackDisplayTimer <= 0.0f)
            attackDisplay->SetText(u8"");
    }
}

void Enemy::Attack(Player* player, float& currentTurnTime, const EngineContext& context)
{
    if (!player || !attackDisplay)
        return;

    attackDisplayTimer = 2.0f; 

    switch (type)
    {
    case EnemyType::Boss:
        context.soundManager->Play("Boss");
        player->ModifyHealth(-5, context);
        currentTurnTime -= 5.0f;
        attackDisplay->SetText(u8"최종 보스가 포효합니다!");
        break;
    case EnemyType::Fast:
        player->ModifyHealth(-1, context);
        currentTurnTime -= 2.0f;
        attackDisplay->SetText(u8"빠른 말이 시간을 훔쳤습니다!");
        break;

    case EnemyType::Angry:
        player->ModifyHealth(-3, context);
        attackDisplay->SetText(u8"성난 말이 강력하게 발길질합니다!");
        break;

    default:
        player->ModifyHealth(-2, context);
        attackDisplay->SetText(u8"말이 공격했습니다.");
        break;
    }
}

void Enemy::ModifyHealth(int amount, const EngineContext& context)
{
    hp = std::max(0, hp + amount);

    if (hpBarText)
        hpBarText->SetText(std::to_string(hp));

    if (amount != 0) 
    {
        Font* font = context.renderManager->GetFontByTag("[Font]default");
        if (font)
        {
            glm::vec4 color;
            std::string msg;

            if (amount > 0)
            {
                color = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);
                msg = "+" + std::to_string(amount);
            }
            else 
            {
                color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
                msg = std::to_string(amount);
                if (isBoss)
                    context.soundManager->Play("Boss");
                else
                    context.soundManager->Play("EnemyHitSFX");
            }

            static std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> dist(-30.0f, 30.0f);

            glm::vec2 enemyPos = this->GetTransform2D().GetPosition();
            glm::vec2 spawnPos = enemyPos + glm::vec2(dist(rng), dist(rng) + 50.0f);

            auto floatText = std::make_unique<FloatingText>(font, msg, spawnPos, color);

            GameState* state = context.stateManager->GetCurrentState();
            if (state)
            {
                state->GetObjectManager().AddObject(std::move(floatText));
            }
        }
    }
}

void Enemy::SetAsBoss(const EngineContext& context)
{
    isBoss = true;
    type = EnemyType::Boss;
    basePosition = GetTransform2D().GetPosition();
    SetMaterial(context, "[Material]Boss");
}

void Enemy::KillAll(const EngineContext& context)
{
    if (type == EnemyType::Boss)
    {
        Font* font = context.renderManager->GetFontByTag("[Font]default");
        glm::vec2 bossPos = GetTransform2D().GetPosition();

        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

        for (int i = 0; i < 15; i++)
        {
            glm::vec2 spawnPos = bossPos + glm::vec2(dist(rng), dist(rng));
            auto horsePiece = std::make_unique<FloatingText>(
                font, "Horse", spawnPos, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
            );

            GameState* state = context.stateManager->GetCurrentState();
            if (state)
                state->GetObjectManager().AddObject(std::move(horsePiece));
        }
    }
    if (nameDisplay) 
        nameDisplay->Kill();
    if (attackDisplay) 
        attackDisplay->Kill();
    if (hpBar) 
        hpBar->Kill();
    if (hpBarText) 
        hpBarText->Kill();
    this->Kill();
}
