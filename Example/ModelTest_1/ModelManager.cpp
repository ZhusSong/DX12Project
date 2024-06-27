#include "ModelManager.h"
#include "d3dUtil.h"

using namespace Assimp;
Importer mImporter;

 ModelManager::ModelManager(const std::string& path)
{
	 // 初始化调试
	 initLogger("Logs/LogFile.txt", "Logs/WarningFile.txt", "Logs/ErrorFile.txt");
	 // 去掉里面的点、线图元
	 mImporter.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	 const aiScene* localScene = mImporter.ReadFile(
		 path,
		 aiProcess_Triangulate |
		 // 转换为左手系
		 aiProcess_ConvertToLeftHanded |	
		 // This preset enables almost every optimization step to achieve perfectly optimized data. In D3D, need combine with aiProcess_ConvertToLeftHanded
		 //aiProcessPreset_TargetRealtime_MaxQuality |
		 // Triangulates all faces of all meshes、
		 // 三角化网格的所有面
		 aiProcess_GenBoundingBoxes |
		 // 改善缓存局部性
		 aiProcess_ImproveCacheLocality     
		 //// Calculates the tangents and bitangents for the imported meshes
		 //aiProcess_CalcTangentSpace |
		 //// Splits large meshes into smaller sub-meshes
		 //// This is quite useful for real-time rendering, 
		 //// where the number of triangles which can be maximally processed in a single draw - call is limited by the video driver / hardware
		 //aiProcess_SplitLargeMeshes |
		 ////// A postprocessing step to reduce the number of meshes
		 //	 aiProcess_OptimizeMeshes |
		 //aiProcess_SortByPType 
		 ////// A postprocessing step to optimize the scene hierarchy
		 //aiProcess_OptimizeGraph
	 );
	 if (localScene == nullptr || localScene->mRootNode == nullptr)
	 { 
		 std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
	 }
	 // "localScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE" is used to check whether value data returned is incomplete
	 if (localScene->mFlags && AI_SCENE_FLAGS_INCOMPLETE )
	 { 
		 std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
	 }

	 m_Directory = path.substr(0, path.find_last_of('\\'));

	 ReadNodeHierarchy(localScene->mRootNode, -1);
	 ProcessNode(localScene, localScene->mRootNode);
	 SetupRenderInfo();
	 LoadAnimations(localScene);
}

 void ModelManager::ProcessNode(const aiScene* scene, aiNode* node)
 {
	 // load mesh
	 // 读取mesh
	 for (size_t i = 0; i < node->mNumMeshes; i++)
	 {
		 aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		 m_Meshes.push_back(ProcessMesh(scene, mesh));
	 }
	 // Traverse child nodes
	 // 遍历子节点
	 for (UINT i = 0; i < node->mNumChildren; i++)
	 {
		 ProcessNode(scene, node->mChildren[i]);
	 }
 }

Mesh ModelManager::ProcessMesh(const aiScene* scene, aiMesh* mesh)
 {
	 std::vector<SkinnedVertex> localVertices;
	 std::vector<UINT> localIndices;

	 std::vector<BoneData> vertexBoneData(mesh->mNumVertices);
	 LoadBones(mesh, vertexBoneData);

	 localVertices.resize(scene->mNumMeshes);
	 // process vertex position, normal, tangent, texture coordinates
	 for (size_t i = 0; i < mesh->mNumVertices; i++)
	 {
		 SkinnedVertex vertex;

		 vertex.position.x = mesh->mVertices[i].x;
		 vertex.position.y = mesh->mVertices[i].y;
		 vertex.position.z = mesh->mVertices[i].z;

		 if (mesh->HasNormals()) 
		 {
			 vertex.normal.x = mesh->mNormals[i].x;
			 vertex.normal.y = mesh->mNormals[i].y;
			 vertex.normal.z = mesh->mNormals[i].z;
		 }
		 else
		 {
			 vertex.normal.x =0.0f;
			 vertex.normal.y = 0.0f;
			 vertex.normal.z = 0.0f;
		 }

		 if (mesh->HasTangentsAndBitangents()) {
			 vertex.tangent.x = mesh->mTangents[i].x;
			 vertex.tangent.y = mesh->mTangents[i].y;
			 vertex.tangent.z = mesh->mTangents[i].z;
		 }

		
		 // assimp allow one model have 8 different texture coordinates in one vertex, but we just care first texture coordinates because we will not use so many
		 if (mesh->mTextureCoords[0])
		 {
			 vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			 vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		 }
		 else
		 {
			 vertex.texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
		 }

		 vertex.boneWeights.x = vertexBoneData[i].Weights[0];
		 vertex.boneWeights.y = vertexBoneData[i].Weights[1];
		 vertex.boneWeights.z = vertexBoneData[i].Weights[2];

		 for (size_t j = 0; j < NUM_BONES_PER_VERTEX; j++)
			 vertex.boneIndices[j] = vertexBoneData[i].BoneIndex[j];

		 localVertices.push_back(vertex);

		 // 向调试文件中写入顶点信息
		 std::string a = "Vertex is " + std::to_string(vertex.position.x) + ' '
			 + std::to_string(vertex.position.y) + ' '
			 + std::to_string(vertex.position.z) + "\n\t\t\t\t\t"
			 + "Normal is " + std::to_string(vertex.normal.x) + ' '
			 + std::to_string(vertex.normal.y) + ' '
			 + std::to_string(vertex.normal.z) + "\n\t\t\t\t\t"
			 + "Tangent is " + std::to_string(vertex.tangent.x) + ' '
			 + std::to_string(vertex.tangent.y) + ' '
			 + std::to_string(vertex.tangent.z) + "\n\t\t\t\t\t"
			 + "Tex is " + std::to_string(vertex.texCoord.x) + ' '
			 + std::to_string(vertex.texCoord.y);
		 LOG(Info) << a;

	 }
	 
	 localIndices.resize(scene->mNumMeshes);
	 for (size_t i = 0; i < mesh->mNumFaces; i++)
	 {
		 aiFace face = mesh->mFaces[i];
		 for (size_t j = 0; j < face.mNumIndices; j++)
		 {
			 localIndices.push_back(face.mIndices[j]);

			 // 向调试文件中写入索引信息
			 std::string a = "Indices is " + std::to_string(face.mIndices[j]);
			 LOG(Info) << a;
		 }
	 }

	 std::vector<UINT> diffuseMaps;
	 if (mesh->mMaterialIndex >= 0) {
		 aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		 diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
	 }
	 UINT materialIndex = SetupMaterial(diffuseMaps);


	 return Mesh(localVertices, localIndices, materialIndex);
 }

