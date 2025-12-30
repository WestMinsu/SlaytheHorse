#include "Player.h"

Player::Player(const glm::vec2& pos, const glm::vec2& size)
: initSize(size), initPos(pos) {}

void Player::Init(const EngineContext& context)
{
	SetMesh(context, "[EngineMesh]default");
	SetMaterial(context, "[Material]Player");

	GetTransform2D().SetPosition(initPos);
	GetTransform2D().SetScale(initSize);
	SetColor({ 1.f, 1.f, 1.f, 1.f });

	SetRenderLayer("[Layer]Player");
}
