#include "Object.h"

Object::Object(MODEL_ID ID, DirectX::SimpleMath::Matrix matrix)
{
	info.id = ID;
	info.data.transform = matrix;
}

Object::~Object()
{
}

void Object::multiply(DirectX::SimpleMath::Matrix matrix)
{
	this->info.data.transform *= matrix;
}
