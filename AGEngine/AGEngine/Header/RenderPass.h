#pragma once
class RenderPass {
	struct SubpassDescription {
		VkPipelineBindPoint            pipelineBindPoint;
		std::vector<VkAttachmentReference> inputAttachment;
		std::vector<VkAttachmentReference> colorAttachment;
		VkAttachmentReference depthStencilAttachment;
		std::vector<VkAttachmentReference> resolveAttachment;
		std::vector<VkAttachmentReference> preserveAttachment;
		bool enableDepth;
	};

public:
	VkExtent2D renderArea;
	VkDevice device;
	VkRenderPass renderpass;
	RenderPass(const RenderPass& a) = delete;
	RenderPass& operator=(const RenderPass&) = delete;

	RenderPass(uint32_t attachmentCount, VkExtent2D renderArea);
	std::vector<SubpassDescription> subpasses;
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDependency> subpassDependencies;
	std::vector<VkClearValue> clearColors;

	static VkAttachmentDescription CreateAttachmentDescriptionForDepthAttachment(
		VkAttachmentLoadOp loadOp,//= VK_ATTACHMENT_LOAD_OP_CLEAR,
		VkAttachmentStoreOp storeOp,// = VK_ATTACHMENT_STORE_OP_STORE
		VkAttachmentLoadOp stencilLoadOp,// = VK_ATTACHMENT_LOAD_OP_CLEAR,
		VkAttachmentStoreOp stencilStoreOp,//= VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout startLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,//= VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout endLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,// = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkFormat format = DEPTH_ATTACHMENT_FORMAT
	);

	 void CreateAttachmentDescriptionForColorAttachment(uint32_t attachIndex, VkFormat format,
		VkAttachmentLoadOp loadOp,
		VkAttachmentStoreOp storeOp,
		VkImageLayout startLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout endLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	static VkAttachmentReference CreateAttachmentReference(uint32_t attachIndex, VkImageLayout layout);

	void CreateSubpassDescription(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference> colorAttachment,bool enableDepth, VkAttachmentReference depthAttachment);

	void CreateSubpassDependency(uint32_t srcSubpass,
		uint32_t                dstSubpass,
		VkPipelineStageFlags    srcStageMask,
		VkPipelineStageFlags    dstStageMask,
		VkAccessFlags           srcAccessMask,
		VkAccessFlags           dstAccessMask,
		VkDependencyFlags       dependencyFlags);

	void CreateRenderPass();

	void SetClearColor(std::vector<VkClearValue> _clearColors);

	void SetRenderArea(VkExtent2D area);
};