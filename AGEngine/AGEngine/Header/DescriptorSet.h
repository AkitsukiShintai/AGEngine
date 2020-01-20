#pragma once
class Descriptor;
class DescriptorSet {
public:
	VkDescriptorSet descriptorSet;
	void CreateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout);
	void UpdateDescriptorSet(std::vector<Descriptor> writeDescriptors);
};