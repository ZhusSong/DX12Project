#pragma once

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H
// 模型读取相关
#include <wrl/client.h>
#include <filesystem>
#include <DirectXMath.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


#define ASSIMP_LOAD_FLAGS 0

struct ModelVertex
{

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	aiColor4D color;
	int	materialIndex;		
	//DirectX::XMFLOAT3 bitangent;
	DirectX::XMFLOAT2 texCoord;
};

struct SubSet {
	std::string meshname;				
	int materialIndex;					
	unsigned int vertexBase;			
	unsigned int vertexNum;				
	unsigned int indexBase;				
	unsigned int indexNum;				
	std::string	 mtrlname;
};

struct ModelMaterial
{
	std::string mtrlname;
	aiColor4D	Ambient;
	aiColor4D	Diffuse;
	aiColor4D	Specular;
	aiColor4D	Emission;
	float		Shiness;		
	std::string diffusetexturename;
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

	void LoadMesh(const aiScene* scene);
	void LoadMaterial(const aiScene* scene);

	std::vector<ModelMaterial> g_materials{};
	std::vector<SubSet> g_subsets{};
	std::vector<std::vector<ModelVertex>> g_vertices;
	std::vector<std::vector<uint32_t>> g_indices;
	std::vector<SubSet> GetSubsets();

	std::vector<ModelMaterial> GetMaterials();
	std::vector<std::vector<ModelVertex>>  GetVertices();

	std::vector<std::vector<uint32_t>> GetIndices();

private:
	std::string directory;
	std::vector<Mesh> m_meshs;


};
#endif