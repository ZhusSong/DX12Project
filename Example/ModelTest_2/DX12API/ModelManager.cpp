#include "ModelManager.h"
#include "d3dUtil.h"

using namespace Assimp;
Importer mImporter;

ModelManager::ModelManager(const std::string& path)
{
	initLogger("Logs/LogFile.txt", "Logs/WarningFile.txt", "Logs/ErrorFile.txt");


	const aiScene* pLocalScene = mImporter.ReadFile(
		path,
		// Triangulates all faces of all meshes、
		// 三角化网格的所有面
		aiProcess_Triangulate |
		// Supersedes the aiProcess_MakeLeftHanded and aiProcess_FlipUVs and aiProcess_FlipWindingOrder flags
		aiProcess_ConvertToLeftHanded |
		// This preset enables almost every optimization step to achieve perfectly optimized data. In D3D, need combine with aiProcess_ConvertToLeftHanded
		//aiProcessPreset_TargetRealtime_MaxQuality |
		// Calculates the tangents and bitangents for the imported meshes
		aiProcess_CalcTangentSpace |
		//// Splits large meshes into smaller sub-meshes
		////This is quite useful for real-time rendering, 
		//// where the number of triangles which can be maximally processed in a single draw - call is limited by the video driver / hardware
		//aiProcess_SplitLargeMeshes |
		//// A postprocessing step to reduce the number of meshes
		//aiProcess_OptimizeMeshes |
		aiProcess_SortByPType 
		//// A postprocessing step to optimize the scene hierarchy
		//aiProcess_OptimizeGraph
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
	assert(pLocalScene != nullptr);

	LoadMaterial(pLocalScene);
	directory = path.substr(0, path.find_last_of('\\'));

	LoadMesh(pLocalScene);
}

void ModelManager::TraverseNode(const aiScene* scene, aiNode* node)
{
	// load mesh
	//for (UINT i = 0; i < node->mNumMeshes; ++i)
	//{
	//	aiMesh* pLocalMesh = scene->mMeshes[node->mMeshes[i]];
	//	m_meshs.push_back(LoadMesh(scene, pLocalMesh));
	//}
	//// traverse child node
	//for (UINT i = 0; i < node->mNumChildren; ++i)
	//{
	//	TraverseNode(scene, node->mChildren[i]);
	//}
}

void ModelManager::LoadMesh(const aiScene* scene)
{

	g_vertices.resize(scene->mNumMeshes);
	// process vertex position, normal, tangent, texture coordinates
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int vidx = 0; vidx < mesh->mNumVertices; vidx++)
		{

			ModelVertex localVertex;

			localVertex.position.x = mesh->mVertices[i].x;
			localVertex.position.y = mesh->mVertices[i].y;
			localVertex.position.z = mesh->mVertices[i].z;

			localVertex.materialIndex = mesh->mMaterialIndex;
			//PrintfW(L"Positions[%u]:\n", localVertex.position);

			if (mesh->HasNormals()) {
				localVertex.normal.x = mesh->mNormals[i].x;
				localVertex.normal.y = mesh->mNormals[i].y;
				localVertex.normal.z = mesh->mNormals[i].z;
			}
			else
			{
				localVertex.normal.x = 0.0f;
				localVertex.normal.y = 0.0f;
				localVertex.normal.z = 0.0f;
			}
			if (mesh->HasVertexColors(0))
			{
				localVertex.color = mesh->mColors[0][vidx];
			}
			else
			{
				localVertex.color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
			}

			/*	 if (mesh->HasTangentsAndBitangents()) {
					 localVertex.tangent.x = mesh->mTangents[i].x;
					 localVertex.tangent.y = mesh->mTangents[i].y;
					 localVertex.tangent.z = mesh->mTangents[i].z;
				 }*/


				 /* localVertex.tangent.x = mesh->mTangents[i].x;
				  localVertex.tangent.y = mesh->mTangents[i].y;
				  localVertex.tangent.z = mesh->mTangents[i].z;*/

				  // assimp allow one model have 8 different texture coordinates in one vertex, but we just care first texture coordinates because we will not use so many
			if (mesh->mTextureCoords[0])
			{
				localVertex.texCoord.x = mesh->mTextureCoords[0][vidx].x;
				localVertex.texCoord.y = mesh->mTextureCoords[0][vidx].y;
			}
			else
			{
				localVertex.texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
			}

			 std::string a = "Vertex is " + std::to_string(localVertex.position.x) + ' '
				 + std::to_string(localVertex.position.y) + ' '
				 + std::to_string(localVertex.position.z) + "\n\t\t\t\t\t"
				 + "Normal is " + std::to_string(localVertex.normal.x) + ' '
				 + std::to_string(localVertex.normal.y) + ' '
				 + std::to_string(localVertex.normal.z) + "\n\t\t\t\t\t"
			/*	 + "Tangent is " + std::to_string(localVertex.tangent.x) + ' '
				 + std::to_string(localVertex.tangent.y) + ' '
				 + std::to_string(localVertex.tangent.z) + "\n\t\t\t\t\t"*/
				 + "Tex is " + std::to_string(localVertex.texCoord.x) + ' '
				 + std::to_string(localVertex.texCoord.y);
			 LOG(Info) << a;
			g_vertices[i].push_back(localVertex);
		}
	}

		g_indices.resize(scene->mNumMeshes);
		for (unsigned int m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];

			// ���b�V�����擾
			std::string meshname = std::string(mesh->mName.C_Str());
			for (UINT i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace localFace = mesh->mFaces[i];
				assert(localFace.mNumIndices == 3);
				for (UINT j = 0; j < localFace.mNumIndices; j++)
				{
					g_indices[m].push_back(localFace.mIndices[j]);

					std::string a = "Indices is " + std::to_string(localFace.mIndices[j]);
					LOG(Info) << a;

				}
			}
		}


	g_subsets.resize(scene->mNumMeshes);
	for (unsigned int m = 0; m < g_subsets.size(); m++)
	{
		g_subsets[m].indexNum = g_indices.size();
		g_subsets[m].vertexNum = g_vertices.size();
		g_subsets[m].vertexBase = 0;
		g_subsets[m].indexBase = 0;
		g_subsets[m].materialIndex = g_vertices[m][0].materialIndex;
	}

	//return Mesh(localVertices, localIndices);
	for (int m = 0; m < g_subsets.size(); m++)
	{
		// ���_�o�b�t�@�̃x�[�X���v�Z
		g_subsets[m].vertexBase = 0;
		for (int i = m - 1; i >= 0; i--) {
			g_subsets[m].vertexBase += g_subsets[i].vertexNum;
		}

		// �C���f�b�N�X�o�b�t�@�̃x�[�X���v�Z
		g_subsets[m].indexBase = 0;
		for (int i = m - 1; i >= 0; i--) {
			g_subsets[m].indexBase += g_subsets[i].indexNum;
		}
	}

}


