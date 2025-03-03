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


#define ASSIMP_LOAD_FLAGS 0

struct ModelVertex
{
	ModelVertex() = default;
	ModelVertex(const ModelVertex& rhs)
	{
		this->position = rhs.position;
		this->normal = rhs.normal;
		//this->tangent = rhs.tangent;
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
	//DirectX::XMFLOAT3 tangent;
	//DirectX::XMFLOAT3 bitangent;
	DirectX::XMFLOAT2 texCoord;
};
struct ModelMaterial
{
	aiColor4D	Ambient;				
	aiColor4D	Diffuse;				
	aiColor4D	Specular;				
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

	std::vector<ModelMaterial> GetMaterials();

	std::vector< ModelVertex> GetVertices();

	std::vector<uint32_t> GetIndices();

private:
	std::string directory;
	std::vector<Mesh> m_meshs;


};
#endif