#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"

class PhysBody;
class b2Contact;

enum class EntityType
{
	PLAYER,
	ITEM,
	CAMERA,
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

	// Contact bodies may not be in the same order as parameters
	virtual void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {}

	virtual iPoint GetOrigin() const {
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

	bool SetToDestroy();

	void SetPBody(PhysBody* pb) {
		pbody = pb;
	}

public:

	SString name;
	EntityType type;
	bool active = true;
	bool setToDestroy = false;
	pugi::xml_node parameters; 

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;       
	bool renderable = true;
	
	Camera* boundCam = nullptr; // Used to unbind camera if this entity is set to be deleted

	PhysBody* pbody = nullptr;

};

#endif // __ENTITY_H__