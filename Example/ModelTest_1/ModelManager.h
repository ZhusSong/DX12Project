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
#include <assimp/cimport.h>
#include <assimp/mesh.h>

#include "SkinnedData.h"
#include "TextureData.h"
#include "d3dUtil.h"

#define ASSIMP_LOAD_FLAGS 0

// 材质信息
struct MaterialInfo {
	UINT diffuseMaps;
};
// 渲染信息
struct RenderInfo {
	std::vector<SkinnedVertex> vertices;
	std::vector<UINT> indices;
};
//struct ModelVertex
//{
//	ModelVertex() = default;
//	ModelVertex(const ModelVertex& rhs)
//	{
//		this->position = rhs.position;
//		this->normal = rhs.normal;
//		//this->tangent = rhs.tangent;
//		this->texCoord = rhs.texCoord;
//		//this->bitangent = rhs.bitangent;
//		
//	}
//	ModelVertex& operator= (ModelVertex& rhs)
//	{
//		return rhs;
//	}
//
//	ModelVertex(ModelVertex&& rhs) = default;
//
//	DirectX::XMFLOAT3 position;
//	DirectX::XMFLOAT3 normal;
//	//DirectX::XMFLOAT3 tangent;
//	//DirectX::XMFLOAT3 bitangent;
//	DirectX::XMFLOAT2 texCoord;
//};
//struct ModelMaterial
//{
//	aiColor4D	Ambient;				
//	aiColor4D	Diffuse;				
//	aiColor4D	Specular;				
//};

class Mesh
{
public:
	std::vector<SkinnedVertex> mVertices;
	std::vector<UINT> mIndices;
	UINT mMaterialIndex;

	Mesh(std::vector<SkinnedVertex> vertices, std::vector<UINT> indices, UINT materialIndex) {
		this->mVertices = vertices;
		this->mIndices = indices;
		this->mMaterialIndex = materialIndex;
	}
};

bool CompareMaterial(MaterialInfo dest, MaterialInfo source);
wchar_t* StringToWideChar(const std::string& str);

class ModelManager
{
public:
	struct BoneData 
	{
		UINT BoneIndex[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];
		void Add(UINT boneID, float weight) {
			for (size_t i = 0; i < NUM_BONES_PER_VERTEX; i++) {
				if (Weights[i] == 0.0f) {
					BoneIndex[i] = boneID;
					Weights[i] = weight;
					return;
				}
			}
			//insert error program
			MessageBox(NULL, L"Bone index out of size", L"Error", NULL);
		}
	};
	struct BoneInfo 
	{
		bool IsSkinned = false;
		DirectX::XMFLOAT4X4 BoneOffset;
		DirectX::XMFLOAT4X4 DefaultOffset;
		int ParentIndex;
	};
	
	ModelManager(const std::string& path);

	//void TraverseNode(const aiScene* scene, aiNode* node);

	//Mesh LoadMesh(const aiScene* scene, aiMesh* mesh);
	//
	//void GetMaterialData(const aiScene* scene);

	std::vector<Mesh> m_Meshes;
	std::vector<MaterialInfo> m_Materials;
	std::vector<RenderInfo> m_RenderInfo;
	std::vector<TextureData> m_TextureHasLoaded;

	void GetBoneMapping(std::unordered_map<std::string, UINT>& boneMapping) {

		boneMapping = this->m_BoneMapping;
	}
	void GetBoneOffsets(std::vector<DirectX::XMFLOAT4X4>& boneOffsets) 
	{
		for (size_t i = 0; i < m_BoneInfo.size(); i++)
			boneOffsets.push_back(m_BoneInfo[i].BoneOffset);
	}
	void GetNodeOffsets(std::vector<DirectX::XMFLOAT4X4>& nodeOffsets) 
	{
		for (size_t i = 0; i < m_BoneInfo.size(); i++)
			nodeOffsets.push_back(m_BoneInfo[i].DefaultOffset);
	}
	void GetBoneHierarchy(std::vector<int>& boneHierarchy) 
	{
		for (size_t i = 0; i < m_BoneInfo.size(); i++)
			boneHierarchy.push_back(m_BoneInfo[i].ParentIndex);
	}
	void GetAnimations(std::unordered_map<std::string, AnimationClip>& animations)
	{
		animations.insert(this->m_Animations.begin(), this->m_Animations.end());
	}
	void LoadTexturesFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
	std::string m_Directory;
	std::vector<std::string> m_TexturePath;

	void ProcessNode(const aiScene* scene, aiNode* node);
	Mesh ProcessMesh(const aiScene* scene, aiMesh* mesh);
	UINT SetupMaterial(std::vector<UINT> diffuseMaps);
	void SetupRenderInfo();
	std::vector<UINT> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

	//Bone/Animation Information
	std::vector<BoneInfo> m_BoneInfo;
	std::unordered_map<std::string, UINT> m_BoneMapping;
	std::unordered_map<std::string, AnimationClip> m_Animations;

	void LoadBones(const aiMesh* mesh, std::vector<BoneData>& bones);
	void ReadNodeHierarchy(const aiNode* node, int parentIndex);
	void LoadAnimations(const aiScene* scene);
};

#endif