#include<Models/Model.h>

struct Texture {
    unsigned int id;
    std::string path;  // we store the path of the texture to compare with other textures
};

std::vector<Texture> loadedTextures;



void Model::draw(ShaderProgram& shader, glm::mat4 model, bool setMat) {
    shader.use();
    shader.setMat4("model", model);
    Material activeMat;
	for (unsigned int i = 0; i < meshes.size(); i++) {
        if (setMat && activeMat.name != materials[matIndices[i]].name) {
            activeMat = materials[matIndices[i]];
            shader.setVec3("AmbientColor",activeMat.AmbientColor);
            shader.setVec3("DiffuseColor", activeMat.DiffuseColor);
            shader.setVec3("SpecularColor", activeMat.SpecularColor);
            shader.setFloat("SpecularExponent", activeMat.SpecularExponent);
            shader.setFloat("OpticalDensity", activeMat.OpticalDensity);
            shader.setFloat("Dissolve", activeMat.Dissolve);
            shader.setFloat("Illumination", activeMat.Illumination);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, activeMat.AmbientTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, activeMat.DiffuseTexture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, activeMat.SpecularTexture);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, activeMat.SpecularHightlightTexture);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, activeMat.AlphaTexture);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, activeMat.BumpTexture);

        }

        
		meshes[i].draw();
	}
}
void Model::loadModel(std::string path) {
    std::string directory = path.substr(0, path.find_last_of("/")+1);
    std::cout << directory << std::endl;
	objl::Loader loader;
	if (loader.LoadFile(path)) {
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
            //set shape
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			MeshRenderer newRenderer;
			newRenderer.fillBuffers(curMesh.Vertices, curMesh.Indices);
			std::cout << "Mesh added  vertices Amt:" << curMesh.Vertices.size() << " Indices Amt:" << curMesh.Indices.size() << std::endl;
			meshes.push_back(newRenderer);

            //set material
            //only add new material if not already created
            //keep track of which material is assigned to a mesh in matIndices
            objl::Material curMat = curMesh.MeshMaterial;
            bool alreadyExist = false;
            unsigned int matIndex;
            for (size_t j = 0; j < materials.size(); j++)
            {
                if (materials[j].name == curMat.name) {
                    alreadyExist = true;
                    matIndex = j;
                    std::cout << "Old Material: " << materials[j].name << "\n\n";
                }
            }
            if (!alreadyExist) {
                matIndex = materials.size();
                if (curMat.name == "") {
                    objl::Vector3 defaultCol = objl::Vector3(1.f, 1.f, 1.f);
                    curMat.Ka = defaultCol;
                    curMat.Kd = defaultCol;
                    curMat.Ks = defaultCol;
                }

                Material newMat;
                newMat.name = curMat.name;
                newMat.AmbientColor = glm::vec3(curMat.Ka.X, curMat.Ka.Y, curMat.Ka.Z);
                newMat.DiffuseColor = glm::vec3(curMat.Kd.X, curMat.Kd.Y, curMat.Kd.Z);
                newMat.SpecularColor = glm::vec3(curMat.Ks.X, curMat.Ks.Y, curMat.Ks.Z);
                newMat.SpecularExponent = curMat.Ns;
                newMat.OpticalDensity = curMat.Ni;
                newMat.Dissolve = curMat.d;
                newMat.Illumination = curMat.illum;
                newMat.AmbientTexture = loadTexture((directory + curMat.map_Ka).c_str());
                if (newMat.AmbientTexture == 0)
                    newMat.AmbientTexture = defaultTexture;
                newMat.DiffuseTexture = loadTexture((directory + curMat.map_Kd).c_str());
                if (newMat.DiffuseTexture == 0)
                    newMat.DiffuseTexture = defaultTexture;
                newMat.SpecularTexture = loadTexture((directory + curMat.map_Ks).c_str());
                if (newMat.SpecularTexture == 0)
                    newMat.SpecularTexture = defaultTexture;
                newMat.SpecularHightlightTexture = loadTexture((directory + curMat.map_Ns).c_str());
                if (newMat.SpecularHightlightTexture == 0)
                    newMat.SpecularHightlightTexture = defaultTexture;
                newMat.AlphaTexture = loadTexture((directory + curMat.map_d).c_str());
                if (newMat.AlphaTexture == 0)
                    newMat.AlphaTexture = defaultTexture;
                newMat.BumpTexture = loadTexture((directory + curMat.map_bump).c_str());
                if (newMat.BumpTexture == 0)
                    newMat.BumpTexture = defaultTexture;


                materials.push_back(newMat);
                std::cout << "New Material at Index: " << matIndex << "\n";
                std::cout << "Name: " << curMat.name << "\n";
                std::cout << "Ambient Color: " << curMat.Ka.X << ", " << curMat.Ka.Y << ", " << curMat.Ka.Z << "\n";
                std::cout << "Diffuse Color: " << curMat.Kd.X << ", " << curMat.Kd.Y << ", " << curMat.Kd.Z << "\n";
                std::cout << "Specular Color: " << curMat.Ks.X << ", " << curMat.Ks.Y << ", " << curMat.Ks.Z << "\n";
                std::cout << "Specular Exponent: " << curMat.Ns << "\n";
                std::cout << "Optical Density: " << curMat.Ni << "\n";
                std::cout << "Dissolve: " << curMat.d << "\n";
                std::cout << "Illumination: " << curMat.illum << "\n";
                std::cout << "Ambient Texture Map: " << curMat.map_Ka << "\n";
                std::cout << "Diffuse Texture Map: " << curMat.map_Kd << "\n";
                std::cout << "Specular Texture Map: " << curMat.map_Ks << "\n";
                std::cout << "Alpha Texture Map: " << curMat.map_d << "\n";
                std::cout << "Bump Map: " << curMat.map_bump << "\n\n";
            }
            matIndices.push_back(matIndex);
		}
	}
	else {
		std::cout << "Error couldn't load file at " << path << std::endl;
	}
	
}


void Model::setDefaultTexture(const char* filePath) {
    
    defaultTexture = loadTexture(filePath);
}

unsigned int Model::loadTexture(std::string filePath){
    //textures
    if (filePath == "" || filePath.back()=='/' ) {
        return 0;
    }
    for (int i = 0; i < loadedTextures.size(); i++)
    {
        if (loadedTextures[i].path == filePath) {
            std::cout << "Old Texture at " << filePath << std::endl;
            return loadedTextures[i].id;
        }
    }
    std::cout << "New Texture at " << filePath << std::endl;
    stbi_set_flip_vertically_on_load(false);
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture at " << filePath << std::endl;
        stbi_image_free(data);
        return 0;
    }
    stbi_image_free(data);
    Texture t;
    t.id = texture;
    t.path = filePath;
    loadedTextures.push_back(t);

    return texture;
}