void ModelManager::LoadMaterial(const aiScene* pScene)
{
	for (unsigned int m = 0; m < pScene->mNumMaterials; m++)
	{
		aiMaterial* material = pScene->mMaterials[m];

		// �}�e���A�����擾
		std::string mtrlname = std::string(material->GetName().C_Str());
		std::cout << mtrlname << std::endl;

		aiColor4D ambient;
		aiColor4D diffuse;
		aiColor4D specular;
		aiColor4D emission;
		float shiness;

		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
		}
		else {
			ambient = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
		}
		else {
			diffuse = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
		}

		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular)) {
		}
		else {
			specular = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emission)) {
		}
		else {
			emission = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		}

		if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shiness)) {
		}
		else {
			shiness = 0.0f;
		}

		std::vector<std::string> texpaths{};
		for (unsigned int t = 0; t < material->GetTextureCount(aiTextureType_DIFFUSE); t++)
		{
			aiString path;
			if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, t, &path))
			{
				std::string texpath = std::string(path.C_Str());
				std::cout << texpath << std::endl;
				texpaths.push_back(texpath);
			}
		}

		ModelMaterial mtrl{};
		mtrl.mtrlname = mtrlname;
		mtrl.Ambient = ambient;
		mtrl.Diffuse = diffuse;
		mtrl.Specular = specular;
		mtrl.Emission = emission;
		mtrl.Shiness = shiness;

		std::string a = "Ambient  is " + std::to_string(ambient.r) + ' '
			+ std::to_string(ambient.g) + ' '
			+ std::to_string(ambient.b) + "\n\t\t\t\t\t"
			+ "Diffuse is " + std::to_string(diffuse.r) + ' '
			+ std::to_string(diffuse.g) + ' '
			+ std::to_string(diffuse.b) + "\n\t\t\t\t\t"
			/*	 + "Tangent is " + std::to_string(localVertex.tangent.x) + ' '
				 + std::to_string(localVertex.tangent.y) + ' '
				 + std::to_string(localVertex.tangent.z) + "\n\t\t\t\t\t"*/
			+"Specular is " + std::to_string(specular.r) + ' '
			+ std::to_string(specular.g) + std::to_string(diffuse.b);
		LOG(Info) << a;

		if (texpaths.size() == 0)
			mtrl.diffusetexturename = "";
		else
			mtrl.diffusetexturename = texpaths[0];

		g_materials.push_back(mtrl);
	}
}
std::vector<SubSet> ModelManager::GetSubsets()
{
	return g_subsets;
}
std::vector<ModelMaterial> ModelManager::GetMaterials()
{
	return g_materials;
}
std::vector<std::vector<ModelVertex>>  ModelManager::GetVertices()
{
	return g_vertices;
}

std::vector <std::vector<uint32_t>> ModelManager::GetIndices()
{
	return g_indices;
}
