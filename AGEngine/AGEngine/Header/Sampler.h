#pragma once
class Sampler {
public:
	VkSampler sampler;
	VkSamplerCreateInfo createInfo;
	Sampler(VkDevice device, bool linear, std::array< VkSamplerAddressMode, 3> addressModes, VkBorderColor borderColor, VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR);
};