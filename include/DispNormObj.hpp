#pragma once 

#include "StarObject.hpp"
#include "StarObjectInstance.hpp"
#include "BasicObject.hpp"


class DispNormObjInstance : public star::StarObjectInstance {
public:
	glm::mat4 textureDisplacement = glm::mat4(0); 

	DispNormObjInstance(int instanceIndex) : star::StarObjectInstance(instanceIndex) {}

	virtual void updateBufferData(star::StarBuffer& buffer, int bufferIndex) override; 

	virtual std::vector<vk::DeviceSize> getBufferInfoSize() override{
		return std::vector<vk::DeviceSize>{
			sizeof(glm::mat4),
			sizeof(glm::mat4),
			sizeof(glm::mat4)
		};
	}
};

class DispNormObj : public star::BasicObject {
public:
	static std::unique_ptr<DispNormObj> New(std::string objPath); 

	virtual std::unordered_map<star::Shader_Stage, star::StarShader> getShaders() override; 

protected:
	DispNormObj(std::string objPath) : star::BasicObject(objPath) {};

};