void ModelManager::LoadBones(const aiMesh* mesh, std::vector<ModelManager::BoneData>& boneData) {
	for (size_t i = 0; i < mesh->mNumBones; i++) {
		UINT boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.C_Str());

		if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
			//insert error program
			MessageBox(NULL, L"cannot find node", NULL, NULL);
		}
		else
			boneIndex = m_BoneMapping[boneName];

		m_BoneMapping[boneName] = boneIndex;

		if (!m_BoneInfo[boneIndex].IsSkinned) {
			aiMatrix4x4 offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
			m_BoneInfo[boneIndex].BoneOffset = {
				offsetMatrix.a1, offsetMatrix.b1, offsetMatrix.c1, offsetMatrix.d1,
				offsetMatrix.a2, offsetMatrix.b2, offsetMatrix.c2, offsetMatrix.d2,
				offsetMatrix.a3, offsetMatrix.b3, offsetMatrix.c3, offsetMatrix.d3,
				offsetMatrix.a4, offsetMatrix.b4, offsetMatrix.c4, offsetMatrix.d4
			};
			m_BoneInfo[boneIndex].IsSkinned = true;
		}

		for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			UINT vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			boneData[vertexID].Add(boneIndex, weight);
		}
	}
}

void ModelManager::ReadNodeHierarchy(const aiNode* node, int parentIndex) {
	BoneInfo boneInfo;

	UINT boneIndex = this->m_BoneInfo.size();
	m_BoneMapping[node->mName.C_Str()] = boneIndex;

	DirectX::XMStoreFloat4x4(&boneInfo.BoneOffset, DirectX::XMMatrixIdentity());
	boneInfo.ParentIndex = parentIndex;
	boneInfo.DefaultOffset = {
		node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
		node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
		node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
		node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4
	};

	this->m_BoneInfo.push_back(boneInfo);

	for (size_t i = 0; i < node->mNumChildren; i++)
		ReadNodeHierarchy(node->mChildren[i], boneIndex);
}

std::vector<UINT> ModelManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type) {
	std::vector<UINT> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, 0, &str);
		bool skip = false;
		std::string texturePath = m_Directory + str.C_Str();
		for (UINT j = 0; j < this->m_TexturePath.size(); j++)
		{
			if (this->m_TexturePath[j] == texturePath) {
				textures.push_back(j);
				skip = true;
				break;
			}
		}
		if (!skip) {
			textures.push_back(this->m_TexturePath.size());
			this->m_TexturePath.push_back(texturePath);
		}
	}
	return textures;
}

