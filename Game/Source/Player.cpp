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
#include "RayCastCallback.h"

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
		grounded = false;
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
		//impulse.x = abs(impulse.x) < 0.2f ? 0 : LERP(impulse.x,0,5/dt);
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

void Player::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::PLATFORM:{
		LOG("Collision PLATFORM");
		OnWallCollision(physA, physB, contactInfo);
		break;
	}
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}

void Player::OnWallCollision(PhysBody* player, PhysBody* wall, b2Contact* contactInfo)
{
	b2Vec2 pos = pbody->body->GetPosition();
	//TODO hacer que funcione la detección de suelo
	b2Vec2 otherPos = wall->body->GetPosition();
	b2Vec2 otherHalfSize = { PIXEL_TO_METERS(wall->width), PIXEL_TO_METERS(wall->height) };

	float pRadius = PIXEL_TO_METERS(16);

	if (pos.x+(pRadius/2) >= otherPos.x - otherHalfSize.x && pos.x+(pRadius/2) <= otherPos.x + otherHalfSize.x && pos.y < otherPos.y - otherHalfSize.y) {
		LOG("Ground touched");
		grounded = true;
		jumpsAvailable = maxJumps;
	}
}
