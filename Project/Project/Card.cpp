#include "Card.h"
#include "TextObject.h"
#include "ObjectManager.h"
#include "GameState.h"    
#include "RenderManager.h"
#include "StateManager.h"
#include <iostream>

Card::Card() : GameObject()
{
}

void Card::Init(const EngineContext& engineContext)
{
    SetMesh(engineContext, "[EngineMesh]default");
    SetMaterial(engineContext, "[Material]Card");

    originalScale = { 150.0f, 210.0f };
    hoverScale = originalScale * hoverMultiplier; 

    GetTransform2D().SetScale(originalScale);
    originalDepth = GetTransform2D().GetDepth();
    basePosition = GetTransform2D().GetPosition(); 

    hoverUpOffset = (hoverScale.y - originalScale.y) / 2.0f;

    auto font = engineContext.renderManager->GetFontByTag("[Font]default");
    if (font)
    {
        auto textObj = std::make_unique<TextObject>(font, cardName, TextAlignH::Center, TextAlignV::Middle);
        textDisplay = textObj.get();
        textDisplay->SetRenderLayer("[Layer]UIText");
        textDisplay->GetTransform2D().SetDepth(originalDepth - 0.1f);
        originalTextScale = textDisplay->GetTransform2D().GetScale();

        if (engineContext.stateManager->GetCurrentState())
            engineContext.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(textObj));
    }
}

void Card::Update(float dt, const EngineContext& engineContext)
{
    if (textDisplay)
        textDisplay->GetTransform2D().SetPosition(GetTransform2D().GetPosition());
}

void Card::SetBasePosition(const glm::vec2& pos)
{
    basePosition = pos;
    if (!isHovered)
        GetTransform2D().SetPosition(basePosition);
}

void Card::SetHoverState(bool hover)
{
    if (isHovered == hover) return;

    isHovered = hover;
    if (isHovered)
    {
        GetTransform2D().SetScale(hoverScale);
        GetTransform2D().SetDepth(originalDepth - 1.0f);
        GetTransform2D().SetPosition(basePosition + glm::vec2(0.0f, hoverUpOffset));
        if (textDisplay)
        {
            textDisplay->GetTransform2D().SetScale(originalTextScale * hoverMultiplier);
            textDisplay->GetTransform2D().SetDepth(originalDepth - 1.1f);
        }
    }
    else
    {
        GetTransform2D().SetScale(originalScale);
        GetTransform2D().SetDepth(originalDepth);
        GetTransform2D().SetPosition(basePosition);

        if (textDisplay)
        {
            textDisplay->GetTransform2D().SetScale(originalTextScale);
            textDisplay->GetTransform2D().SetDepth(originalDepth - 0.1f);
        }
    }
}
glm::vec4 Card::GetBoundingBox() const
{
    glm::vec2 pos = GetWorldPosition();
    glm::vec2 scale = GetWorldScale();

    return glm::vec4(
        pos.x - scale.x / 2.0f,
        pos.x + scale.x / 2.0f,
        pos.y - scale.y / 2.0f,
        pos.y + scale.y / 2.0f
    );
}

void Card::SetCardName(const std::string& name)
{
    cardName = name;
    if (textDisplay)
        textDisplay->SetText(name);
}

std::string Card::GetCardName() const
{
    return cardName;
}