#pragma once
class DescriptorSet;
class Descriptor {
public:
	VkDescriptorImageInfo imageInfo;
	VkDescriptorBufferInfo bufferInfo;
	VkWriteDescriptorSet descriptorWrites;
	Descriptor(const GraphicsImage& image, uint32_t binding);

	Descriptor(const Buffer& buffer, uint32_t binding);
};
