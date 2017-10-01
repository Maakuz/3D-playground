#include "Camera.h"
using namespace DirectX::SimpleMath;
#define MOVE_SPEED 0.015f

Camera::Camera(ID3D11Device* device, int width, int height, float drawDistance, float fieldOfView)
{
	this->mFieldOfView = fieldOfView;
	this->mDrawDistance = drawDistance;

	this->mProjection = DirectX::XMMatrixPerspectiveFovRH(fieldOfView, float(width) / height, 0.1f, drawDistance);

	values.VP = this->mProjection * this->view;
	values.V = this->view;

	yaw = 270;
	pitch = 0;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(ShaderValues);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &values;

	device->CreateBuffer(&desc, &data, &this->mVPBuffer);
}

Camera::~Camera()
{
	this->mVPBuffer->Release();
}

void Camera::setPos(DirectX::SimpleMath::Vector3 pos)
{
	this->pos = pos;
}

void Camera::setForward(DirectX::SimpleMath::Vector3 forward)
{
	this->forward = forward;
}

void Camera::setUp(DirectX::SimpleMath::Vector3 up)
{
	this->mUp = up;
}

void Camera::setRight(DirectX::SimpleMath::Vector3 right)
{
	this->right = right;
}

DirectX::SimpleMath::Vector3 Camera::getPos() const
{
	return this->pos;
}

DirectX::SimpleMath::Vector3 Camera::getForward() const
{
	return this->forward;
}

DirectX::SimpleMath::Vector3 Camera::getUp() const
{
	return this->view.Up();
}

DirectX::SimpleMath::Vector3 Camera::getRight() const
{
	return this->view.Right();
}

DirectX::SimpleMath::Matrix Camera::getView() const
{
	return view;
}

DirectX::SimpleMath::Matrix Camera::getProj() const
{
	return mProjection;
}

ID3D11Buffer* Camera::getBuffer()
{
	return this->mVPBuffer;
}


void Camera::update(ID3D11DeviceContext* context)
{
	updateKeyboard();
	updateMouse();

	view = DirectX::XMMatrixLookToRH(pos, forward, Vector3(0, 1, 0));

	values.V = this->view;
	values.VP = this->view * this->mProjection;
	values.camPos = Vector4(pos);

	this->right = view.Right();


	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(data));

	context->Map(this->mVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, &values, sizeof(ShaderValues));

	context->Unmap(this->mVPBuffer, 0);
}

void Camera::updateMouse()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
	DirectX::Mouse::State ms = DirectX::Mouse::Get().GetState();

	yaw += 0.1f * (ms.x);
	pitch -= 0.1f * (ms.y);

	// Pitch lock and yaw correction
	if (pitch > 89)
		pitch = 89;
	if (pitch < -89)
		pitch = -89;
	if (yaw < 0.f)
		yaw = 360.f;
	if (yaw > 360.f)
		yaw = 0;

	// Create forward
	forward.x = cos(DirectX::XMConvertToRadians(pitch)) * cos(DirectX::XMConvertToRadians(yaw));
	forward.y = sin(DirectX::XMConvertToRadians(pitch));
	forward.z = cos(DirectX::XMConvertToRadians(pitch)) * sin(DirectX::XMConvertToRadians(yaw));

	forward.Normalize();
}

void Camera::updateKeyboard()
{
	Vector3 accelDir;
	auto ks = DirectX::Keyboard::Get().GetState();

	if (ks.W)
	{
		Vector3 dir = Vector3(forward.x, 0, forward.z);
		dir.Normalize();
		accelDir += dir;
	}

	if (ks.S)
	{
		Vector3 dir = Vector3(forward.x, 0, forward.z);
		dir.Normalize();
		accelDir -= dir;
	}

	if (ks.A)
	{
		Vector3 dir = Vector3(forward.x, 0, forward.z).Cross({ 0, 1, 0 });
		dir.Normalize();
		accelDir -= dir;
	}

	if (ks.D)
	{
		Vector3 dir = Vector3(forward.x, 0, forward.z).Cross({ 0, 1, 0 });
		dir.Normalize();
		accelDir += dir;
	}

	accelDir.Normalize();

	accelDir *= MOVE_SPEED;

	pos += accelDir;
	forward += accelDir;

}
