#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"
#include "PugiXml/src/pugixml.hpp"
#include "Physics.h"

class PhysBody;
class b2Contact;

// TODO add new entity types here
enum class EntityType
{
	PLAYER,
	ITEM,
	CAMERA,
	ENEMY,
	ENEMY_GROUNDED,
	ENEMY_FLYING,
	UNKNOWN
};

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp();

	virtual bool LoadState(pugi::xml_node& objRootNode)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node& objRootNode)
	{
		return true;
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {}

	virtual iPoint GetOrigin() const
	{
		return iPoint(0,0);
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void SetPosition(const iPoint& newPos)
	{
		if (pbody == nullptr) {
			position.Create(newPos.x, newPos.y);
			return;
		}
		b2Transform t = pbody->body->GetTransform();
		t.p.Set(PIXEL_TO_METERS(newPos.x), PIXEL_TO_METERS(newPos.y));
		pbody->body->SetTransform(t.p, t.q.GetAngle());
	}

	void SetPBody(PhysBody* pb) {
		pbody = pb;
	}
	
	bool SetToDestroy(bool _destroyBody)
	{
		setToDestroy = true;
		destroyBody = _destroyBody;
		if (_destroyBody && pbody != nullptr)
			pbody->body->SetActive(false);
		/*if (pbody != nullptr) {
			pbody->setToDestroy = true;
			if (pbody->body != nullptr)
				pbody->body->SetActive(false);
			if (pbody->boundEntity != nullptr)
				pbody->boundEntity = nullptr;
			pbody = nullptr;
			active = false;
		}*/
		return setToDestroy;
	}

	

public:
	 
	SString name;
	EntityType type;
	bool active = true;
	// To build from entity presets
	pugi::xml_node parameters;
	// To build from tmx file parameters
	pugi::xml_node dataFromMap;

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;
	bool renderable = true;

	Camera* boundCam = nullptr; // Used to unbind camera if this entity is set to be deleted

	PhysBody* pbody = nullptr;

	bool setToDestroy = false;
	bool destroyBody = false;
};

#endif // __ENTITY_H__