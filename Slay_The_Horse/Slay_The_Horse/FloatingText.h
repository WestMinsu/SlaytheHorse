#pragma once
#include "TextObject.h"

class FloatingText : public TextObject
{
public:
    FloatingText(Font* font, const std::string& text, const glm::vec2& startPos, const glm::vec4& color)
        : TextObject(font, text, TextAlignH::Center, TextAlignV::Middle)
    {
        GetTransform2D().SetPosition(startPos);
        SetColor(color);
        SetRenderLayer("[Layer]UIText");
        GetTransform2D().SetDepth(900.0f);
    }

    void Update(float dt, const EngineContext& context) override
    {
        float moveSpeed = 50.0f;
        GetTransform2D().AddPosition({ 0.0f, moveSpeed * dt });

        float fadeSpeed = 1.0f;
        glm::vec4 color = GetColor();
        color.a -= fadeSpeed * dt;
        SetColor(color);

        if (color.a <= 0.0f)
        {
            Kill();
        }
    }
};