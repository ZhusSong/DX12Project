#include "ModelManager.h"

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
		 aiProcess_GenBoundingBoxes |
		 // Supersedes the aiProcess_MakeLeftHanded and aiProcess_FlipUVs and aiProcess_FlipWindingOrder flags
		 aiProcess_ConvertToLeftHanded |
		 // This preset enables almost every optimization step to achieve perfectly optimized data. In D3D, need combine with aiProcess_ConvertToLeftHanded
		 aiProcessPreset_TargetRealtime_MaxQuality |
		 //// Calculates the tangents and bitangents for the imported meshes
		 aiProcess_CalcTangentSpace |
		 //// Splits large meshes into smaller sub-meshes
		 //// This is quite useful for real-time rendering, 
		 //// where the number of triangles which can be maximally processed in a single draw - call is limited by the video driver / hardware
		 aiProcess_SplitLargeMeshes |
		 //// A postprocessing step to reduce the number of meshes
		 aiProcess_OptimizeMeshes |
		 aiProcess_SortByPType |
		 //// A postprocessing step to optimize the scene hierarchy
		 aiProcess_OptimizeGraph
	 );
	 if (pLocalScene == nullptr || pLocalScene->mRootNode == nullptr)
	 {
		 std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
	 }
	 // "localScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE" is used to check whether value data returned is incomplete
	 if ( pLocalScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE )
	 {
		 std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << std::endl;
	 }

	 directory = path.substr(0, path.find_last_of('/'));

	 TraverseNode(pLocalScene, pLocalScene->mRootNode);
}

 void ModelManager::TraverseNode(const aiScene* scene, aiNode* node)
 {
	 // load mesh
	 for (UINT i = 0; i < node->mNumMeshes; ++i)
	 {
		 aiMesh* pLocalMesh = scene->mMeshes[node->mMeshes[i]];
		 m_meshs.push_back(LoadMesh(scene, pLocalMesh));
	 }
	 // traverse child node
	 for (UINT i = 0; i < node->mNumChildren; ++i)
	 {
		 TraverseNode(scene, node->mChildren[i]);
	 }
 }

 ModelManager::Mesh ModelManager::LoadMesh(const aiScene* scene, aiMesh* mesh)
 {
	 std::vector<ModelVertex> localVertices;
	 std::vector<uint32_t> localIndices;

	 // process vertex position, normal, tangent, texture coordinates
	 for (UINT i = 0; i < mesh->mNumVertices; ++i)
	 {
		 ModelVertex localVertex;

		 localVertex.position.x = mesh->mVertices[i].x;
		 localVertex.position.y = mesh->mVertices[i].y;
		 localVertex.position.z = mesh->mVertices[i].z;

		 if (mesh->HasNormals()) {
			 localVertex.normal.x = mesh->mNormals[i].x;
			 localVertex.normal.y = mesh->mNormals[i].y;
			 localVertex.normal.z = mesh->mNormals[i].z;
		 }
		 else
		 {
			 localVertex.normal.x =0.0f;
			 localVertex.normal.y = 0.0f;
			 localVertex.normal.z = 0.0f;
		 }
		 if (mesh->HasTangentsAndBitangents()) {
			 localVertex.tangent.x = mesh->mTangents[i].x;
			 localVertex.tangent.y = mesh->mTangents[i].y;
			 localVertex.tangent.z = mesh->mTangents[i].z;
		 }
		
		
		/* localVertex.tangent.x = mesh->mTangents[i].x;
		 localVertex.tangent.y = mesh->mTangents[i].y;
		 localVertex.tangent.z = mesh->mTangents[i].z;*/

		 // assimp allow one model have 8 different texture coordinates in one vertex, but we just care first texture coordinates because we will not use so many
		 if (mesh->mTextureCoords[0])
		 {
			 localVertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			 localVertex.texCoord.y = mesh->mTextureCoords[0][i].y;
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
			 + "Tangent is " + std::to_string(localVertex.tangent.x) + ' '
			 + std::to_string(localVertex.tangent.y) + ' '
			 + std::to_string(localVertex.tangent.z) + "\n\t\t\t\t\t"
			 + "Tex is " + std::to_string(localVertex.texCoord.x) + ' '
			 + std::to_string(localVertex.texCoord.y);
		 LOG(Info) << a;

		 localVertices.push_back(localVertex);
	 }

	 for (UINT i = 0; i < mesh->mNumFaces; ++i)
	 {
		 aiFace localFace = mesh->mFaces[i];
		 for (UINT j = 0; j < localFace.mNumIndices; ++j)
		 {
			 localIndices.push_back(localFace.mIndices[j]);

			 std::string a = "Indices is " + std::to_string(localFace.mIndices[j]);
			 LOG(Info) << a;

		 }

	 }

	 return Mesh(localVertices, localIndices);
 }
 std::vector<ModelVertex> ModelManager::GetVertices()
 {
	 std::vector<ModelVertex> localVertices;

	 for (auto& m : m_meshs)
	 {
		 for (auto& v : m.vertices)
		 {
			 localVertices.push_back(v);
		 }
	 }

	 return localVertices;
 }

 std::vector<uint32_t> ModelManager::GetIndices()
 {
	 std::vector<uint32_t> localIndices;

	 for (auto& m : m_meshs)
	 {
		 for (auto& i : m.indices)
		 {
			 localIndices.push_back(i);
		 }
	 }

	 return localIndices;
 }
	