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
    Font* font = engineContext.renderManager->GetFontByTag("[Font]default");

    if (font == nullptr)
        return;

    auto textObj = std::make_unique<TextObject>(font, cardName);
    textDisplay = textObj.get();

    textDisplay->SetRenderLayer("[Layer]UIText");

    if (engineContext.stateManager->GetCurrentState() != nullptr)
        engineContext.stateManager->GetCurrentState()->GetObjectManager().AddObject(std::move(textObj));
}

void Card::Update(float deltaTime, const EngineContext& engineContext)
{
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