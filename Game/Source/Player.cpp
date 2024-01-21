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
#include "Map.h"
#include "Timer.h"

#include "Camera.h"

#include "MathUtil.h"



Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("player");
}

Player::~Player() {
}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	playerHSize.x = parameters.attribute("sizeX").as_int();
	playerHSize.y = parameters.attribute("sizeY").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	accel = parameters.attribute("accel").as_float();
	maxJumps = parameters.attribute("maxJumps").as_int();
	jumpPower = parameters.attribute("jumpPower").as_float();
	velCap.x = parameters.attribute("velCap_x").as_float();
	velCap.y = parameters.attribute("velCap_y").as_float();
	maxSlope = parameters.attribute("maxPlatformAngle").as_float();
	maxSlope = sin(maxSlope);
	alive = true;
	linearDamping = 1.0f;

	LoadAllAnimations();

	return true;
}

bool Player::Start() {

	mapBounds = app->map->mapData.GetMapSize();

	//initilize textures

	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircle(position.x + playerHSize.x, position.y + playerHSize.y, playerHSize.x, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->GetFixtureList()->SetFriction(2.0);
	pbody->body->SetLinearDamping(linearDamping);
	pbody->body->SetFixedRotation(true);
	pbody->body->SetSleepingAllowed(false);
	pbody->boundEntity = this;
	gravityScale = pbody->body->GetGravityScale();

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	/*jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");*/

	Camera* cam = app->render->cam;
	if (cam != nullptr && cam->GetTarget() == nullptr)
		cam->SetTarget(this);

	SetActive(true);

	return true;
}

bool Player::Update(float dt)
{
	// TODO detectar cuando jugador cae de plataforma (raycast?)
	currentAnimation = idleAnim;

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godMode = !godMode;
	}

	b2Vec2 impulse = b2Vec2_zero;
	if (godMode) {

		pbody->body->SetGravityScale(0);
		pbody->body->SetLinearDamping(5.0f);

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			impulse.y -= accel;
			if (grounded)
				currentAnimation = backwardAnim;
		}
		else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			impulse.y += accel;

		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			impulse.x -= accel;
			if (grounded)
				currentAnimation = backwardAnim;
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			impulse.x += accel;

		}
	}
	else if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_UP) {
		pbody->body->SetGravityScale(gravityScale);
		pbody->body->SetLinearDamping(linearDamping);
	}


	if (alive)
	{

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && jumpsAvailable > 0) {
  			impulse.y -= jumpPower;
  			jumpsAvailable--;
			currentAnimation = forwardjump;
			grounded = false;
			app->audio->PlayFx(jumpFxId);
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			impulse.x -= accel;
			if (grounded)
				currentAnimation = backwardAnim;
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			impulse.x += accel;
			if (grounded)
				currentAnimation = forwardAnim;

		}
		else {
			//impulse.x = abs(impulse.x) < 0.2f ? 0 : LERP(impulse.x,0,5/dt);
		}
	}
	if (grounded == false) {
		if (pbody->body->GetLinearVelocity().x < 0) {
			currentAnimation = backwardjump;
		}
		else if (pbody->body->GetLinearVelocity().x > 0) {
			currentAnimation = forwardjump;
		}
	}
	if(alive == false) {
		currentAnimation = death;
	}
	

	//Limit de velocitat
	impulse.x = b2Clamp(impulse.x, -velCap.x, velCap.x);
	impulse.y = b2Clamp(impulse.y, -velCap.y, velCap.y);

	//Set the velocity of the pbody of the player
	pbody->body->ApplyLinearImpulse(impulse, pbody->body->GetPosition(), false);
	pbody->body->SetLinearVelocity(b2Clamp(pbody->body->GetLinearVelocity(), -velCap, velCap));
	
	//Limit horizontal movement
	int clampedPosX;
	clampedPosX = b2Clamp(position.x, 0, mapBounds.x - playerHSize.x);
	if (clampedPosX != position.x) {
		position.x = clampedPosX;
		b2Transform pbT = pbody->body->GetTransform(); pbT.p.x = PIXEL_TO_METERS((position.x+playerHSize.x));
		pbody->body->SetTransform(pbT.p, pbT.q.GetAngle());
		
		pbody->body->SetLinearVelocity(b2Vec2(0, pbody->body->GetLinearVelocity().y));
	}

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - playerHSize.x;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - playerHSize.y;

	currentAnimation->Update();

	app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());

	return true;
}

