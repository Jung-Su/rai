#include "mesh_readAssimp.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct AssimpLoader {
  std::vector<rai::Mesh> meshes;
  std::string directory;

  AssimpLoader(std::string const &path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
      HALT("ERROR::ASSIMP:: " << importer.GetErrorString());
    }

    directory = path.substr(0, path.find_last_of('/'));

    arr T = eye(4);
    T(1,1) = T(2,2) = 0.;
    T(1,2) = -1;
    T(2,1) = +1.;
    loadNode(scene->mRootNode, scene, T);
  }

  void loadNode(aiNode *node, const aiScene *scene, arr T){
      arr t(4,4);
      for(uint i=0;i<4;i++) for(uint j=0;j<4;j++) t(i,j) = node->mTransformation[i][j];

      T = T*t;

      arr R = T.sub(0,2,0,2);
      arr p = T.sub(0,2,3,3).reshape(3);

//      cout <<"loading node -- transform: T=\n" <<T <<"\n p=" <<p <<" R=\n" <<R <<endl;

    for(unsigned int i = 0; i < node->mNumMeshes; i++)  {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(loadMesh(mesh, scene));
      rai::Mesh& M = meshes[meshes.size()-1];
//      arr m = M.getMean();
//      cout <<"HERE1 mean = " <<M.getCenter() <<endl;
//      for(uint i=0;i<M.V.d0;i++) M.V[i] -= m;
      M.V = M.V * ~R;
      for(uint i=0;i<M.V.d0;i++) M.V[i] += p; //+.1*m;
//      cout <<"HERE2 mean = " <<M.getCenter() <<endl;
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
      loadNode(node->mChildren[i], scene, T);
    }
  }

  rai::Mesh loadMesh(aiMesh *mesh, const aiScene *scene) {
//      cout <<"loading mesh: #V=" <<mesh->mNumVertices <<endl;
    rai::Mesh M;
    M.V.resize(mesh->mNumVertices, 3);
    M.Vn.resize(mesh->mNumVertices, 3);
    if(mesh->mTextureCoords[0]) M.tex.resize(mesh->mNumVertices, 2);

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
      M.V[i] = ARR( mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z );
      M.Vn[i] = ARR(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z );
      if(mesh->mTextureCoords[0]) M.tex[i] = ARR(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }

    M.T.resize(mesh->mNumFaces, 3);
    M.T.setZero();
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace& face = mesh->mFaces[i];
//      CHECK_EQ(face.mNumIndices, 3, "");
      if(face.mNumIndices==3)
          M.T[i] = TUP(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }
//cout <<"mean of loaded mesh=" <<M.getCenter() <<endl;

    if(mesh->mTextureCoords[0]) M.Tt = M.T;

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    for(uint i=0;i<material->mNumProperties;i++){
        aiMaterialProperty *m = material->mProperties[i];
//        cout <<  m->mKey.C_Str() <<endl;
        if(!strcmp(m->mKey.C_Str(),"$clr.diffuse")){
            float *col = (float*)m->mData;
            M.C = ARR(col[0], col[1], col[2], col[3]);
        }
    }


    uint nTex = material->GetTextureCount(aiTextureType_DIFFUSE);
//    cout <<"material: #textures=" <<nTex <<endl;
    if(nTex){
      CHECK_EQ(nTex, 1, "");
      aiString str;
      material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

//      cout <<"texture=" <<str.C_Str() <<endl;

      std::string filename = this->directory + '/' + std::string(str.C_Str());

      int width, height, nrComponents;
      unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
      if (data) {
        M.texImg.resize(height, width, nrComponents);
        memmove(M.texImg.p, data, M.texImg.N);
        M.C = {1., 1., 1.};
      } else {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
      }
      stbi_image_free(data);
    }

    return M;
  }

};

rai::Mesh mesh_readAssimp(const char* path){
    AssimpLoader A(path);
    CHECK(A.meshes.size(), "nothing loaded");
    if(A.meshes.size()==1) return A.meshes[0];

    rai::Mesh M = A.meshes[0];
    for(uint i=1;i<A.meshes.size();i++){
        M.addMesh(A.meshes[i]);
    }

    return M;
}
