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

private:
    std::string cardName;
    TextObject* textDisplay = nullptr;
};