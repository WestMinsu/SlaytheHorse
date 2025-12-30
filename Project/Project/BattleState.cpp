#include "BattleState.h"
#include "Card.h"

void BattleState::Init(const EngineContext& engineContext)
{
    engineContext.renderManager->RegisterTexture("[Texture]Button", "Textures/test1.png");
    engineContext.renderManager->RegisterMaterial("[Material]Button", "[EngineShader]default_texture", { {"u_Texture","[Texture]Button"} });

    battleManager.SetupDeck(engineContext);
    battleManager.DrawCard(3);

    auto inputFieldObj = objectManager.AddObject(std::make_unique<InputField>(glm::vec2(0, 250.f), glm::vec2(300.0f, 150.0f)), "[Object]InputField");
    inputField = static_cast<InputField*>(inputFieldObj);

    inputField->onCommit = [this](const std::string& text)
        {
            this->OnProcessInput(text);
        };
}

void BattleState::Update(float dt, const EngineContext& engineContext)
{
    GameState::Update(dt, engineContext);

    const auto& hand = battleManager.GetHand();
    for (size_t i = 0; i < hand.size(); ++i)
    {
        hand[i]->GetTransform2D().SetPosition({ -300.0f + (i * 200.0f), -200.0f });
    }
}

void BattleState::Free(const EngineContext& engineContext)
{
    engineContext.renderManager->UnregisterTexture("[Texture]Button", engineContext);
    engineContext.renderManager->UnregisterMaterial("[Material]Button", engineContext);
}

void BattleState::OnProcessInput(const std::string& text)
{
    JIN_LOG("Player typed: " << text);
}
