#include "Camera.h"
#include "Player.h"

#include "App.h"
#include "Physics.h"
#include "Render.h"

#include "MathUtil.h"

Camera::Camera(Entity* e) : Entity(EntityType::CAMERA) {
	SetTarget(e);
	offset.x = parameters.attribute("offset_x").as_float();
	offset.y = parameters.attribute("offset_y").as_float();
	camSpeed = parameters.attribute("speed").as_float();
}

Camera::~Camera()
{
}

bool Camera::Awake()
{
	return true;
}

bool Camera::Start()
{
	if (target != nullptr) {
		iPoint pos = target->position;
		rect.x = pos.x + offset.x;
		rect.y = pos.y + offset.y;
	}
	return true;
}

bool Camera::Update(float dt)
{
	if (target != nullptr) {
		iPoint pos = target->position;
		rect.x = LERP(rect.x, pos.x, camSpeed * dt);
		rect.y = LERP(rect.y, pos.y, camSpeed * dt);
	}

	if (app->physics->debug)
		DebugDraw();

	return true;
}

bool Camera::CleanUp()
{
	target = nullptr;
	return true;
}

bool Camera::DebugDraw()
{
	return app->render->DrawCircle(rect.w / 2, rect.h / 2, 5, 255, 255, 255, 255, true);
}

void Camera::SetTarget(Entity* p)
{
	target = p;
}
