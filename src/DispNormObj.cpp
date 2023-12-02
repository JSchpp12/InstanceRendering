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

void DispNormObj::cleanupRender(star::StarDevice& device)
{
	this->crackTexture->cleanupRender(device); 

	this->star::BasicObject::cleanupRender(device); 
}

star::StarObjectInstance& DispNormObj::createInstance()
{
	int instanceCount = static_cast<int>(this->instances.size());

	this->instances.push_back(std::make_unique<DispNormObjInstance>(instanceCount));
	return *this->instances.back();
}

std::unordered_map<star::Shader_Stage, star::StarShader> DispNormObj::getShaders()
{
	std::unordered_map<star::Shader_Stage, star::StarShader> shaders; 

	//load vertex shader
	std::string vertShaderPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "/shaders/dispTexture.vert";
	shaders.insert(std::pair<star::Shader_Stage, star::StarShader>(star::Shader_Stage::vertex, star::StarShader(vertShaderPath, star::Shader_Stage::vertex)));

	//load fragment shader
	std::string fragShaderPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "/shaders/dispTexture.frag";
	shaders.insert(std::pair<star::Shader_Stage, star::StarShader>(star::Shader_Stage::fragment, star::StarShader(fragShaderPath, star::Shader_Stage::fragment)));

	return shaders; 
}

std::vector<std::unique_ptr<star::StarDescriptorSetLayout>> DispNormObj::getDescriptorSetLayouts(star::StarDevice& device)
{
	auto allSets = std::vector<std::unique_ptr<star::StarDescriptorSetLayout>>();
	auto staticSetBuilder = star::StarDescriptorSetLayout::Builder(device);

	star::StarDescriptorSetLayout::Builder updateSetBuilder = star::StarDescriptorSetLayout::Builder(device)
		.addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
		.addBinding(1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
		.addBinding(2, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex); 

	this->getMeshes().front()->getMaterial().applyDescriptorSetLayouts(staticSetBuilder, updateSetBuilder);
		
	allSets.push_back(std::move(updateSetBuilder.build()));

	auto staticSet = staticSetBuilder.build();

	if (staticSet->getBindings().size() > 0)
		allSets.push_back(std::move(staticSet));

	auto decal = star::StarDescriptorSetLayout::Builder(device)
		.addBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
		.build(); 

	allSets.push_back(std::move(decal)); 

	return allSets;
}

void DispNormObj::initRender(int numFramesInFlight)
{
	this->star::BasicObject::initRender(numFramesInFlight); 

	star::ManagerDescriptorPool::request(vk::DescriptorType::eCombinedImageSampler, numFramesInFlight);
}

void DispNormObj::prepRender(star::StarDevice& device, vk::Extent2D swapChainExtent, 
	vk::PipelineLayout pipelineLayout, vk::RenderPass renderPass, int numSwapChainImages, 
	std::vector<std::reference_wrapper<star::StarDescriptorSetLayout>> groupLayout,
	std::vector<std::vector<vk::DescriptorSet>> globalSets)
{
	this->crackTexture->prepRender(device);

	this->star::BasicObject::prepRender(device, swapChainExtent, pipelineLayout, renderPass, numSwapChainImages, groupLayout, globalSets);  
}

void DispNormObj::prepRender(star::StarDevice& device, int numSwapChainImages, 
	std::vector<std::reference_wrapper<star::StarDescriptorSetLayout>> groupLayout,
	std::vector<std::vector<vk::DescriptorSet>> globalSets, star::StarPipeline& sharedPipeline)
{
	this->crackTexture->prepRender(device);

	this->star::BasicObject::prepRender(device, numSwapChainImages, groupLayout, globalSets, sharedPipeline); 
}

DispNormObj::DispNormObj(std::string objPath) : star::BasicObject(objPath)
{
	auto texPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "images/cracks.png"; 
	this->crackTexture = std::make_unique<star::Texture>(texPath);
	this->crackTexture->setAlpha(star::Color(0, 0, 0, 30)); 
}

void DispNormObj::prepareDescriptors(star::StarDevice& device, int numSwapChainImages, 
	std::vector<std::reference_wrapper<star::StarDescriptorSetLayout>> fullGroupLayout,
	std::vector<std::vector<vk::DescriptorSet>> globalSets)
{
	auto& groupLayout = fullGroupLayout.at(1);
	star::StarDescriptorPool& pool = star::ManagerDescriptorPool::getPool();
	std::vector<std::unordered_map<int, vk::DescriptorSet>> finalizedSets; 

	for (int i = 0; i < numSwapChainImages; i++) {
		std::unordered_map<int, vk::DescriptorSet> set; 
		for (int j = 0; j < globalSets.at(i).size(); j++) {
			set[j] = globalSets.at(i).at(j); 
		}

		star::StarDescriptorWriter writer = star::StarDescriptorWriter(device, groupLayout, star::ManagerDescriptorPool::getPool());

		std::vector<vk::DescriptorBufferInfo> bufferInfos = std::vector<vk::DescriptorBufferInfo>(this->instances.front()->getBufferInfoSize().size());

		for (int j = 0; j < this->instances.front()->getBufferInfoSize().size(); j++) {
			vk::DeviceSize bufferSize = this->instances.front()->getBufferInfoSize().at(j) * this->instances.size();

			//add descriptor for uniform buffer object
			bufferInfos.at(j) = vk::DescriptorBufferInfo{
				this->instanceUniformBuffers[i][j]->getBuffer(),
				0,
				bufferSize };
			writer.writeBuffer(j, bufferInfos.at(j));
		}
		vk::DescriptorSet objectSet = writer.build();

		vk::DescriptorSet objectTexSet; 
		{
			auto& staticLayout = fullGroupLayout.at(3);
			star::StarDescriptorWriter writer = star::StarDescriptorWriter(device, staticLayout, star::ManagerDescriptorPool::getPool());

			vk::DescriptorImageInfo imageInfo{
				this->crackTexture->getSampler(),
				this->crackTexture->getImageView(),
				vk::ImageLayout::eShaderReadOnlyOptimal
			};
			writer.writeImage(0, imageInfo);
			objectTexSet = writer.build();
		}

		set[1] = objectSet;
		set[3] = objectTexSet; 

		finalizedSets.push_back(set); 
	}

	auto& materialLayout = fullGroupLayout.at(2);

	for (auto& mesh : this->getMeshes()) {
		//descriptors
		mesh->getMaterial().finalizeDescriptors(device, materialLayout, pool, finalizedSets, numSwapChainImages);
	}


}
