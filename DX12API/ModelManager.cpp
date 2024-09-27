#include "ModelManager.h"

using namespace Assimp;
Importer mImporter;

ModelManager::ModelManager(std::string path)
{
	initLogger("Logs/LogFile.txt", "Logs/WarningFile.txt", "Logs/ErrorFile.txt");

	const aiScene* pLocalScene = mImporter.ReadFile(
		path,
		// Triangulates all faces of all meshes、
		// 三角化网格的所有面
		aiProcess_Triangulate |
		aiProcess_GenBoundingBoxes |
		aiProcess_GenNormals|
		// Supersedes the aiProcess_MakeLeftHanded and aiProcess_FlipUVs and aiProcess_FlipWindingOrder flags
		aiProcess_ConvertToLeftHanded |
		// This preset enables almost every optimization step to achieve perfectly optimized data. In D3D, need combine with aiProcess_ConvertToLeftHanded
		aiProcessPreset_TargetRealtime_MaxQuality |
		
		//// Calculates the tangents and bitangents for the imported meshes
		//aiProcess_CalcTangentSpace 
		//// Splits large meshes into smaller sub-meshes
		//// This is quite useful for real-time rendering, 
		//// where the number of triangles which can be maximally processed in a single draw - call is limited by the video driver / hardware
		//aiProcess_SplitLargeMeshes 
		//// A postprocessing step to reduce the number of meshes
		//aiProcess_OptimizeMeshes 
		//aiProcess_SortByPType 
		// A postprocessing step to optimize the scene hierarchy
		aiProcess_OptimizeGraph
	);
	if (pLocalScene == nullptr || pLocalScene->mRootNode == nullptr)
	{
		//PrintfA("无法解析文件(%s)：%s (%d)\n", path, mImporter.GetErrorString(), ::GetLastError());
		std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
	}
	// "localScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE" is used to check whether value data returned is incomplete
	if (pLocalScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		//PrintfA("无法解析文件(%s)：%s (%d)\n", path, mImporter.GetErrorString(), ::GetLastError());
		std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
	}
	directory = path.substr(0, path.find_last_of('\\')) + '\\';
	ReadNodeHierarchy(pLocalScene->mRootNode, -1);
	ProcessNode(pLocalScene, pLocalScene->mRootNode);
	SetupRenderInfo();
	LoadAnimations(pLocalScene);
}

void ModelManager::ProcessNode(const aiScene* scene, aiNode* node)
{
	// load mesh
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* pLocalMesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(scene, pLocalMesh));
	}
	// traverse child node
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(scene, node->mChildren[i]);
	}
}
Mesh ModelManager::ProcessMesh(const aiScene* scene, aiMesh* mesh)
{
	std::vector<SkinnedVertex> localVertices;
	std::vector<UINT> localIndices;

	std::vector<BoneData> localVertexBoneData(mesh->mNumVertices);
	LoadBones(mesh, localVertexBoneData);


	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		SkinnedVertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.texCoord = { 0.0f, 0.0f };
		}
		vertex.boneWeights.x = localVertexBoneData[i].weights[0];
		vertex.boneWeights.y = localVertexBoneData[i].weights[1];
		vertex.boneWeights.z = localVertexBoneData[i].weights[2];

		for (size_t j = 0; j < NUM_BONES_PER_VERTEX; j++)
			vertex.boneIndices[j] = localVertexBoneData[i].boneIndex[j];

		localVertices.push_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			localIndices.push_back(face.mIndices[j]);
	}

	std::vector<UINT> diffuseMaps;
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
	}
	UINT materialIndex = SetupMaterial(diffuseMaps);

	return Mesh(localVertices, localIndices, materialIndex);
}

