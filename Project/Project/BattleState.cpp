#include "BattleState.h"
#include "Card.h"
#include "InputManager.h"

void BattleState::Init(const EngineContext& engineContext)
{
    battleManager.SetupDeck(engineContext);
    battleManager.DrawCard(5);
}

void BattleState::Update(float dt, const EngineContext& engineContext)
{
    GameState::Update(dt, engineContext);

    const auto& currentHand = battleManager.GetHand();
    float spacing = 170.0f;
    float totalWidth = (currentHand.size() - 1) * spacing;
    float startX = -totalWidth / 2.0f;

    Camera2D* activeCam = GetActiveCamera();
    glm::vec2 mousePos = engineContext.inputManager->GetMouseWorldPos(activeCam);

    for (size_t i = 0; i < currentHand.size(); ++i)
    {
        Card* card = currentHand[i];        
        float posX = startX + (i * spacing);
        card->SetBasePosition({ posX, -250.0f });

        glm::vec4 bounds = card->GetBoundingBox();
        bool isMouseOver = (mousePos.x >= bounds.x && mousePos.x <= bounds.y &&
            mousePos.y >= bounds.z && mousePos.y <= bounds.w);

        card->SetHoverState(isMouseOver);
    }
}