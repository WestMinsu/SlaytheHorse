#include "GameObject.h"
#include "EngineContext.h"
#include "InputManager.h"
#include "Collider.h"
#include "Debug.h"
#include "TextObject.h"

class Button : public GameObject
{
public:
    std::function<void(const EngineContext&)> onClick;

    Button(const glm::vec2& position, const glm::vec2& size, const std::string& text);

    void Init(const EngineContext& context) override;

    void Update(float dt, const EngineContext& context) override;

private:
    glm::vec2 initPosition;
    glm::vec2 initSize;
    std::string initText;

    TextObject* labelObject = nullptr;
};

