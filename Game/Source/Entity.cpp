#include "Entity.h"
#include "Camera.h"

bool Entity::CleanUp()
{
	if (pbody != nullptr) {
		pbody->setToDestroy |= destroyBody;
		pbody->boundEntity = nullptr;
		pbody = nullptr;
	}

	if (boundCam != nullptr)
		boundCam->SetTarget(nullptr);
	renderable = false;

	return true;
}
