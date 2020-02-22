#include <PCH.h>
#include <Graphics/RenderPass.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Graphics.h>
Framebuffer::Framebuffer():framebuffer(VK_NULL_HANDLE) {}

Framebuffer::Framebuffer(std::vector<GraphicsImage> images,const RenderPass& renderPass, uint32_t width, uint32_t height) {
	if (framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(Graphics::GetInstance()->device, framebuffer,nullptr);
	}
	std::vector<VkImageView> attachments(images.size());
	size_t i = 0;
	for (GraphicsImage& image : images)
	{
		attachments[i] = image.imageView;
		++i;
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass.renderpass;
	framebufferInfo.attachmentCount = attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Graphics::GetInstance()->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
		throw std::exception("failed to create framebuffer!");
	}
}


void Framebuffer::CreateFramebuffer(std::vector<GraphicsImage> images, const RenderPass& renderPass, uint32_t width, uint32_t height) {
	std::vector<VkImageView> attachments(images.size());
	size_t i = 0;
	for (GraphicsImage& image : images)
	{
		attachments[i] = image.imageView;
		++i;
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass.renderpass;
	framebufferInfo.attachmentCount = attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Graphics::GetInstance()->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
		throw std::exception("failed to create framebuffer!");
	}
}
