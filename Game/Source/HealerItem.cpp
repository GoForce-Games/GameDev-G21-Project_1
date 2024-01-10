#include "HealerItem.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"


HealerItem::HealerItem() : Entity(EntityType::HEALERITEM)
{
	name.Create("healeritem");
}

HealerItem::~HealerItem() {}

bool HealerItem::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool HealerItem::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ITEM;
	pbody->listener = this;
	pbody->body->SetGravityScale(0);

	return true;
}

bool HealerItem::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	if (pbody != nullptr) {
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;
	}
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

void HealerItem::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo)
{
	if (physB->ctype == ColliderType::PLAYER) {
		
		SetToDestroy(true);
	}
}

iPoint HealerItem::GetOrigin() const
{
	return { 16,16 };
}

bool HealerItem::CleanUp()
{
	return Entity::CleanUp();
}

bool HealerItem::LoadState(pugi::xml_node& node)
{
	int x = (node.attribute("x").as_int());
	int y = (node.attribute("y").as_int());

	// Crear la posición adecuada y establecerla
	position.x = x;
	position.y = y;
	SetPosition(position);
	return true;
}

bool HealerItem::SaveState(pugi::xml_node& node)
{
	pugi::xml_node playerNode = node;
	playerNode.append_attribute("x").set_value(position.x);
	playerNode.append_attribute("y").set_value(position.y);
	return true;
}