void ModelManager::LoadTexturesFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) {
	std::vector<TextureData> textures;
	for (UINT i = 0; i < m_TexturePath.size(); i++)
	{
		TextureData texture;
		wchar_t* path = StringToWideChar(m_TexturePath[i]);
		ImageInfo imageInfo;
		bool res = LoadPixelWithWIC(path, GUID_WICPixelFormat32bppRGBA, imageInfo);
		if (!res) MessageBox(NULL, L"Load Texture Error", NULL, NULL);
		texture.LoadImageInfo(imageInfo);
		texture.InitBuffer(device);
		texture.SetCopyCommand(cmdList);
		textures.push_back(texture);
	}
	this->m_TextureHasLoaded.insert(m_TextureHasLoaded.end(), textures.begin(), textures.end());
}

UINT ModelManager::SetupMaterial(std::vector<UINT> diffuseMaps) {
	MaterialInfo material;
	if (diffuseMaps.size() > 0)
		material.diffuseMaps = diffuseMaps[0];
	else
		material.diffuseMaps = 0;

	UINT materialIndex;
	bool skip = false;
	for (UINT i = 0; i < m_Materials.size(); i++) {
		if (CompareMaterial(m_Materials[i], material)) {
			materialIndex = i;
			skip = true;
			break;
		}
	}
	if (!skip) {
		materialIndex = m_Materials.size();
		m_Materials.push_back(material);
	}
	return materialIndex;
}

void ModelManager::SetupRenderInfo() {
	m_RenderInfo.resize(m_Materials.size());

	for (UINT i = 0; i < m_Meshes.size(); i++)
	{
		UINT index = m_Meshes[i].mMaterialIndex;
		UINT indexOffset = m_RenderInfo[index].vertices.size();

		for (UINT j = 0; j < m_Meshes[i].mIndices.size(); j++)
			m_RenderInfo[index].indices.push_back(m_Meshes[i].mIndices[j] + indexOffset);

		m_RenderInfo[index].vertices.insert(m_RenderInfo[index].vertices.end(), m_Meshes[i].mVertices.begin(), m_Meshes[i].mVertices.end());
	}
}

void ModelManager::LoadAnimations(const aiScene* scene) {
	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		AnimationClip animation;
		std::vector<BoneAnimation> boneAnims(m_BoneInfo.size());
		aiAnimation* anim = scene->mAnimations[i];

		float ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f;
		float timeInTicks = 1.0f / ticksPerSecond;

		for (size_t j = 0; j < anim->mNumChannels; j++) {
			BoneAnimation boneAnim;
			aiNodeAnim* nodeAnim = anim->mChannels[j];

			for (size_t k = 0; k < nodeAnim->mNumPositionKeys; k++) {
				VectorKey keyframe;
				keyframe.TimePos = nodeAnim->mPositionKeys[k].mTime * timeInTicks;
				keyframe.Value.x = nodeAnim->mPositionKeys[k].mValue.x;
				keyframe.Value.y = nodeAnim->mPositionKeys[k].mValue.y;
				keyframe.Value.z = nodeAnim->mPositionKeys[k].mValue.z;
				boneAnim.mTranslation.push_back(keyframe);
			}
			for (size_t k = 0; k < nodeAnim->mNumScalingKeys; k++) {
				VectorKey keyframe;
				keyframe.TimePos = nodeAnim->mScalingKeys[k].mTime * timeInTicks;
				keyframe.Value.x = nodeAnim->mScalingKeys[k].mValue.x;
				keyframe.Value.y = nodeAnim->mScalingKeys[k].mValue.y;
				keyframe.Value.z = nodeAnim->mScalingKeys[k].mValue.z;
				boneAnim.mScale.push_back(keyframe);
			}
			for (size_t k = 0; k < nodeAnim->mNumRotationKeys; k++) {
				QuatKey keyframe;
				keyframe.TimePos = nodeAnim->mRotationKeys[k].mTime * timeInTicks;
				keyframe.Value.x = nodeAnim->mRotationKeys[k].mValue.x;
				keyframe.Value.y = nodeAnim->mRotationKeys[k].mValue.y;
				keyframe.Value.z = nodeAnim->mRotationKeys[k].mValue.z;
				keyframe.Value.w = nodeAnim->mRotationKeys[k].mValue.w;
				boneAnim.mRotationQuat.push_back(keyframe);
			}
			boneAnims[m_BoneMapping[nodeAnim->mNodeName.C_Str()]] = boneAnim;
		}
		animation.mBoneAnimations = boneAnims;

		std::string animName(anim->mName.C_Str());
		animName = animName.substr(animName.find_last_of('|') + 1, animName.length() - 1);

		m_Animations[animName] = animation;
	}
}

bool CompareMaterial(MaterialInfo dest, MaterialInfo source) {
	bool isSame = false;
	if (dest.diffuseMaps == source.diffuseMaps)
		isSame = true;
	else
		isSame = false;
	return isSame;
}

wchar_t* StringToWideChar(const std::string& str) {
	const char* pCStrKey = str.c_str();
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t* pWCStrKey = new wchar_t[pSize];
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}