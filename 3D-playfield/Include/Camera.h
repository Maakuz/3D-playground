#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <d3d11.h>
#include "SimpleMath.h"
#include "Keyboard.h"
#include "Mouse.h"

class Camera
{
public:
	Camera(ID3D11Device* device, int width, int height, float drawDistance = 100, float fieldOfView = M_PI * 0.45);
	~Camera();

	void setPos(DirectX::SimpleMath::Vector3 pos);
	void setForward(DirectX::SimpleMath::Vector3 forward);
	void setUp(DirectX::SimpleMath::Vector3 up);
	void setRight(DirectX::SimpleMath::Vector3 right);

	DirectX::SimpleMath::Vector3 getPos() const;
	DirectX::SimpleMath::Vector3 getForward() const;
	DirectX::SimpleMath::Vector3 getUp() const;
	DirectX::SimpleMath::Vector3 getRight() const;
	DirectX::SimpleMath::Matrix getView() const;
	DirectX::SimpleMath::Matrix getProj() const;
	ID3D11Buffer* getBuffer();

	void update(ID3D11DeviceContext* context);
private:
	struct ShaderValues
	{
		DirectX::SimpleMath::Matrix VP;
		DirectX::SimpleMath::Matrix V;
		DirectX::SimpleMath::Vector4 camPos;
	};

	DirectX::SimpleMath::Vector3 pos, forward, mUp, right;

	float yaw;
	float pitch;

	float mDrawDistance;
	float mFieldOfView;
	float mFadeStart;

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix mProjection;

		
	ShaderValues values;

	ID3D11Buffer* mVPBuffer;

	void updateMouse();
	void updateKeyboard();

};
