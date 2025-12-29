#include "GameObject.h"
#include "EngineContext.h"
#include "InputManager.h"
#include "Collider.h"
#include "Debug.h"

class Button : public GameObject
{
public:
    std::function<void()> onClick;

    Button() : GameObject() {}

    void Init(const EngineContext& context) override;

    void Update(float dt, const EngineContext& context) override;
};

