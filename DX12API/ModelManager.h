#pragma once

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H
// 模型读取相关
#include <d3d11_1.h>
#include <wrl/client.h>
#include <filesystem>
#include <DirectXMath.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "assimp/mesh.h"
#include "assimp/texture.h"


#define ASSIMP_LOAD_FLAGS 0

struct ModelVertex
{
	ModelVertex() = default;
	ModelVertex(const ModelVertex& rhs)
	{
		this->position = rhs.position;
		this->normal = rhs.normal;
		aiColor4D color=rhs.color;
		this->tangent = rhs.tangent;
		this->texCoord = rhs.texCoord;
		//this->bitangent = rhs.bitangent;

	}
	ModelVertex& operator= (ModelVertex& rhs)
	{
		return rhs;
	}

	ModelVertex(ModelVertex&& rhs) = default;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	aiColor4D color;
	DirectX::XMFLOAT3 tangent;
	//DirectX::XMFLOAT3 bitangent;
	DirectX::XMFLOAT2 texCoord;
};
struct ModelMaterial
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
};
class ModelManager
{
public:
	struct Mesh
	{
		Mesh() = default;
		std::vector<ModelVertex> vertices;
		std::vector<uint32_t> indices;

		//Material mats;
		Mesh(std::vector<ModelVertex>& vertices, std::vector<UINT>& indices)
		{
			this->vertices = vertices;
			this->indices = indices;
		}
	};

	ModelManager(const std::string& path);

	void TraverseNode(const aiScene* scene, aiNode* node);

	Mesh LoadMesh(const aiScene* scene, aiMesh* mesh);
	ModelMaterial LoadMaterial(const aiScene* scene, aiMesh* mesh);

	std::vector<ModelMaterial> GetMaterials()
	{
		return m_materials;
	};

	std::vector< ModelVertex> GetVertices();

	std::vector<uint32_t> GetIndices();

private:
	std::string directory;
	std::vector<Mesh> m_meshs;
	std::vector<ModelMaterial> m_materials;


};
#endif