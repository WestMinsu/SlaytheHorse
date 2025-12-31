#include "Player.h"
#include <algorithm>
#include "GameState.h"
#include "BattleState.h"
#include "FloatingText.h"
#include <random>

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

        auto pObj = std::make_unique<TextObject>(font, "+0", TextAlignH::Center, TextAlignV::Middle);
        powerText = pObj.get(); // 포인터 저장

        // 3. 속성 설정
        powerText->SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); // 노란색
        powerText->SetRenderLayer("[Layer]UIText"); // UI 레이어
        powerText->GetTransform2D().SetDepth(hpBarText->GetTransform2D().GetDepth()); // 맨 위에 표시

        // 4. 위치 고정 (예: 플레이어 위치 기준 오른쪽 위)
        glm::vec2 fixedPos = hpBarText->GetTransform2D().GetPosition() + glm::vec2(48.0f, 0.0f);
        powerText->GetTransform2D().SetPosition(fixedPos);

        // 5. 초기에는 힘이 0일 테니 숨김 처리
        powerText->SetVisibility(false);

        // 6. 매니저에 등록
        context.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(pObj));
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

    // --- FloatingText 생성 로직 추가 ---
    Font* font = context.renderManager->GetFontByTag("[Font]default");
    if (font)
    {
        // 1. 색상 및 텍스트 결정
        glm::vec4 color;
        std::string msg;

        if (amount > 0)
        {
            // 회복: 초록색, "+n"
            context.soundManager->Play("HealSFX");
            color = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f);
            msg = "+" + std::to_string(amount);
        }
        else // amount < 0
        {
            // 데미지: 빨간색, "-n" (음수는 자동으로 -가 붙음)
            context.soundManager->Play("HitSFX");
            color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
            msg = std::to_string(amount);
        }

        // 2. 랜덤 위치 계산
        // 플레이어 위치 기준으로 X, Y 각각 -30 ~ +30 범위 내 랜덤 오프셋
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(-30.0f, 30.0f);

        glm::vec2 playerPos = this->GetTransform2D().GetPosition();
        glm::vec2 randomOffset = { dist(rng), dist(rng) + 50.0f }; // Y는 머리 위쪽으로 뜨게 +50 정도 추가
        glm::vec2 spawnPos = playerPos + randomOffset;

        // 3. 생성 및 등록
        auto floatText = std::make_unique<FloatingText>(font, msg, spawnPos, color);

        // Player는 GameObject이므로 자신의 소유자인 ObjectManager가 있을 수도 있지만,
        // 안전하게 State를 통해 가져옵니다.
        GameState* state = context.stateManager->GetCurrentState();
        if (state)
        {
            state->GetObjectManager().AddObject(std::move(floatText));
        }
    }
    // ----------------------------------

    if (currHP == 0)
    {
        context.soundManager->Play("DeathSFX");
        isDead = true;
        outTimer = 0.f;
        BattleState* BS = static_cast<BattleState*>(context.stateManager->GetCurrentState());
        BS->inputField->SetInteractable(false);
    }
}

void Player::ModifyPower(int amount, const EngineContext& context)
{
    // 변화가 없으면 리턴
    if (amount == 0) return;

    power += amount;

    if (power < 0)
        power = 0;

    // --- FloatingText 생성 로직 ---
    Font* font = context.renderManager->GetFontByTag("[Font]default");
    if (font)
    {
        glm::vec4 color;
        std::string msg;

        // 힘이 증가할 때
        if (amount > 0)
        {
            // 노란색 (R:1.0, G:1.0, B:0.0)
            color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            msg = "+" + std::to_string(amount);

            // 힘 증가 효과음이 있다면 재생 (예시)
            // context.soundManager->Play("BuffSFX"); 
        }
        else // 힘이 감소할 때 (필요하다면 사용, 여기서는 회색 처리 예시)
        {
           /* color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
            msg = std::to_string(amount);*/

            // context.soundManager->Play("DebuffSFX");
        }

        // 랜덤 위치 계산 (플레이어 머리 위쪽 주변)
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(-30.0f, 30.0f);

        glm::vec2 playerPos = this->GetTransform2D().GetPosition();
        // Y축으로 +50.0f를 더해 머리 위쪽에 뜨게 설정
        glm::vec2 spawnPos = playerPos + glm::vec2(dist(rng), dist(rng) + 50.0f);

        // FloatingText 생성 및 ObjectManager에 등록
        auto floatText = std::make_unique<FloatingText>(font, msg, spawnPos, color);

        GameState* state = context.stateManager->GetCurrentState();
        if (state)
        {
            state->GetObjectManager().AddObject(std::move(floatText));
        }
    }

    if (powerText)
    {
        if (power > 0)
        {
            // 힘이 1 이상이면 텍스트 갱신 및 보이기
            powerText->SetText("+" + std::to_string(power));
            powerText->SetVisibility(true);
        }
        else
        {
            // 힘이 0이면 숨기기
            powerText->SetVisibility(false);
        }
    }
}

void Player::HideUI()
{
    hpBar->SetVisibility(false);
    hpBarText->SetVisibility(false);
    powerText->SetVisibility(false);
}
