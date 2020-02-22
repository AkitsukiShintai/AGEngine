#pragma once
#include <RenderPass.h>
#include <Framebuffer.h>
#include <Pipeline.h>
#include <DescriptorSet.h>
#include <Mesh.h>
class DescriptorSet;
class RenderPass;
class Framebuffer;
class Pipeline;
class CommandBuffer {
public:
	CommandBuffer();
	VkCommandBufferBeginInfo beginInfo;
	VkCommandBuffer commandBuffer;
	bool primary;
	bool oneTime;
	bool start;
	bool end;
	const RenderPass* lastRenderPass;
	void CopyImageToImage(uint32_t width, uint32_t height, GraphicsImage& srcImage, GraphicsImage& dstImage);

	void BeginRenderpass(const RenderPass& renderpass, const Framebuffer& framebuffer);

	void BeginPipeline(const Pipeline& pipeline, const RenderPass& renderpass, const  DescriptorSet& descriptorSet);

	void Draw(const Mesh& mesh, VertexInputFlags inputData, uint32_t instantCount, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0, VkIndexType indexType = VK_INDEX_TYPE_UINT32);

	void Draw(const Buffer& vertexbuffer, const Buffer& indexbuffer, uint32_t vertexCount, uint32_t instanceCount = 1, VkIndexType indexType = VK_INDEX_TYPE_UINT32);

	void TransitionImageLayout(GraphicsImage& image, VkImageLayout newLayout, VkImageSubresourceRange subSourceRange = { VK_IMAGE_ASPECT_COLOR_BIT ,0,1,0,1 });

	void EndRenderpass();

	void EndCommand();

	void StartCommand();
};