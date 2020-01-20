#pragma once
class Framebuffer {
public:
	VkFramebuffer framebuffer;
	Framebuffer(std::vector<GraphicsImage> images, const RenderPass& renderPass, uint32_t width, uint32_t height);
	Framebuffer();
	void CreateFramebuffer(std::vector<GraphicsImage> images, const RenderPass& renderPass, uint32_t width, uint32_t height);
};