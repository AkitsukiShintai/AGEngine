#include "PCH.h"
#include <RenderPass.h>
#include <FShaderCompiler.h>
#include <Pipeline.h>
#include <Graphics.h>

VkPipelineColorBlendAttachmentState Pipeline::GetColorBlendAttachmentState(bool enable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp blendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp, VkColorComponentFlags colorFlag)
{
	VkPipelineColorBlendAttachmentState state;
	state.blendEnable = enable;
	state.srcColorBlendFactor = srcColorBlendFactor;
	state.dstColorBlendFactor = srcColorBlendFactor;
	state.colorBlendOp = blendOp;
	state.srcAlphaBlendFactor = srcAlphaBlendFactor;
	state.dstAlphaBlendFactor = dstAlphaBlendFactor;
	state.alphaBlendOp = alphaBlendOp;
	state.colorWriteMask = colorFlag;
	return state;
}

Pipeline::Pipeline(VkExtent2D _swipChainExtent,uint32_t _subpassIndex) :subpassIndex(_subpassIndex), device(VK_NULL_HANDLE), swipChainExtent(_swipChainExtent) {
	device = Graphics::GetInstance()->device;
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = false;
}

void Pipeline::LoadShader(const std::vector<std::string> paths) {
	ShaderCompiler* shaderCompiler = ShaderCompiler::GetInstance();
	for (std::string const& name : paths)
	{
		std::cout << "Loading Shader: " << name << std::endl;
		std::string _suffix = File::getSuffix(name);
		VkShaderStageFlagBits shader_type = VK_SHADER_STAGE_VERTEX_BIT;
		if (_suffix == ".vert")
		{
			shader_type = VK_SHADER_STAGE_VERTEX_BIT;
		}
		else if (_suffix == ".frag")
		{
			shader_type = VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		else if (_suffix == ".geom")
		{
			shader_type = VK_SHADER_STAGE_GEOMETRY_BIT;
		}
		else if (_suffix == ".tesc")
		{
			shader_type = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}
		else if (_suffix == ".tese")
		{
			shader_type = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}
		else if (_suffix == ".comp")
		{
			shader_type = VK_SHADER_STAGE_COMPUTE_BIT;
		}
		else
		{
			std::cerr << "Failed To Load Shader " << name << std::endl;
			//FDebug::reportErr("Failed To Load Shader!");
			return;
		}
		std::string src = File::loadTextFile(name);

		SearchingForUniform(src, shader_type, descriptorSetLaytouBindings);

		src = shaderCompiler->PreprocessGLSLShader(name, shader_type, src);
		if (shader_type == VK_SHADER_STAGE_VERTEX_BIT)
		{
			src = ProcessingForAttribute(src, vertexInputBindingDescription, vertexInputAttributesDescription);
		}

		//std::cout << src << std::endl;
		std::vector<uint32_t> binary = shaderCompiler->CompileGLSLShader(name, shader_type, src);
		VkShaderModule module = LoadShaderCode(shader_type, binary);
		shaders.push_back(module);

		VkPipelineShaderStageCreateInfo stageInfo = {};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = shader_type;
		stageInfo.module = module;
		stageInfo.pName = "main";

		shaderStages.push_back(stageInfo);
	}

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	for (auto t : descriptorSetLaytouBindings)
	{
		bindings.push_back(t.second);
	}
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VkShaderModule Pipeline::LoadShaderCode(VkShaderStageFlagBits _shader_type, std::vector<uint32_t> const& _binary)
{
	VkShaderModule module;
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _binary.size() * sizeof(uint32_t);
	createInfo.pCode = _binary.data();
	ERR_GUARD_VULKAN(vkCreateShaderModule(device, &createInfo, nullptr, &module));
	return module;
}

void Pipeline::SetPipelineInputeAssembly(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
{
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
}

void Pipeline::SetViewPorts(std::vector<VkViewport> _viewPorts)
{
	viewports = _viewPorts;
}

void Pipeline::SetScissors(std::vector<VkRect2D> _scissors)
{
	scissors = _scissors;
}

void Pipeline::SetColorBlendings(std::vector<VkPipelineColorBlendAttachmentState> _colorBlendAttachments)
{
	colorBlendAttachments = _colorBlendAttachments;
}

void Pipeline::SetColorBlendingCreateInfo(VkBool32 opEnable, VkLogicOp logicOp, float blendConstants[4])
{

	colorBlendingCreateInfo = {};
	colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;

	for (int i = 0; i < 4; i++)
	{
		colorBlendingCreateInfo.blendConstants[i] = blendConstants[i];
	}

}

void Pipeline::AddRasterizer(VkBool32 depthClamp, VkBool32 rasterizerDiscardEnable, VkPolygonMode polygonMode, float lineWidth, VkBool32 depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, VkCullModeFlags cullMode, VkFrontFace frontFace)
{

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = depthClamp;
	rasterizer.rasterizerDiscardEnable = rasterizerDiscardEnable;
	rasterizer.polygonMode = polygonMode;
	rasterizer.lineWidth = lineWidth;

	rasterizer.depthBiasEnable = depthBiasEnable;
	rasterizer.depthBiasConstantFactor = depthBiasConstantFactor;
	rasterizer.depthBiasClamp = depthBiasClamp;
	rasterizer.depthBiasSlopeFactor = depthBiasSlopeFactor;
	rasterizer.cullMode = cullMode;
	rasterizer.frontFace = frontFace;
	rasterizers.push_back(rasterizer);
}

void Pipeline::AddMultiSampling(VkBool32 sampleShadingEnable, VkSampleCountFlagBits rasterizationSamples, float minSampleShading, VkSampleMask* pSampleMask, VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable)
{
	VkPipelineMultisampleStateCreateInfo ultisampling = {};
	ultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ultisampling.sampleShadingEnable = sampleShadingEnable;
	ultisampling.rasterizationSamples = rasterizationSamples;
	ultisampling.minSampleShading = minSampleShading; // Optional
	ultisampling.pSampleMask = pSampleMask; // Optional
	ultisampling.alphaToCoverageEnable = alphaToCoverageEnable; // Optional
	ultisampling.alphaToOneEnable = alphaToOneEnable; // Optional
	multisamplings.push_back(ultisampling);
}

void Pipeline::SetDynamicState(std::vector<VkDynamicState> _dynamicStates) {
	dynamicStates = _dynamicStates;
}

void Pipeline::CreatePipeline(const RenderPass& renderPass)
{

	if (viewports.size() == 0)
	{
		VkViewport m_viewport = {};
		m_viewport.x = 0;
		m_viewport.y = 0;
		m_viewport.width = swipChainExtent.width;
		m_viewport.height = swipChainExtent.height;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;
		viewports.push_back(m_viewport);
	}

	if (scissors.size() == 0)
	{
		VkRect2D m_scissor = {};
		m_scissor.offset = { 0,0 };
		m_scissor.extent = swipChainExtent;
		scissors.push_back(m_scissor);
	}

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = viewports.size();
	viewportState.pViewports = viewports.data();
	viewportState.scissorCount = scissors.size();
	viewportState.pScissors = scissors.data();

	if (rasterizers.size() == 0)
	{
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizers.push_back(rasterizer);
	}


	if (multisamplings.size() == 0)
	{
		VkPipelineMultisampleStateCreateInfo ultisampling = {};
		ultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ultisampling.sampleShadingEnable = VK_FALSE;
		ultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		ultisampling.minSampleShading = 1.0f; // Optional
		ultisampling.pSampleMask = nullptr; // Optional
		ultisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		ultisampling.alphaToOneEnable = VK_FALSE; // Optional
		multisamplings.push_back(ultisampling);
	}

	if (colorBlendAttachments.size() == 0)
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachments.push_back(colorBlendAttachment);

		colorBlendingCreateInfo = {};
		colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendingCreateInfo.blendConstants[0] = 0.0f;
		colorBlendingCreateInfo.blendConstants[1] = 0.0f;
		colorBlendingCreateInfo.blendConstants[2] = 0.0f;
		colorBlendingCreateInfo.blendConstants[3] = 0.0f;
	}
	colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingCreateInfo.attachmentCount = colorBlendAttachments.size();
	colorBlendingCreateInfo.pAttachments = colorBlendAttachments.data();

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = dynamicStates.size();
	dynamicState.pDynamicStates = dynamicStates.size() == 0? VK_NULL_HANDLE : dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::exception("failed to create pipeline layout!");
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount = vertexInputBindingDescription.size();
	vertexInputInfo.vertexAttributeDescriptionCount = vertexInputAttributesDescription.size();
	vertexInputInfo.pVertexBindingDescriptions = vertexInputBindingDescription.data();
	vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributesDescription.data();


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = rasterizers.data();
	pipelineInfo.pMultisampleState = multisamplings.data();
	pipelineInfo.pDepthStencilState = setDepth == true ? &depthInfo : nullptr;

	pipelineInfo.pColorBlendState = &colorBlendingCreateInfo;
	pipelineInfo.pDynamicState = &dynamicState; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass.renderpass;
	pipelineInfo.subpass = subpassIndex;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkResult r = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	if (r != VK_SUCCESS) {
		throw std::exception("failed to create graphics pipeline!");
	}

	//m_RenderPass_Pipelines[renderPass].push_back(pipeline);
	for (auto& stage : shaders)
	{
		vkDestroyShaderModule(device, stage, nullptr);
	}

}

void Pipeline::SetDepthStencilState(bool depthTestEnable, bool stencilTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTest)
{
	setDepth = true;
	depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthInfo.depthBoundsTestEnable = depthBoundsTest;
	depthInfo.depthCompareOp = depthCompareOp;
	depthInfo.depthTestEnable = depthTestEnable;
	depthInfo.stencilTestEnable = stencilTestEnable;
	depthInfo.depthWriteEnable = depthWriteEnable;
	depthInfo.maxDepthBounds = 1.0f;
	depthInfo.minDepthBounds = 0.0f;
};
