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
	std::string fragShaderPath = star::ConfigFile::getSetting(star::Config_Settings::mediadirectory) + "/shaders/bump.frag";
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

	return std::move(allSets);
}

void DispNormObj::prepareDescriptors(star::StarDevice& device, int numSwapChainImages, std::vector<std::unique_ptr<star::StarDescriptorSetLayout>>& fullGroupLayout, std::vector<std::vector<vk::DescriptorSet>> globalSets)
{
	auto& groupLayout = *fullGroupLayout.at(1);
	star::StarDescriptorPool& pool = star::ManagerDescriptorPool::getPool();

	//create descriptor layout
	this->setLayout = star::StarDescriptorSetLayout::Builder(device)
		.addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
		.addBinding(1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
		.addBinding(2, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
		.build();

	for (int i = 0; i < numSwapChainImages; i++) {
		star::StarDescriptorWriter writer = star::StarDescriptorWriter(device, *this->setLayout, star::ManagerDescriptorPool::getPool());

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
		vk::DescriptorSet set = writer.build();

		globalSets.at(i).push_back(set);
	}

	for (auto& mesh : this->getMeshes()) {
		//descriptors
		mesh->getMaterial().buildDescriptorSets(device, groupLayout, pool, globalSets, numSwapChainImages);
	}
}
