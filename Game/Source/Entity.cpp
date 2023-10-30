#include "Entity.h"

#include "Camera.h"

bool Entity::CleanUp()
{
	boundCam->SetTarget(nullptr);

	return true;
}
