#include "DispNormObj.hpp"

void DispNormObjInstance::updateBufferData(star::StarBuffer& buffer, int bufferIndex)
{
	switch (bufferIndex) {
	case(0):
		auto displayMatrix = this->getDisplayMatrix();
		buffer.writeToBuffer(&displayMatrix, sizeof(displayMatrix), sizeof(displayMatrix) * this->instanceIndex);
		break;
	case(1):
		auto normalMatrix = glm::inverseTranspose(getDisplayMatrix());
		//buffer.writeToIndex(&normalMatrix, this->instanceIndex);
		buffer.writeToBuffer(&displayMatrix, sizeof(normalMatrix), sizeof(normalMatrix) * this->instanceIndex);
		break;
	case(2):
		buffer.writeToBuffer(&this->textureDisplacement, sizeof(this->textureDisplacement), sizeof(this->textureDisplacement) * this->instanceIndex); 
	}
}

std::unique_ptr<DispNormObj> DispNormObj::New(std::string objPath)
{
	return std::unique_ptr<DispNormObj>(new DispNormObj(objPath));
}

std::unordered_map<star::Shader_Stage, star::StarShader> DispNormObj::getShaders()
{
	std::unordered_map<star::Shader_Stage, star::StarShader> shaders; 

	//load vertex shader
	std::string vertShaderPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "/shaders/dispTexture.vert";
	shaders.insert(std::pair<star::Shader_Stage, star::StarShader>(star::Shader_Stage::vertex, star::StarShader(vertShaderPath, star::Shader_Stage::vertex)));

	//load fragment shader
	std::string fragShaderPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "/shaders/bump.frag";
	shaders.insert(std::pair<star::Shader_Stage, star::StarShader>(star::Shader_Stage::fragment, star::StarShader(fragShaderPath, star::Shader_Stage::fragment)));

	return shaders; 
}
