#include "Card.h"
#include "TextObject.h"
#include "ObjectManager.h"
#include "GameState.h"    
#include "RenderManager.h"
#include "StateManager.h"
#include <iostream>
#include "BattleState.h"
#include "Debug.h"

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
    
    SetRenderLayer("[Layer]UIText");

    auto font = engineContext.renderManager->GetFontByTag("[Font]default");
    if (font)
    {
        auto currentState = engineContext.stateManager->GetCurrentState();
        if (!currentState)
            return;

        auto nameObj = std::make_unique<TextObject>(font, cardName, TextAlignH::Center, TextAlignV::Middle);
        textDisplay = nameObj.get();
        textDisplay->SetRenderLayer("[Layer]UIText");
        textDisplay->GetTransform2D().SetDepth(originalDepth + 0.2f);
        textDisplay->GetTransform2D().SetScale(textDisplay->GetTransform2D().GetScale() * 0.7f);
        originalNameScale = textDisplay->GetTransform2D().GetScale();
        currentState->GetObjectManager().AddObject(std::move(nameObj));

        auto lineObj = std::make_unique<GameObject>();
        separatorLine = lineObj.get();
        separatorLine->SetMesh(engineContext, "[EngineMesh]default");
        separatorLine->SetMaterial(engineContext, "[Material]Button");
        separatorLine->SetRenderLayer("[Layer]UIText");
        separatorLine->GetTransform2D().SetScale({ 110.0f, 1.5f });
        originalLineScale = separatorLine->GetTransform2D().GetScale();
        separatorLine->GetTransform2D().SetDepth(originalDepth + 0.1f);
        currentState->GetObjectManager().AddObject(std::move(lineObj));

        auto descObj = std::make_unique<TextObject>(font, cardDescription, TextAlignH::Center, TextAlignV::Top);
        descriptionDisplay = descObj.get();
        descriptionDisplay->SetRenderLayer("[Layer]UIText");
        descriptionDisplay->GetTransform2D().SetDepth(originalDepth + 0.2f);
        descriptionDisplay->GetTransform2D().SetScale(originalNameScale * 0.7f);
        originalDescScale = descriptionDisplay->GetTransform2D().GetScale();
        currentState->GetObjectManager().AddObject(std::move(descObj));
    }

    SetVisibilityAll(IsVisible());
}

void Card::Update(float dt, const EngineContext& engineContext)
{
    float currentMultiplier = isHovered ? hoverMultiplier : 1.0f;
    glm::vec2 currentPos = GetTransform2D().GetPosition();

    if (textDisplay)
        textDisplay->GetTransform2D().SetPosition(currentPos + glm::vec2(0.0f, nameYOffset * currentMultiplier));

    if (separatorLine)
        separatorLine->GetTransform2D().SetPosition(currentPos + glm::vec2(0.0f, lineYOffset * currentMultiplier));

    if (descriptionDisplay)
        descriptionDisplay->GetTransform2D().SetPosition(currentPos + glm::vec2(0.0f, descYOffset * currentMultiplier));
}

void Card::SetHoverState(bool hover)
{
    if (isHovered == hover) return;

    isHovered = hover;
    float targetDepth = isHovered ? originalDepth + 1.0f : originalDepth;
    glm::vec2 targetScale = isHovered ? hoverScale : originalScale;
    glm::vec2 targetPos = isHovered ? (basePosition + glm::vec2(0.0f, hoverUpOffset)) : basePosition;

    GetTransform2D().SetScale(targetScale);
    GetTransform2D().SetDepth(targetDepth);
    GetTransform2D().SetPosition(targetPos);

    float textMultiplier = isHovered ? hoverMultiplier : 1.0f;

    if (textDisplay)
    {
        textDisplay->GetTransform2D().SetScale(originalNameScale * textMultiplier);
        textDisplay->GetTransform2D().SetDepth(targetDepth + 0.2f);
    }

    if (separatorLine)
    {
        separatorLine->GetTransform2D().SetScale(originalLineScale * textMultiplier);
        separatorLine->GetTransform2D().SetDepth(targetDepth + 0.1f);
    }

    if (descriptionDisplay)
    {
        descriptionDisplay->GetTransform2D().SetScale(originalDescScale * textMultiplier);
        descriptionDisplay->GetTransform2D().SetDepth(targetDepth + 0.2f);
    }
}

void Card::SetCardDescription(const std::string& desc)
{
    cardDescription = desc;
    if (descriptionDisplay)
        descriptionDisplay->SetText(desc);
}

std::string Card::GetCardDescription() const
{
    return cardDescription;
}

void Card::SetBasePosition(const glm::vec2& pos)
{
    basePosition = pos;
    if (!isHovered)
        GetTransform2D().SetPosition(basePosition);
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

void Card::UseCard(const EngineContext& engineContext)
{
    JIN_LOG("Using Card: " << this->cardName);

    BattleState* BS = static_cast<BattleState*>(engineContext.stateManager->GetCurrentState());

    BS->battleManager->DiscardCard(this);
}

void Card::KillAll()
{
    textDisplay->Kill();
    descriptionDisplay->Kill();
    separatorLine->Kill();
    this->Kill();
}

void Card::SetVisibilityAll(bool _isVisible)
{
    if (textDisplay)
        textDisplay->SetVisibility(_isVisible);

    if (descriptionDisplay)
        descriptionDisplay->SetVisibility(_isVisible);

    if (separatorLine)
        separatorLine->SetVisibility(_isVisible);

    this->SetVisibility(_isVisible);
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