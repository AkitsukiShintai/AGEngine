#include <PCH.h>
#include <Graphics/Sampler.h>

Sampler::Sampler(VkDevice device, bool linear, std::array< VkSamplerAddressMode, 3> addressModes, VkBorderColor borderColor, VkSamplerMipmapMode mipmapMode) {

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = linear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
	samplerInfo.minFilter = samplerInfo.magFilter;

	samplerInfo.addressModeU = addressModes[0];
	samplerInfo.addressModeV = addressModes[1];
	samplerInfo.addressModeW = addressModes[2];

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = borderColor;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;

	samplerInfo.mipmapMode = mipmapMode;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
};