#pragma once
#include "GameObject.h"
#include <string>

class TextObject;

class Card : public GameObject
{
public:
    Card();
    ~Card() override = default;

    void Init(const EngineContext& engineContext) override;
    void Update(float dt, const EngineContext& engineContext) override;

    void SetCardName(const std::string& name);
    std::string GetCardName() const;

    void SetCardDescription(const std::string& desc);
    std::string GetCardDescription() const;

    void SetHoverState(bool hover);
    bool IsHovered() const { return isHovered; }

    void SetBasePosition(const glm::vec2& pos);
    glm::vec4 GetBoundingBox() const;

    void UseCard(const EngineContext& engineContext);

private:
    std::string cardName;
    std::string cardDescription;

    TextObject* textDisplay = nullptr;       
    TextObject* descriptionDisplay = nullptr; 
    GameObject* separatorLine = nullptr;     

    bool isHovered = false;
    glm::vec2 originalScale;
    glm::vec2 hoverScale;
    float originalDepth;

    glm::vec2 basePosition;
    float hoverUpOffset = 0.0f;
    glm::vec2 originalTextScale;
    const float hoverMultiplier = 1.5f;

    const float nameYOffset = 80.0f;
    const float lineYOffset = 50.0f;
    const float descYOffset = 45.0f;

    glm::vec2 originalLineScale; 
    glm::vec2 originalNameScale; 
    glm::vec2 originalDescScale;
};