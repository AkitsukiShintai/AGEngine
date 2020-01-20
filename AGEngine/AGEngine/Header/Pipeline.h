#pragma once
class RenderPass;
class Pipeline {
public:
	static VkPipelineColorBlendAttachmentState GetColorBlendAttachmentState(bool enable, VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp blendOp = VK_BLEND_OP_ADD, VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD, VkColorComponentFlags colorFlag = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	std::vector<VkShaderModule> shaders;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributesDescription; 
	VkDescriptorSetLayout descriptorSetLayout;
	std::map<uint32_t, VkDescriptorSetLayoutBinding> descriptorSetLaytouBindings;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkPipelineDepthStencilStateCreateInfo depthInfo;
	uint32_t subpassIndex;
	VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo;
	std::vector <VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	std::vector<VkViewport> viewports;
	std::vector <VkRect2D> scissors;
	std::vector < VkPipelineRasterizationStateCreateInfo> rasterizers;
	std::vector < VkPipelineMultisampleStateCreateInfo> multisamplings;
	std::vector < VkDynamicState> dynamicStates;

	VkDevice device;
	VkExtent2D swipChainExtent;

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

public:
	Pipeline(const Pipeline& a) = delete;
	Pipeline& operator=(const Pipeline&) = delete;

	Pipeline(VkExtent2D _swipChainExtent, uint32_t _subpassIndex);

	void LoadShader(const std::vector<std::string> paths);

	VkShaderModule LoadShaderCode(VkShaderStageFlagBits _shader_type, std::vector<uint32_t> const& _binary);

	void SetPipelineInputeAssembly(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE);

	void SetViewPorts(std::vector<VkViewport> _viewPorts);

	void SetScissors(std::vector<VkRect2D> _scissors);

	void SetColorBlendings(std::vector <VkPipelineColorBlendAttachmentState> _colorBlendAttachments);

	void SetColorBlendingCreateInfo(VkBool32 opEnable, VkLogicOp logicOp, float blendConstants[4]);

	void AddRasterizer(VkBool32 depthClamp, VkBool32 rasterizerDiscardEnable, VkPolygonMode polygonMode, float lineWidth, VkBool32 depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, VkCullModeFlags cullMode, VkFrontFace frontFace);

	void AddMultiSampling(VkBool32 sampleShadingEnable, VkSampleCountFlagBits rasterizationSamples, float minSampleShading, VkSampleMask* pSampleMask, VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable);

	void SetDynamicState(std::vector<VkDynamicState> _dynamicStates);

	void CreatePipeline(const RenderPass& renderPass);

};