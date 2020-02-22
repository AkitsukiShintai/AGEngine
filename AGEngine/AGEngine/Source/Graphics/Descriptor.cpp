#include <PCH.h>
#include <Graphics/DescriptorSet.h>
#include <Graphics/Descriptor.h>

Descriptor::Descriptor(const GraphicsImage& image, uint32_t binding):imageInfo(), bufferInfo(), descriptorWrites() {
	descriptorWrites = {};
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstBinding = binding;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorType = (image.usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) != 0 ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorWrites.descriptorCount = 1;

	assert(image.sampler != VK_NULL_HANDLE);
	imageInfo.sampler = image.sampler;
	imageInfo.imageView = image.imageView;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptorWrites.pImageInfo = &imageInfo;
}

Descriptor::Descriptor(const Buffer& buffer, uint32_t binding) {
	descriptorWrites = {};
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstBinding = binding;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorType = (buffer.usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) != 0 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites.descriptorCount = 1;
	bufferInfo.buffer = buffer.handle;
	bufferInfo.offset = 0;
	bufferInfo.range = buffer.accurateSize;

	descriptorWrites.pBufferInfo = &bufferInfo;

}
