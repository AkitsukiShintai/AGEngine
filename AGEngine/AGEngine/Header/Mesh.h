#pragma once
struct SubMesh {
	std::string name;
	uint32_t indexOffset;
	uint32_t indexCount;
	uint32_t vertexOffset;
	uint32_t vertexCount;
};


class Mesh {
public:
	Mesh();
	uint32_t submeshCount;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> texCoords;
	std::vector<uint32_t> indices;

	std::vector<SubMesh> submeshes;

	Buffer vertexBuffer;
	Buffer indexBuffer;

	size_t positionOffset;
	size_t normalOffset;
	size_t tangentOffset;
	size_t texCoordsOffset;

	size_t indexCount;
	size_t vertexCount;

	void LoadMesh(std::string path);
	void AddVertexNumber(uint32_t number);
	void AddIndexNumber(uint32_t number);

private:
	
};
