#include "Camera.h"
#include "Player.h"

#include "App.h"
#include "Physics.h"
#include "Render.h"
#include "Window.h"

#include "MathUtil.h"
#include "Log.h"

Camera::Camera(Entity* e) : Entity(EntityType::CAMERA) {
	SetTarget(e);
}

Camera::~Camera()
{
}

bool Camera::Awake()
{
	offset.x = parameters.attribute("offset_x").as_float();
	offset.y = parameters.attribute("offset_y").as_float();
	camSpeed = parameters.attribute("speed").as_float();
	if (parameters.attribute("mainCam").as_bool()) {
		offset.x -= app->win->screenSurface->w / 2;
		offset.y -= app->win->screenSurface->h / 2;
	}
	return true;
}

bool Camera::Start()
{
	
	if (target != nullptr) {
		iPoint pos = target->position;
		rect.x = pos.x + offset.x;
		rect.y = pos.y + offset.y;
		targetOffset = target->GetOrigin();
	}
	return true;
}

bool Camera::Update(float dt)
{
	// TODO afegir restriccions de moviment a la càmara (que no surti del mapa, poder restringir moviment vertical/horitzontal, etc.)
	if (target != nullptr) {
		iPoint pos = target->position;
		//TODO potser s'ha de canviar una mica per a que el personatge quedi al centre
		position.x = rect.x = LERP(rect.x, pos.x + offset.x + targetOffset.x, camSpeed * dt);
		position.x = rect.y = LERP(rect.y, pos.y + offset.y + targetOffset.y, camSpeed * dt);
	}

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
		e->boundCam = this;
		targetOffset = e->GetOrigin();
	}
	else {
		LOG("Target is null! Camera will be stationary");
		target = nullptr;
	}
}

Entity* Camera::GetTarget() const
{
	return target;
}
