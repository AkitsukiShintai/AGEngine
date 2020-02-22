#include<PCH.h>
#include <Mesh.h>

void ReadMesh(Mesh* result, aiMesh* mesh, const aiScene* scene, uint32_t submeshIndex);
void ReadNode(aiNode* node, const aiScene* scene, Mesh* mesh) {
	for (int i = 0; i < node->mNumMeshes; i++)
	{	
		mesh->submeshes.push_back(SubMesh());
		ReadMesh(mesh, scene->mMeshes[node->mMeshes[i]], scene, mesh->submeshCount);
		mesh->submeshCount++;
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ReadNode(node->mChildren[i], scene, mesh);
	}


}

void ReadMesh(Mesh* result, aiMesh* mesh, const aiScene* scene,uint32_t submeshIndex) {
	
	result->submeshes[submeshIndex].name = mesh->mName.C_Str();
	size_t vertexoffset = result->vertexCount;
	size_t indexoffset = result->indexCount;

	if (submeshIndex == 0)
	{
		result->submeshes[submeshIndex].indexOffset = 0;
		result->submeshes[submeshIndex].vertexOffset = 0;		
	}
	else
	{
		result->submeshes[submeshIndex].indexOffset = result->submeshes[submeshIndex - 1].indexOffset + result->submeshes[submeshIndex - 1].indexCount;
		result->submeshes[submeshIndex].vertexOffset = result->submeshes[submeshIndex - 1].vertexOffset + result->submeshes[submeshIndex - 1].vertexCount;;
	}
	result->submeshes[submeshIndex].indexCount = mesh->mNumFaces * 3;
	result->submeshes[submeshIndex].vertexCount = mesh->mNumVertices;


	result->AddVertexNumber(mesh->mNumVertices);
	result->AddIndexNumber(mesh->mNumFaces * 3);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		//read position
		result->positions[vertexoffset +i].x = mesh->mVertices[i].x;
		result->positions[vertexoffset + i].y = mesh->mVertices[i].y;
		result->positions[vertexoffset + i].z = mesh->mVertices[i].z;
		//read normal
		result->normals[vertexoffset + i].x = mesh->mNormals[i].x;
		result->normals[vertexoffset + i].y = mesh->mNormals[i].y;
		result->normals[vertexoffset + i].z = mesh->mNormals[i].z;

		//read tangent
		result->tangents[vertexoffset + i].x = mesh->mTangents[i].x;
		result->tangents[vertexoffset + i].y = mesh->mTangents[i].y;
		result->tangents[vertexoffset + i].z = mesh->mTangents[i].z;
		//read texture coordinate
		if (mesh->mTextureCoords[0])
		{
			result->texCoords[vertexoffset + i].x = mesh->mTextureCoords[0][i].x;
			result->texCoords[vertexoffset + i].y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			result->texCoords[vertexoffset + i].x = 0;
			result->texCoords[vertexoffset + i].y = 0;
		}
	}

	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			result->indices[indexoffset + i*3 + j] =face.mIndices[j];
		}
	}
}

Mesh::Mesh() :submeshCount(0), vertexCount(0), indexCount(0), positionOffset(0), normalOffset(0), tangentOffset(0), texCoordsOffset(0) {

}

void Mesh::LoadMesh(std::string path)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	if (!scene)
	{
		std::exception(importer.GetErrorString());
	}
	aiNode* root = scene->mRootNode;

	ReadNode(root, scene, this);

	assert( (positions.size() == normals.size()) && (positions.size() == tangents.size()) &&(positions.size() == texCoords.size()));
	positionOffset = 0;
	normalOffset = sizeof(glm::vec3) * positions.size() + positionOffset;
	tangentOffset = sizeof(glm::vec3) * normals.size() + normalOffset;
	texCoordsOffset = sizeof(glm::vec3) * tangents.size() + tangentOffset;

}

void Mesh::AddVertexNumber(uint32_t number)
{
	vertexCount = vertexCount + number;
	positions.resize(number);
	normals.resize(number);
	tangents.resize(number);
	texCoords.resize(number);
}

void Mesh::AddIndexNumber(uint32_t number) {
	indexCount = indexCount + number;
	indices.resize(number);
}
