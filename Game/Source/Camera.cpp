#include "Camera.h"
#include "Player.h"

#include "App.h"
#include "Physics.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"

#include "MathUtil.h"
#include "Log.h"

Camera::Camera(Entity* e) : Entity(EntityType::CAMERA) {
	name.Create("camera");
	SetTarget(e);
}

Camera::~Camera()
{
}

bool Camera::Awake()
{
	if (awoken) {
		return true;
	}
	camSpeed = parameters.attribute("speed").as_float();

	offset.x = parameters.attribute("offset_x").as_float();
	offset.y = parameters.attribute("offset_y").as_float();
	offset.x -= app->win->screenSurface->w / 2;
	offset.y -= app->win->screenSurface->h / 2;
	
	rect.w = app->win->screenSurface->w;
	rect.h = app->win->screenSurface->h;

	return true;
}

bool Camera::Start()
{
	mapBounds = app->map->mapData.GetMapSize();
	//If created with a target, starting position is centered on said target
	if (target != nullptr) {
		iPoint pos = target->position;
		rect.x = pos.x + offset.x;
		rect.y = pos.y + offset.y;
		targetOffset = target->GetOrigin(); // This is already set on SetTarget() but target position possibly not defined on camera creation
	}
	return true;
}

bool Camera::Update(float dt)
{
	if (target != nullptr) {
		iPoint pos = target->position;
		//TODO potser s'ha de canviar una mica per a que el personatge quedi al centre
		position.x = rect.x = LERP(rect.x, pos.x + offset.x + targetOffset.x, camSpeed * dt);
		position.y = rect.y = LERP(rect.y, pos.y + offset.y + targetOffset.y, camSpeed * dt);
	}

	//Prevent from going beyond world bounds
	rect.x = position.x = b2Clamp(position.x, 0, mapBounds.x - rect.w);
	rect.y = position.y = b2Clamp(position.y, 0, mapBounds.y - rect.h);
	
	return true;
}

bool Camera::CleanUp()
{
	target = nullptr;
	return true;
}

bool Camera::DebugDraw()
{
	// NOTE potser no està del tot ben escrita aquesta línia, falta provar (implementar nova càmara com a principal primer)
	app->render->DrawCircle(rect.x + rect.w/2, rect.y + rect.h/2, 5, 255, 0, 0, 255, true);
	app->render->DrawCircle(rect.w/2, rect.h/2, 10, 255, 0, 255, 255, false);
	return true;
}

void Camera::SetTarget(Entity* e)
{
	if (e != nullptr) {
		target = e;
		e->BindCamera(this);
		targetOffset = e->GetOrigin();
	}
	else {
		LOG("Target is null! Camera will be stationary");
		target = nullptr;
	}
}

const Entity* Camera::GetTarget() const
{
	return target;
}