bool Player::CleanUp()
{
	if (boundCam != nullptr)
		boundCam->SetTarget(nullptr);

	for (ListItem<Animation*>* item = animationList.start; item != nullptr; item = item->next) {
		RELEASE(item->data);
	}
	animationList.Clear();

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		OnCoinCollision(physA, physB);
		break;
	case ColliderType::HEALERITEM:
		LOG("Collision HEALERITEM");
		OnHealerCollision(physA, physB);
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

void Player::OnHurt()
{
	if (godMode) return;
	// TODO implement multiple health points (?)
	live_points--;
	if (live_points <= 0) {
		OnDeath();
	}
	
	
}

void Player::OnDeath()
{
	if (alive && !godMode) {
		alive = false;
		currentAnimation = death; death->Reset();
		if (boundCam != nullptr) {
			boundCam->SetTarget(nullptr);
			boundCam = nullptr;
		}
		app->scene->timer.Start();
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

void Player::OnCoinCollision(PhysBody* thisBody, PhysBody* coinBody)
{
	
	points = +100;
	app->audio->PlayFx(pickCoinFxId);
}
void Player::OnHealerCollision(PhysBody* thisBody, PhysBody* healerBody) {
	live_points++;
}



void Player::OnPlatformCollision(PhysBody* player, PhysBody* wall, b2Contact* contactInfo)
{
	Properties::Property* p = wall->properties.GetProperty("hurt");
	if (p != nullptr && p->boolVal) {
		OnHurt();
		return;
	}

	b2Vec2 pos = pbody->body->GetPosition();
	b2Vec2 otherPos = wall->body->GetPosition();
	b2Vec2 otherHalfSize = { PIXEL_TO_METERS(wall->width), PIXEL_TO_METERS(wall->height) };

	float pRadius = PIXEL_TO_METERS(16);

	b2Vec2 contactNormal = contactInfo->GetManifold()->localNormal;
	//Funciona solo en rectangulos alineados a los ejes del mundo
	//if (pos.x+(pRadius/2) >= otherPos.x - otherHalfSize.x && pos.x-(pRadius/2) <= otherPos.x + otherHalfSize.x && pos.y < otherPos.y - otherHalfSize.y) {
	if (contactNormal.y < 0 && abs(contactNormal.x) < maxSlope){
		LOG("Ground touched");
		grounded = true;
		jumpsAvailable = maxJumps;
	}
}

void Player::LoadAllAnimations()
{
	for (pugi::xml_node animNode = parameters.child("animations").child("animation"); animNode; animNode = animNode.next_sibling())
	{
		Animation* anim = new Animation();

		anim->name = animNode.attribute("name").as_string();
		anim->speed = animNode.attribute("speed").as_float();
		anim->loop = animNode.attribute("loop").as_bool();
		anim->pingpong = animNode.attribute("pingpong").as_bool();

		for (pugi::xml_node frameNode = animNode.child("frame"); frameNode; frameNode = frameNode.next_sibling())
		{
			int x = frameNode.attribute("x").as_int();
			int y = frameNode.attribute("y").as_int();
			int w = frameNode.attribute("w").as_int();
			int h = frameNode.attribute("h").as_int();
			anim->PushBack({ x,y,w,h });
		}
		animationList.Add(anim);
	}

	idleAnim = GetAnimation("idle");
	forwardAnim = GetAnimation("forwardAnim");
	backwardAnim = GetAnimation("backwardAnim");
	forwardjump = GetAnimation("forwardJump");
	backwardjump = GetAnimation("backwardJump");
	death = GetAnimation("death");


}

Animation* Player::GetAnimation(SString name)
{
	for (ListItem<Animation*>* item = animationList.start; item != nullptr; item = item->next)
	{
		if (item->data != nullptr) {
			if (item->data->name == name) return item->data;
		}
	}
	return nullptr;
}
bool Player::LoadState(pugi::xml_node& node)
{
	int x;
	int y;
	x = (node.attribute("x").as_int());
	y = (node.attribute("y").as_int());
	position.x = x;
	position.y = y;
	SetPosition(position);
	return true;
}

bool Player::SaveState(pugi::xml_node& node)
{
	pugi::xml_node playerNode = node;
	playerNode.append_attribute("x").set_value(position.x);
	playerNode.append_attribute("y").set_value(position.y);
	return true;
}