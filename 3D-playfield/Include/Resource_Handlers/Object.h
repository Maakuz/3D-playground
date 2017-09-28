#pragma once
#include <d3d11.h>
#include "SimpleMath.h"
#include "../Renderer.h"

class Object
{
public:
	Object(MODEL_ID ID, DirectX::SimpleMath::Matrix matrix = DirectX::SimpleMath::Matrix());
	~Object();

	void multiply(DirectX::SimpleMath::Matrix matrix);

	void setMatrix(DirectX::SimpleMath::Matrix matrix) { info.data.transform = matrix; }
	RenderInfo * getRenderInfo() { return &info; }
	operator RenderInfo* () { return &info; };

private:
	RenderInfo info;

};
