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

#include "Camera.h"

#include "MathUtil.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	

	idleAnim.PushBack({ 182, 314, 21,33 });

	forwardAnim.PushBack({ 273, 314, 31, 33 });
	forwardAnim.PushBack({ 368, 314, 35, 31 });
	forwardAnim.PushBack({ 463, 316, 31, 31 });
	forwardAnim.PushBack({ 561, 315, 27, 32 });
	forwardAnim.PushBack({ 663, 316, 21, 31 });
	forwardAnim.speed = 0.2f;

	backwardAnim.PushBack({ 1755, 316, 21, 34 });
	backwardAnim.PushBack({ 1653, 316, 31, 33 });
	backwardAnim.PushBack({ 1554, 316, 34, 31 });
	backwardAnim.PushBack({ 1462, 318, 32, 32 });
	backwardAnim.PushBack({ 1368, 318, 27, 32 });
	backwardAnim.PushBack({ 1272, 318, 21, 31 });
	backwardAnim.speed = 0.2f;

	forwardjump.PushBack({ 175, 446, 33, 32 });
	forwardjump.PushBack({ 270, 446, 37, 32 });
	forwardjump.PushBack({ 366, 448, 29, 30 });
	forwardjump.PushBack({ 458, 452, 36, 26 });
	forwardjump.speed = 0.2f;

	backwardjump.PushBack({ 1750, 448, 33, 31 });
	backwardjump.PushBack({ 1651, 448, 36, 31 });
	backwardjump.PushBack({ 1562, 450, 29, 29 });
	backwardjump.PushBack({ 1463, 454, 36, 25 });
	backwardjump.speed = 0.2f;

	death.PushBack({ 177, 585, 25, 31 });
	death.PushBack({ 270, 587, 32, 29 });
	death.PushBack({ 369, 587, 31, 29 });
	death.PushBack({ 465, 587, 32, 29 });
	death.PushBack({ 562, 587, 31, 29 });
	death.PushBack({ 658, 587, 31 ,29 });
	death.PushBack({ 754, 587, 31, 29 });
	death.PushBack({ 850, 587, 31, 29 });
	death.PushBack({ 948, 587, 28, 28 });
	death.speed = 0.4f;

}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	accel = parameters.attribute("accel").as_float();
	maxJumps = parameters.attribute("maxJumps").as_int();
	jumpPower = parameters.attribute("jumpPower").as_float();
	velCap.x = parameters.attribute("velCap_x").as_float();
	velCap.y = parameters.attribute("velCap_y").as_float();

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
	texture = app->tex->Load("Assets/Texture/Characte-Frames.png");

	return true;
}

bool Player::Update(float dt)
{
	// TODO detectar cuando jugador cae de plataforma (raycast?)
	currentAnimation = &idleAnim;



	//b2Vec2 vel = pbody->body->GetLinearVelocity();
	b2Vec2 impulse = b2Vec2_zero;

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && jumpsAvailable > 0) {
		impulse.y -= jumpPower;
		jumpsAvailable--;
		currentAnimation = &forwardjump;
		grounded = false;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		impulse.x -= accel;
		currentAnimation = &backwardAnim;
		
	}
	else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		impulse.x += accel;
		currentAnimation = &forwardAnim;
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

	currentAnimation->Update();

	app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());

	return true;
}

bool Player::CleanUp()
{
	if (boundCam != nullptr)
		boundCam->SetTarget(nullptr);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		OnCoinCollision(physA, physB);
		break;
	case ColliderType::PLATFORM:{
		LOG("Collision PLATFORM");
		OnPlatformCollision(physA, physB, contactInfo);
		break;
	}
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}

void Player::OnCoinCollision(PhysBody* thisBody, PhysBody* coinBody)
{
	app->audio->PlayFx(pickCoinFxId);
}

void Player::OnPlatformCollision(PhysBody* player, PhysBody* wall, b2Contact* contactInfo)
{
	


	b2Vec2 pos = pbody->body->GetPosition();
	b2Vec2 otherPos = wall->body->GetPosition();
	b2Vec2 otherHalfSize = { PIXEL_TO_METERS(wall->width), PIXEL_TO_METERS(wall->height) };

	float pRadius = PIXEL_TO_METERS(16);

	if (pos.x+(pRadius/2) >= otherPos.x - otherHalfSize.x && pos.x-(pRadius/2) <= otherPos.x + otherHalfSize.x && pos.y < otherPos.y - otherHalfSize.y) {
		LOG("Ground touched");
		grounded = true;
		jumpsAvailable = maxJumps;
	}
}

iPoint Player::GetOrigin() const
{
	iPoint ret;
	if (pbody != nullptr) {
		ret.x = pbody->width;
		ret.y = pbody->height;
	}

	return ret;
}
