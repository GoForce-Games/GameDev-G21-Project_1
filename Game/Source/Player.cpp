#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

#include "MathUtil.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetLinearDamping(1.0f);
	pbody->body->SetFixedRotation(true);
	pbody->body->SetSleepingAllowed(false);

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	//b2Vec2 vel = pbody->body->GetLinearVelocity();
	b2Vec2 impulse = b2Vec2_zero;

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && jumpsAvailable > 0) {
		impulse.y -= jumpPower;
		jumpsAvailable--;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		impulse.x -= accel;
	}
	else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		impulse.x += accel;
	}
	else {
		impulse.x = abs(impulse.x) < 0.2f ? 0 : LERP(impulse.x,0,5/dt);
	}

	//Limit de velocitat
	impulse.x = b2Clamp(impulse.x, -velCap.x, velCap.x);

	impulse.y = b2Clamp(impulse.y, -velCap.y, velCap.y);

	//Set the velocity of the pbody of the player
	//pbody->body->SetLinearVelocity(vel);
	pbody->body->ApplyLinearImpulse(impulse, pbody->body->GetPosition(), false);
	pbody->body->SetLinearVelocity(b2Clamp(pbody->body->GetLinearVelocity(), -velCap, velCap));

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PLATFORM:{
		LOG("Collision PLATFORM");
		b2Vec2 pos = pbody->body->GetPosition();
		float normal_x = 0, normal_y = 0;
		//TODO hacer que funcione la detección de suelo
		pbody->RayCast(pos.x, pos.y, pos.x, pos.y - 32, normal_x, normal_y);
		if (abs(normal_y) < 0.5f) {
			LOG("Ground touched");
			grounded = true;
			jumpsAvailable = maxJumps;
		}
		break;
	}
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}