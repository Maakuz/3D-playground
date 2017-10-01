#include "ObjLoader.h"
#include "WICTextureLoader.h"
#include <fstream>
#include <sstream>
#include <vector>

using namespace DirectX::SimpleMath;

ObjLoader::ObjLoader(ID3D11Device * device)
{
	models[CUBE] = loadObject(device, OBJECT_PATH("betterCube.obj"), true);
	models[BARREL] = loadObject(device, OBJECT_PATH("Barrel.obj"), true);
}

ObjLoader::~ObjLoader()
{
	SAFE_RELEASE(models[CUBE].vertexBuffer);
	SAFE_RELEASE(models[CUBE].instanceBuffer);
	SAFE_RELEASE(models[CUBE].diffuseTexture);
	SAFE_RELEASE(models[CUBE].normalTexture);
	SAFE_RELEASE(models[CUBE].specularTexture);

	SAFE_RELEASE(models[BARREL].vertexBuffer);
	SAFE_RELEASE(models[BARREL].instanceBuffer);
	SAFE_RELEASE(models[BARREL].diffuseTexture);
	SAFE_RELEASE(models[BARREL].normalTexture);
	SAFE_RELEASE(models[BARREL].specularTexture);

}

ModelInfo * ObjLoader::getModelInfo(MODEL_ID id)
{
	return &models[id];
}

ModelInfo ObjLoader::loadObject(ID3D11Device * device, char * path, bool objIsLeft)
{
	ModelInfo info = {};
	std::stringstream sStream;
	std::string line;
	std::string testString;
	std::string materialFile;

	std::vector<Vector3> pos;
	std::vector<Vector3> normal;
	std::vector<Vector2> uv;
	std::vector<Vertex> vertices;
	std::fstream file(path);
	if (!file.is_open())
		throw "File not open";

	while (std::getline(file, line))
	{
		sStream = std::stringstream(line);
		sStream >> testString;

		if (testString.substr(0, 2) == "vn")
		{
			Vector3 temp;
			sStream >> temp.x >> temp.y >> temp.z;
			normal.push_back(temp);
		}

		else if (testString.substr(0, 2) == "vt")
		{
			Vector2 temp;
			sStream >> temp.x >> temp.y;
			uv.push_back(temp);
		}

		else if (testString.substr(0, 1) == "v")
		{
			Vector3 temp;
			sStream >> temp.x >> temp.y >> temp.z;
			pos.push_back(temp);
		}

		else if (testString.substr(0, 6) == "mtllib")
		{
			sStream >> materialFile;
		}
	
		else if (testString.substr(0, 1) == "f")
		{
			for (size_t i = 0; i < 3; i++)
			{
				std::vector<int> nrs;
				std::string temp;
				sStream >> temp;
				std::stringstream smallPiece(temp);

				while (std::getline(smallPiece, temp, '/'))
				{
					nrs.push_back(std::stoi(temp) - 1);
				}

				Vertex vertex;
				vertex.pos = pos[nrs[0]];
				vertex.uv = uv[nrs[1]];
				vertex.normal = normal[nrs[2]];

				if (objIsLeft) 
				{
					vertex.pos.x *= -1;
					vertex.normal.x *= -1;
				}

				vertices.push_back(vertex);

			}

		}
	}

	info.vertexCount = (int)vertices.size();

	file.close();

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = &vertices[0];

	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = sizeof(Vertex) * (UINT)vertices.size();
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	device->CreateBuffer(&desc, &subData, &info.vertexBuffer);

	desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(Vertex) * MAX_INSTANCES;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&desc, NULL, &info.instanceBuffer);


	//MATERIAL
	file.open(MATERIAL_PATH("" + materialFile));

	if (!file.is_open())
		throw "File not open";

	std::wstring normalMapName(TEXTURE_PATH(""));
	std::wstring diffuseMapName(TEXTURE_PATH(""));
	std::wstring specularMapName(TEXTURE_PATH(""));

	while (std::getline(file, line))
	{
		sStream = std::stringstream(line);
		sStream >> testString;

		if (testString.substr(0, 2) == "Ns")
		{
			sStream >> info.matInfo.specularity.w;
		}

		if (testString.substr(0, 2) == "Ka")
		{
			sStream >> info.matInfo.ambient.x >> info.matInfo.ambient.y >> info.matInfo.ambient.z;
		}

		if (testString.substr(0, 2) == "Kd")
		{
			sStream >> info.matInfo.diffuse.x >> info.matInfo.diffuse.y >> info.matInfo.diffuse.z;
		}

		if (testString.substr(0, 2) == "Ks")
		{
			sStream >> info.matInfo.specularity.x >> info.matInfo.specularity.y >> info.matInfo.specularity.z;
		}

		if (testString.substr(0, 6) == "map_Kd")
		{
			std::string temp = "";
			sStream >> temp;
			std::wstring temp2(temp.begin(), temp.end());
			diffuseMapName += temp2;
		}

		if (testString.substr(0, 6) == "map_Ns")
		{
			std::string temp = "";
			sStream >> temp;
			std::wstring temp2(temp.begin(), temp.end());
			specularMapName += temp2;
		}

		if (testString.substr(0, 8) == "map_Bump")
		{
			std::string temp = "";
			sStream >> temp;
			std::wstring temp2(temp.begin(), temp.end());
			normalMapName += temp2;
		}
	}

	file.close();

	ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, diffuseMapName.c_str(), nullptr, &info.diffuseTexture));
	ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, normalMapName.c_str(), nullptr, &info.normalTexture));
	ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, specularMapName.c_str(), nullptr, &info.specularTexture));
	
	
	
	


	return info;
}
