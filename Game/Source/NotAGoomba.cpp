#include "NotAGoomba.h"

#include "App.h"
#include "Textures.h"
#include "Physics.h"
#include "Map.h"
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
	velCap.x = parameters.attribute("velCap_x").as_float();
	velCap.y = parameters.attribute("velCap_y").as_float();
	actionRadius = parameters.attribute("actionRadius").as_float();
	homeRadius = parameters.attribute("homeRadius").as_float();
	texturePath = parameters.attribute("texturepath").as_string();
	moveDirection.Create(-1, 0);

	if (animationList.Count()==0)
		LoadAllAnimations();

	currentAnimation = GetAnimation("idle");
	state = EnemyState::IDLE;

	return true;
}

bool NotAGoomba::Start()
{
	if (texture == nullptr)
	texture = app->tex->Load(texturePath.GetString());

	if (pbody == nullptr) {
		pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
		pbody->ctype = ColliderType::ENEMY;
		pbody->listener = this;
		pbody->body->SetLinearDamping(1.0f);
		pbody->body->SetFixedRotation(true);
		pbody->body->SetSleepingAllowed(true);
	}

	SetPosition(position);

	return true;
}

bool NotAGoomba::Update(float dt)
{
	return Enemy::Update(dt);
}

bool NotAGoomba::CleanUp(bool reuse)
{
	return Enemy::CleanUp(reuse);
}

bool NotAGoomba::LoadState(pugi::xml_node& node)
{
	int x;
	int y;
	x = (node.attribute("x").as_int());
	y = (node.attribute("y").as_int());
	position.x = x;
	position.y = y;
	SetPosition(position, false);
	return true;
}

bool NotAGoomba::SaveState(pugi::xml_node& node)
{
	pugi::xml_node GoombaNode = node;
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
			LOG("Enemy \"%s\" stomped", name.GetString());
			state = EnemyState::DEAD;
			app->entityManager->CacheEntity(this);
		}
	}
}

iPoint NotAGoomba::GetOrigin() const
{
	return {16,16}; //TODO poner el valor correcto segun la textura usada (los valores se calculan al cargar la textura)
}

//Specific logic for this enemy's pathfinding
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
		movement.x = moveDirection.x * speed*dt;
		//TODO si choca contra una pared, cambia la direccion (comprobar en OnCollision)
		if (position.x < home.x - homeRadius)
			moveDirection.x = 1;
		else if (position.x > home.x + homeRadius)
			moveDirection.x = -1;

		pbody->body->ApplyLinearImpulse(movement, pbody->body->GetPosition(), true);

		if (app->entityManager->players[0]->alive && position.DistanceNoSqrt(app->entityManager->players[0]->position) < actionRadius * actionRadius) {
			if (FindPath(app->entityManager->players[0]->position) > 0)
				state = EnemyState::PURSUIT;
		}

		break;
	}
	case EnemyState::PURSUIT:
	{
		if (pathToPlayer.Count() == 0) {
			state = EnemyState::ROAMING;
		}
		else {
			//Don't bother following a dead player
			if (!app->entityManager->players[0]->alive) {
				state = EnemyState::ROAMING;
				break;
			}

			FindPath(app->entityManager->players[0]->position);

			// No path to follow
			if (pathToPlayer.Count() == 0) break;

			iPoint targetPos = pathToPlayer.start->data;
			if (targetPos == app->map->WorldToMap(position.x, position.y)) {
				pathToPlayer.Del(pathToPlayer.start);
			}
			moveDirection = (app->map->MapToWorld(targetPos.x, targetPos.y)-position);
			b2Vec2 movement = b2Vec2(moveDirection.x,0);
			movement.Normalize();
			movement.x *= speed*dt;
			pbody->body->ApplyLinearImpulse(movement, pbody->body->GetPosition(),true);

			if (moveDirection.x < 0)
				LOG("Moving left. Current Speed: %f", pbody->body->GetLinearVelocity().x);

			if (position.DistanceNoSqrt(app->entityManager->players[0]->position) > actionRadius * actionRadius * 1.25f) {
				//Lost target, return to roaming
				state = EnemyState::ROAMING;
			}
		}
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

	// Limit velocity per axis
	pbody->body->SetLinearVelocity(b2Clamp(pbody->body->GetLinearVelocity(), -velCap, velCap));

	return ret;
}