void ModelManager::LoadBones(const aiMesh* mesh, std::vector<BoneData>& boneData) {
	for (size_t i = 0; i < mesh->mNumBones; i++) {
		UINT boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.C_Str());

		if (boneMapping.find(boneName) == boneMapping.end()) {
			//insert error program
			MessageBox(NULL, L"cannot find node", NULL, NULL);
		}
		else
			boneIndex = boneMapping[boneName];

		boneMapping[boneName] = boneIndex;

		if (!boneInfo[boneIndex].isSkinned) {
			aiMatrix4x4 offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
			boneInfo[boneIndex].boneOffset = {
				offsetMatrix.a1, offsetMatrix.b1, offsetMatrix.c1, offsetMatrix.d1,
				offsetMatrix.a2, offsetMatrix.b2, offsetMatrix.c2, offsetMatrix.d2,
				offsetMatrix.a3, offsetMatrix.b3, offsetMatrix.c3, offsetMatrix.d3,
				offsetMatrix.a4, offsetMatrix.b4, offsetMatrix.c4, offsetMatrix.d4
			};
			boneInfo[boneIndex].isSkinned = true;
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

	UINT boneIndex = this->boneInfo.size();
	boneMapping[node->mName.C_Str()] = boneIndex;

	DirectX::XMStoreFloat4x4(&boneInfo.boneOffset, DirectX::XMMatrixIdentity());
	boneInfo.parentIndex = parentIndex;
	boneInfo.defaultOffset = {
		node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
		node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
		node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
		node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4
	};

	this->boneInfo.push_back(boneInfo);

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
		std::string texturePath = directory + str.C_Str();
		for (UINT j = 0; j < this->texturePath.size(); j++)
		{
			if (this->texturePath[j] == texturePath) {
				textures.push_back(j);
				skip = true;
				break;
			}
		}
		if (!skip) {
			textures.push_back(this->texturePath.size());
			this->texturePath.push_back(texturePath);
		}
	}
	return textures;
}

void ModelManager::LoadTexturesFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) {
	std::vector<TextureData > textures;
	for (UINT i = 0; i < texturePath.size(); i++)
	{
		TextureData  texture;
		wchar_t* path = StringToWideChar(texturePath[i]);
		ImageInfo imageInfo;
		bool res = LoadPixelWithWIC(path, GUID_WICPixelFormat32bppRGBA, imageInfo);
		if (!res) MessageBox(NULL, L"Load Texture Error", NULL, NULL);
		texture.LoadImageInfo(imageInfo);
		texture.InitBuffer(device);
		texture.SetCopyCommand(cmdList);
		textures.push_back(texture);
	}
	this->textureHasLoaded.insert(textureHasLoaded.end(), textures.begin(), textures.end());
}

UINT ModelManager::SetupMaterial(std::vector<UINT> diffuseMaps) {
	MaterialInfo material;
	if (diffuseMaps.size() > 0)
		material.diffuseMaps = diffuseMaps[0];
	else
		material.diffuseMaps = 0;

	UINT materialIndex;
	bool skip = false;
	for (UINT i = 0; i < materials.size(); i++) {
		if (CompareMaterial(materials[i], material)) {
			materialIndex = i;
			skip = true;
			break;
		}
	}
	if (!skip) {
		materialIndex = materials.size();
		materials.push_back(material);
	}
	return materialIndex;
}

void ModelManager::SetupRenderInfo() {
	renderInfo.resize(materials.size());

	for (UINT i = 0; i < meshes.size(); i++)
	{
		UINT index = meshes[i].materialIndex;
		UINT indexOffset = renderInfo[index].vertices.size();

		for (UINT j = 0; j < meshes[i].indices.size(); j++)
			renderInfo[index].indices.push_back(meshes[i].indices[j] + indexOffset);

		renderInfo[index].vertices.insert(renderInfo[index].vertices.end(), meshes[i].vertices.begin(), meshes[i].vertices.end());
	}
}

void ModelManager::LoadAnimations(const aiScene* scene) {
	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		AnimationClip animation;
		std::vector<BoneAnimation> boneAnims(boneInfo.size());
		aiAnimation* anim = scene->mAnimations[i];

		float ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f;
		float timeInTicks = 1.0f / ticksPerSecond;

		for (size_t j = 0; j < anim->mNumChannels; j++) {
			BoneAnimation boneAnim;
			aiNodeAnim* nodeAnim = anim->mChannels[j];

			for (size_t k = 0; k < nodeAnim->mNumPositionKeys; k++) {
				VectorKey keyframe;
				keyframe.TimePos= nodeAnim->mPositionKeys[k].mTime * timeInTicks;
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
			boneAnims[boneMapping[nodeAnim->mNodeName.C_Str()]] = boneAnim;
		}
		animation.mBoneAnimations = boneAnims;

		std::string animName(anim->mName.C_Str());
		animName = animName.substr(animName.find_last_of('|') + 1, animName.length() - 1);

		animations[animName] = animation;
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