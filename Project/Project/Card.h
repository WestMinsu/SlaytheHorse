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

    void SetHoverState(bool hover);
    bool IsHovered() const { return isHovered; }

    void SetBasePosition(const glm::vec2& pos);
    glm::vec4 GetBoundingBox() const;

private:
    std::string cardName;
    TextObject* textDisplay = nullptr;

    bool isHovered = false;
    glm::vec2 originalScale;
    glm::vec2 hoverScale;
    float originalDepth;

    glm::vec2 basePosition;     
    float hoverUpOffset = 0.0f; 
    glm::vec2 originalTextScale; 
    const float hoverMultiplier = 1.5f;
};