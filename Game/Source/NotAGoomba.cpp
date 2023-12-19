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
	speed = parameters.attribute("speed").as_float();
	actionRadius = parameters.attribute("actionRadius").as_float();
	homeRadius = parameters.attribute("homeRadius").as_float();
	texturePath = parameters.attribute("texturepath").as_string();
	moveDirection.Create(-1, 0);

	LoadAllAnimations();

	currentAnimation = GetAnimation("Idle");

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
	pbody->body->SetSleepingAllowed(true);

	home.x = position.x;
	home.x = position.y;

	return true;
}

bool NotAGoomba::Update(float dt)
{
	return Enemy::Update(dt);
}

bool NotAGoomba::CleanUp()
{
	return Enemy::CleanUp();
}

bool NotAGoomba::LoadState(pugi::xml_node& node)
{
	position.x = node.child("notGoomba").attribute("x").as_int();
	position.y = node.child("notGoomba").attribute("y").as_int();

	return true;
}

bool NotAGoomba::SaveState(pugi::xml_node& node)
{
	pugi::xml_node GoombaNode = node.append_child("notGoomba");
	GoombaNode.append_attribute("x").set_value(position.x);
	GoombaNode.append_attribute("y").set_value(position.y);

	return true;
}

void NotAGoomba::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo)
{
	if (physB->ctype == ColliderType::PLAYER) {
		int x, y;
		physA->GetPosition(x, y);
		fPoint posA(x, y);
		physB->GetPosition(x, y);
		fPoint posB(x, y);
		fPoint d = (posB-posA);
		d = d / sqrtf(d.x * d.x + d.y * d.y);
		if (d.y < 0.0f && abs(d.x) < 0.5f) {
			LOG("Enemy \"%s\"stomped", name);
			SetToDestroy(true);
		}
	}
}

iPoint NotAGoomba::GetOrigin() const
{
	return position; //TODO poner el valor correcto segun la textura usada (los valores se calculan al cargar la textura)
}

bool NotAGoomba::FindPath(iPoint& destination)
{
	return Enemy::FindPath(destination);
}

bool NotAGoomba::EnemyBehaviour(float dt)
{
	bool ret = true;

	switch (state)
	{
	case EnemyState::IDLE:
	{
		state = EnemyState::ROAMING;
		break;
	}
	case EnemyState::ROAMING:
	{
		b2Vec2 movement = b2Vec2_zero;
		movement.x = moveDirection.x * speed;
		//TODO si choca contra una pared, cambia la direccion (comprobar en OnCollision)
		if (position.x < home.x - homeRadius)
			moveDirection.x = 1;
		else if (position.x > home.x + homeRadius)
			moveDirection.x = -1;

		pbody->body->ApplyForceToCenter(movement,true);

		break;
	}
	case EnemyState::PURSUIT:
	{
		break;
	}
	case EnemyState::FLEEING:
	{
		break;
	}
	case EnemyState::DEAD:
	{
		break;
	}
	default:
		break;
	}

	return ret;
}
