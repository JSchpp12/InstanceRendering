#pragma once 

#include "StarObject.hpp"
#include "StarObjectInstance.hpp"
#include "BasicObject.hpp"


class DispNormObjInstance : public star::StarObjectInstance {
public:
	glm::mat4 textureDisplacement = glm::mat4(1.0); 

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

	virtual void cleanupRender(star::StarDevice& device) override;

	virtual star::StarObjectInstance& createInstance() override;

	virtual std::unordered_map<star::Shader_Stage, star::StarShader> getShaders() override; 

	virtual std::vector<std::unique_ptr<star::StarDescriptorSetLayout>> getDescriptorSetLayouts(star::StarDevice& device) override;

	virtual void initRender(int numFramesInFlight) override;

	virtual void prepRender(star::StarDevice& device, vk::Extent2D swapChainExtent,
		vk::PipelineLayout pipelineLayout, vk::RenderPass renderPass, int numSwapChainImages,
		std::vector<std::reference_wrapper<star::StarDescriptorSetLayout>> groupLayout, std::vector<std::vector<vk::DescriptorSet>> globalSets) override;

	virtual void prepRender(star::StarDevice& device, int numSwapChainImages,
		std::vector<std::reference_wrapper<star::StarDescriptorSetLayout>> groupLayout,
		std::vector<std::vector<vk::DescriptorSet>> globalSets, star::StarPipeline& sharedPipeline) override; 

protected:
	std::unique_ptr<star::Texture> crackTexture;
	std::unique_ptr<star::StarDescriptorSetLayout> staticSetLayout; 

	DispNormObj(std::string objPath);

	virtual void prepareDescriptors(star::StarDevice& device, int numSwapChainImages,
		std::vector<std::reference_wrapper<star::StarDescriptorSetLayout>> groupLayout, 
		std::vector<std::vector<vk::DescriptorSet>> globalSets) override;

	std::vector<std::unique_ptr<DispNormObjInstance>> instance; 

};