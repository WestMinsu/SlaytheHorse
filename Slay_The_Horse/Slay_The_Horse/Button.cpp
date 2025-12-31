#include "Button.h"
#include "GameState.h"

Button::Button(const glm::vec2& position, const glm::vec2& size, const std::string& text)
    :initPosition(position), initSize(size), initText(text) {}

void Button::Init(const EngineContext& context)
{
    SetMesh(context, "[EngineMesh]default");
    SetMaterial(context, "[Material]Button");

    GetTransform2D().SetScale(initSize);
    GetTransform2D().SetPosition(initPosition);

    auto collider = std::make_unique<AABBCollider>(this, glm::vec2(1.0f, 1.0f));
    SetCollider(std::move(collider));

    SetIgnoreCamera(true, context.stateManager->GetCurrentState()->GetActiveCamera());

    Font* font = context.renderManager->GetFontByTag("[Font]default");

    auto textObj = std::make_unique<TextObject>(font, initText, TextAlignH::Center, TextAlignV::Middle);

    textObj->GetTransform2D().SetPosition(initPosition);
    textObj->SetColor({0.f, 0.f, 0.f, 1.f});
    textObj->GetTransform2D().SetScale({0.8f, 0.8f});
    textObj->GetTransform2D().SetDepth(GetTransform2D().GetDepth() + 0.1f);

    if (ShouldIgnoreCamera())
    {
        textObj->SetIgnoreCamera(true, GetReferenceCamera());
    }

    GameState* currentState = context.stateManager->GetCurrentState();
    if (currentState)
    {
        labelObject = static_cast<TextObject*>(
            currentState->GetObjectManager().AddObject(std::move(textObj), GetTag() + "_Label")
            );
    }
}

void Button::Update(float dt, const EngineContext& context)
{
    InputManager* input = context.inputManager;
    Camera2D* camera = context.stateManager->GetCurrentState()->GetActiveCamera();

    glm::vec2 mousePos = input->GetMouseWorldPos(camera);

    if (GetCollider() && GetCollider()->CheckPointCollision(mousePos))
    {
        SetColor({ 0.8f, 0.8f, 0.8f, 1.0f });

        if (input->IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            JIN_LOG("Button Clicked!");
            if (onClick) onClick(context);
        }
    }
    else
    {
        SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }
}