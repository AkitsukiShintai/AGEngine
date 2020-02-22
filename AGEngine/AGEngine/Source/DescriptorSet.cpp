#include <PCH.h>
#include <Descriptor.h>
#include <DescriptorSet.h>
#include <Graphics.h>

void DescriptorSet::CreateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout) {
	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = Graphics::GetInstance()->descriptorPool;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &descriptorSetLayout;

	if (vkAllocateDescriptorSets(Graphics::GetInstance()->device, &info, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
}

void DescriptorSet::UpdateDescriptorSet(std::vector<Descriptor> writeDescriptors) {
	std::vector<VkWriteDescriptorSet> writes;
	writes.resize(writeDescriptors.size());
	for (int i = 0; i< writeDescriptors.size(); i++)
	{
		writes[i] = writeDescriptors[i].descriptorWrites;
		writes[i].dstSet = descriptorSet;
	}
	vkUpdateDescriptorSets(Graphics::GetInstance()->device, writes.size(), writes.data(), 0, nullptr);
}