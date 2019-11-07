#pragma once
Model::Model() {};
Model::Model(const char* path, bool transpParts) : transparent(transpParts) {
	loadModel(path);
}
Model::Model(Mesh &msh, bool transpParts) : transparent(transpParts){
	meshes.push_back(msh);
}
void Model::Draw() {
	for (GLuint t = 0; t < meshes.size(); t++) 
		meshes[t].Draw(this->shader);
}
void Model::DrawShader(Shader* shdr) {
	for (GLuint t = 0; t < meshes.size(); t++)
		meshes[t].Draw(shdr);
}
void Model::EnableCullFace(GLenum cullFaceType) {
	this->cullFaceType = cullFace;
	cullFace = true;
}
void Model::DisableCullFace() {
	cullFace = false;
}

void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene) {
		std::cout << "ASSIMPERROR: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of("/"));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node , const aiScene* scene) {
	static int numMesh = 0;
	for (GLuint t = 0; t < node->mNumMeshes; t++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[t]];
		meshes.push_back(processMesh(mesh, scene));
		numMesh++;
	}
	for (GLuint t = 0; t < node->mNumChildren; t++) {
		processNode(node->mChildren[t], scene);
	}
	//std::cout << "Childrens: " << node->mNumChildren << "\nMeshes: " << node->mNumMeshes << std::endl;
	//std::cout << numMesh << std::endl;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	for (GLuint t = 0; t < mesh->mNumVertices; t++) {
		Vertex* vert = new Vertex();
		glm::vec3 vector;
		vector.x = mesh->mVertices[t].x;
		vector.y = mesh->mVertices[t].y;
		vector.z = mesh->mVertices[t].z;
		vert->position = vector;
		vector.x = mesh->mNormals[t].x;
		vector.y = mesh->mNormals[t].y;
		vector.z = mesh->mNormals[t].z;
		vert->normal = vector;
		if (mesh->mTextureCoords[0]) {
			glm::vec2 texvec;
			texvec.x = mesh->mTextureCoords[0][t].x;
			texvec.y = mesh->mTextureCoords[0][t].y;
			vert->texCoords = texvec;
		}
		else
			vert->texCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(*vert);
	}
	for (GLuint t = 0; t < mesh->mNumFaces; t++) {
		aiFace face = mesh->mFaces[t];
		for (GLuint y = 0; y < face.mNumIndices; y++) {
			indices.push_back(face.mIndices[y]);
		}
	}


	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> reflectMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflect");										// bad aiTType 
		textures.insert(textures.end(), reflectMaps.begin(), reflectMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName) {
	std::vector<Texture> texts;
	for (GLuint t = 0; t < material->GetTextureCount(type); t++) {
		aiString str;
		material->GetTexture(type, t, &str);
		bool skip = false;
		for (GLuint y = 0; y < textures_loaded.size(); y++) {
			if (textures_loaded[y] == str.C_Str()) {
				textures_loaded.push_back(str.C_Str());
				skip = true;
				break;
			}
		}
		if (!skip) {
			std::string s = directory + "/" + str.C_Str();
			Texture tex(s.c_str(), GL_TEXTURE0, GL_RGB, typeName);
			tex.path = str;
			texts.push_back(tex);
		}
	}
	return texts;
}