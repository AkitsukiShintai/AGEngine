#include "PCH.h"
#include <RenderPass.h>
#include <Graphics.h>
RenderPass::RenderPass(uint32_t attachmentCount,VkExtent2D _renderarea) :renderpass(VK_NULL_HANDLE) {
	device = Graphics::GetInstance()->device;
	attachments.resize(attachmentCount);
	renderArea = _renderarea;
}

VkAttachmentDescription RenderPass::CreateAttachmentDescriptionForDepthAttachment(
	VkAttachmentLoadOp loadOp,//= VK_ATTACHMENT_LOAD_OP_CLEAR,
	VkAttachmentStoreOp storeOp,// = VK_ATTACHMENT_STORE_OP_STORE
	VkAttachmentLoadOp stencilLoadOp,// = VK_ATTACHMENT_LOAD_OP_CLEAR,
	VkAttachmentStoreOp stencilStoreOp,//= VK_ATTACHMENT_STORE_OP_STORE,
	VkImageLayout startLayout ,//= VK_IMAGE_LAYOUT_UNDEFINED,
	VkImageLayout endLayout ,// = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	VkFormat format
) {

	VkAttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = loadOp;
	attachment.storeOp = storeOp;
	attachment.stencilLoadOp = stencilLoadOp;
	attachment.stencilStoreOp = stencilStoreOp;
	attachment.initialLayout = startLayout;
	attachment.finalLayout = endLayout;
	return attachment;
}

void RenderPass::CreateAttachmentDescriptionForColorAttachment(uint32_t attachIndex, VkFormat format,
	VkAttachmentLoadOp loadOp,
	VkAttachmentStoreOp storeOp,
	VkImageLayout startLayout ,
	VkImageLayout endLayout) {
	VkAttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = loadOp;
	attachment.storeOp = storeOp;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = startLayout;
	attachment.finalLayout = endLayout;
	attachments[attachIndex] = attachment;
	return;
}

VkAttachmentReference RenderPass::CreateAttachmentReference(uint32_t attachIndex, VkImageLayout layout) {
	VkAttachmentReference refe = {};
	refe.attachment = attachIndex;
	refe.layout = layout;
	return refe;
}

void RenderPass::CreateSubpassDescription(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference> colorAttachment,  bool enableDepth, VkAttachmentReference depthAttachment) {
	subpasses.push_back(SubpassDescription());
	subpasses[subpasses.size() - 1].pipelineBindPoint = bindPoint;
	subpasses[subpasses.size() - 1].colorAttachment = colorAttachment;
	subpasses[subpasses.size() - 1].depthStencilAttachment = depthAttachment;
	subpasses[subpasses.size() - 1].enableDepth = enableDepth;
}

void RenderPass::CreateSubpassDependency(uint32_t srcSubpass,
	uint32_t                dstSubpass,
	VkPipelineStageFlags    srcStageMask,
	VkPipelineStageFlags    dstStageMask,
	VkAccessFlags           srcAccessMask,
	VkAccessFlags           dstAccessMask,
	VkDependencyFlags       dependencyFlags) {
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = srcSubpass;
	dependency.dstSubpass = dstSubpass;
	dependency.srcStageMask = srcStageMask;
	dependency.dstStageMask = dstStageMask;
	dependency.srcAccessMask = srcAccessMask;
	dependency.dstAccessMask = dstAccessMask;
	dependency.dependencyFlags = dependencyFlags;
	subpassDependencies.push_back(dependency);
}



void RenderPass::CreateRenderPass() {
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();

	std::vector<VkSubpassDescription>  subpassDescriptions;
	subpassDescriptions.resize(subpasses.size());
	for (int i = 0; i < subpasses.size(); i++)
	{
		subpassDescriptions[i].pipelineBindPoint = subpasses[i].pipelineBindPoint;
		subpassDescriptions[i].colorAttachmentCount = subpasses[i].colorAttachment.size();
		subpassDescriptions[i].pColorAttachments = subpasses[i].colorAttachment.data();
		if (subpasses[i].enableDepth)
		{
			subpassDescriptions[i].pDepthStencilAttachment = &subpasses[i].depthStencilAttachment;
		}
		else
		{
			subpassDescriptions[i].pDepthStencilAttachment = VK_NULL_HANDLE;
		}
	}

	renderPassInfo.subpassCount = subpassDescriptions.size();
	renderPassInfo.pSubpasses = subpassDescriptions.data();
	renderPassInfo.dependencyCount = subpassDependencies.size();
	renderPassInfo.pDependencies = subpassDependencies.data();
	VkRenderPass m_RenderPass;
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderpass) != VK_SUCCESS) {
		throw std::exception("failed to create render pass!");
	}
}

void RenderPass::SetClearColor(std::vector<VkClearValue> _clearColors)
{
	clearColors = _clearColors;
}

void RenderPass::SetRenderArea(VkExtent2D area)
{
	renderArea = area;
}

