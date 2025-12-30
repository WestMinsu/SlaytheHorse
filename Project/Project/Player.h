#pragma once
#include "GameObject.h"
#include "TextObject.h"

class Player : public GameObject
{
public:
	Player(const glm::vec2& pos, const glm::vec2& size);

	void Init(const EngineContext& context) override;

	void Update(float dt, const EngineContext& context) override;

	void ModifyHealth(int amount, const EngineContext& context);

	int GetCurrHP();

	int GetPower();

	int drawCardCnt = 20;

	int maxHandCount = 7;
private:
	glm::vec2 initSize;
	glm::vec2 initPos;

	GameObject* hpBar = nullptr;
	TextObject* hpBarText = nullptr;

	int maxHP = 20;
	int currHP = 20;

	int power = 0;

	bool isDead = false;
	float outTimer = 0.f;
};