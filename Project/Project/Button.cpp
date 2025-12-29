#include "Button.h"
#include "GameState.h"

void Button::Init(const EngineContext& context)
{
    SetMesh(context, "[EngineMesh]default");
    SetMaterial(context, "[Material]Animation");

    GetTransform2D().SetScale({ 100.0f, 50.0f }); 
    GetTransform2D().SetPosition({ 400.0f, 300.0f });

    auto collider = std::make_unique<AABBCollider>(this, glm::vec2(1.0f, 1.0f));
    SetCollider(std::move(collider));

    SetIgnoreCamera(true, context.stateManager->GetCurrentState()->GetActiveCamera());
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
            if (onClick) onClick();
        }
    }
    else
    {
        SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }
}