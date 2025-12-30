#include "Player.h"
#include <algorithm>
#include "GameState.h"
#include "BattleState.h"

Player::Player(const glm::vec2& pos, const glm::vec2& size)
: initSize(size), initPos(pos) {}

void Player::Init(const EngineContext& context)
{
	SetMesh(context, "[EngineMesh]default");
	SetMaterial(context, "[Material]Player");

	GetTransform2D().SetPosition(initPos);
	GetTransform2D().SetScale(initSize);
	SetColor({ 1.f, 1.f, 1.f, 1.f });

	SetRenderLayer("[Layer]Player");

    auto hpBarObj = std::make_unique<GameObject>();
    hpBarObj->SetMesh(context, "[EngineMesh]default");
    hpBarObj->SetMaterial(context, "[Material]Card");

    hpBarObj->GetTransform2D().SetScale({128.f, 32.f});
    hpBarObj->GetTransform2D().SetPosition({initPos.x, initPos.y + 80.f});

    hpBarObj->SetRenderLayer("[Layer]Player");
    hpBarObj->GetTransform2D().SetDepth(GetTransform2D().GetDepth() + 0.01f);
  
    hpBarObj->SetColor({ 0.f, 0.f, 0.f, 1.0f });

	hpBar = static_cast<GameObject*>(context.stateManager->GetCurrentState()->GetObjectManager().AddObject(
        std::move(hpBarObj), GetTag() + "_Inner"
    ));

    Font* font = context.renderManager->GetFontByTag("[Font]default");

    if (font)
    {
        auto tObj = std::make_unique<TextObject>(font, std::to_string(currHP), TextAlignH::Center, TextAlignV::Middle);

        tObj->SetRenderLayer("[Layer]Player");
        tObj->GetTransform2D().SetPosition(hpBar->GetTransform2D().GetPosition());
        tObj->GetTransform2D().SetDepth(hpBar->GetTransform2D().GetDepth() + 0.01f);
        //tObj->GetTransform2D().SetScale(tObj->GetTransform2D().GetScale() * 10.f);

        tObj->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

        hpBarText = static_cast<TextObject*>(
            context.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(tObj), GetTag() + "_Text")
            );
    }
}

void Player::Update(float dt, const EngineContext& context)
{
    if (isDead)
    {
        float targetRot = glm::radians(180.0f);
        float rotateSpeed = glm::radians(180.0f);

        float currentRot = GetTransform2D().GetRotation();

        if (currentRot < targetRot)
        {
            float nextRot = currentRot + (rotateSpeed * dt);

            if (nextRot > targetRot)
            {
                nextRot = targetRot;
            }

            GetTransform2D().SetRotation(nextRot);
        }
        else
        {
            GetTransform2D().SetRotation(targetRot);

            outTimer += dt;

            if (outTimer >= 2.0f)
            {
                BattleState* BS = static_cast<BattleState*>(context.stateManager->GetCurrentState());
                if (BS)
                {
                    BS->ReturnToMainMenu(context);
                }
            }
        }
    }
}

void Player::ModifyHealth(int amount, const EngineContext& context)
{
    int prevHP = currHP;

	currHP = std::clamp(currHP + amount, 0, maxHP);

    if (prevHP == currHP) return;

    hpBarText->SetText(std::to_string(currHP));

    if (amount > 0)
    {
        context.soundManager->Play("HealSFX");
    }
    else if (amount < 0)
    {
        context.soundManager->Play("HitSFX");
    }

	if (currHP == 0)
	{
        context.soundManager->Play("DeathSFX");
        isDead = true;
        outTimer = 0.f;
        BattleState* BS = static_cast<BattleState*>(context.stateManager->GetCurrentState());
        BS->inputField->SetInteractable(false);
	}
}

int Player::GetCurrHP()
{
	return currHP;
}
