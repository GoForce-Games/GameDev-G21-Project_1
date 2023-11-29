#include "NotAGoomba.h"

#include "App.h"
#include "Textures.h"
#include "Physics.h"

NotAGoomba::NotAGoomba(EntityType type) : Entity(type)
{
	name.Create("notGoomba");
}

NotAGoomba::~NotAGoomba()
{
}

bool NotAGoomba::Awake()
{
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool NotAGoomba::Start()
{
	texture = app->tex->Load(texturePath.GetString());

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetLinearDamping(1.0f);
	pbody->body->SetFixedRotation(true);
	pbody->body->SetSleepingAllowed(false);



	return true;
}

bool NotAGoomba::Update(float dt)
{
	return true;
}

bool NotAGoomba::CleanUp()
{
	return true;
}

bool NotAGoomba::LoadState(pugi::xml_node& objRootNode)
{
	return true;
}

bool NotAGoomba::SaveState(pugi::xml_node& objRootNode)
{
	return true;
}

void NotAGoomba::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo)
{
}

iPoint NotAGoomba::GetOrigin() const
{
	return position; //TODO poner el valor correcto segun la textura usada (los valores se calculan al cargar la textura)
}
