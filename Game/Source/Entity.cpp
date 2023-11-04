#include "Entity.h"
#include "Physics.h"

#include "Camera.h"

bool Entity::CleanUp()
{
	boundCam->SetTarget(nullptr);

	return true;
}

bool Entity::SetToDestroy()
{
	setToDestroy = true;
	if (pbody != nullptr) {
		pbody->setToDestroy = true;
		if (pbody->body != nullptr)
			pbody->body->SetActive(false);
		if (pbody->boundEntity != nullptr)
			pbody->boundEntity = nullptr;
		pbody = nullptr;
	}
	return setToDestroy;
}
