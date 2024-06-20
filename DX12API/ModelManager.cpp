
#include "ModelManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace Assimp;
Importer mImporter;

void ModelManager::LoadModels()
{
    // 读取x文件  
    CHAR pXFile[MAX_PATH] = {};
    const aiScene* pModel = mImporter.ReadFile(pXFile, ASSIMP_LOAD_FLAGS);
    if (pModel == nullptr)
    {
        

    }
}