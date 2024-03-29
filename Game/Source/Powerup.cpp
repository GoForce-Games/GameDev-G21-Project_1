#include "Powerup.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Powerup.h"

Powerup::Powerup() : Entity(EntityType::POWERUP)
{
	name.Create("powerup");
}

Powerup::~Powerup() {}

bool Powerup::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Powerup::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::POWERUP;
	pbody->listener = this;
	pbody->boundEntity = this;
	pbody->body->SetGravityScale(0.00);

	return true;
}

bool Powerup::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	if (pbody != nullptr) {
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
	}
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

void Powerup::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo)
{
	if (physB->ctype == ColliderType::PLAYER) {

		SetToDestroy(true);
	}
}

iPoint Powerup::GetOrigin() const
{
	return { 16,16 };
}

bool Powerup::CleanUp()
{
	return Entity::CleanUp();
}

bool Powerup::LoadState(pugi::xml_node& node)
{
	int x = (node.attribute("x").as_int());
	int y = (node.attribute("y").as_int());

	// Crear la posici�n adecuada y establecerla
	position.x = x;
	position.y = y;
	SetPosition(position);
	return true;
}

bool Powerup::SaveState(pugi::xml_node& node)
{
	pugi::xml_node playerNode = node;
	playerNode.append_attribute("x").set_value(position.x);
	playerNode.append_attribute("y").set_value(position.y);
	return true;
}