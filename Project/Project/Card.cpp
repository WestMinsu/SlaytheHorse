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
    BattleState* BS = static_cast<BattleState*>(engineContext.stateManager->GetCurrentState());

    if (this->cardName == u8"발차기")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card1SFX"); 
       //
    }
    else if (this->cardName == u8"말차 한 잔")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card2SFX");

        BS->player->ModifyHealth(2, engineContext);
    }
    else if (this->cardName == u8"강한 발차기")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card3SFX");
        //
    }
    else if (this->cardName == u8"양말")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card4SFX");

        BS->player->ModifyPower(1, engineContext);
    }
    else if (this->cardName == u8"히히힝")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card5SFX");

        BS->battleManager->DrawCard(engineContext, 2);
    }
    else if (this->cardName == u8"말 달리자")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card6SFX");

        BS->battleManager->DrawCard(engineContext, 1);
        BS->ModifyCurrentTurnTime(2);
    }
    else if (this->cardName == u8"내가 그린 기린 그림")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card7SFX");
        //
    }
    else if (this->cardName == u8"슬 레이 더 홀  스")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card8SFX");

        BS->player->ModifyHealth(6, engineContext);
    }
    else if (this->cardName == u8"           ")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card9SFX");
        //
    }
    else if (this->cardName == u8"맘마미아")
    {
        JIN_LOG("Using Card: " << this->cardName);
        engineContext.soundManager->Play("Card10SFX");

        BS->player->ModifyHealth(-1, engineContext);
        BS->player->ModifyPower(3, engineContext);
    }
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