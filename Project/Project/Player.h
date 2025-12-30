#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(const glm::vec2& pos, const glm::vec2& size);

	void Init(const EngineContext& context) override;
private:
	glm::vec2 initSize;
	glm::vec2 initPos;
};