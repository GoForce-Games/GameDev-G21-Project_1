#include "NotAGoomba.h"

#include "App.h"
#include "Textures.h"
#include "Physics.h"
#include "Player.h"
#include "Log.h"

NotAGoomba::NotAGoomba() : Enemy(EntityType::ENEMY_GROUNDED)
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
	b2Vec2 movement = b2Vec2_zero;

	//TODO implementar pathfinding
	movement.x-=5;

	pbody->body->ApplyForceToCenter(movement,true);

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
	if (dynamic_cast<Player*>(physB->boundEntity)) {
		if (contactInfo->GetManifold()->localNormal.y < 0) {
			LOG("Enemy \"%s\"stomped", name);
			SetToDestroy();
		}
	}
}

iPoint NotAGoomba::GetOrigin() const
{
	return position; //TODO poner el valor correcto segun la textura usada (los valores se calculan al cargar la textura)
}

bool NotAGoomba::FindPath(iPoint& destination)
{
	return true;
}
