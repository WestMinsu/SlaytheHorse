#pragma once
#include "GameObject.h"
#include "TextObject.h"
#include <functional>
#include <string>
#include "HangulAutomata.h"

class InputField : public GameObject
{
public:
	std::function<void(const std::string&, const EngineContext&)> onCommit;

	InputField(const glm::vec2& pos, const glm::vec2& size);

	void Init(const EngineContext& context) override;
	void Update(float dt, const EngineContext& context) override;

	void SetInteractable(bool interactable);
	bool IsInteractable() const { return isInteractable; }

	void SetFocus(bool focus);
private:
	void HandleTyping(const EngineContext& context);

	HangulAutomata automata;

    glm::vec2 initPos;
    glm::vec2 initSize;

	float borderThickness = 25.f;

	GameObject* innerObject = nullptr;
    TextObject* textObject = nullptr;

    bool isInteractable = true;
    bool isFocused = false; 

    float cursorTimer = 0.0f;
    bool showCursor = true;
};