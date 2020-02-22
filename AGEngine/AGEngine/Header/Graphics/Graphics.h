#pragma once
#include <Graphics/CommandBuffer.h>
#include <Graphics/Mesh.h>

class Mesh;
class CommandBuffer;
class Window;
class Graphics:public Singleton<Graphics>{
private:
	friend class Singleton<Graphics>;
	Graphics();

	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void CreateMyWindow();

	void CreateInstance();
	void CreateDebugCallback();
	void CreateSurface();


	void PickPhysicalDevice();
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	void FindQueueFamilies(VkPhysicalDevice device);
	void CreateDevice();

	void CreateMemoryAllocator();

	void GetQueues(Familys queueFamily);

	void CreateSwapChain();

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void CreateSwapChainImageViews();

	void CreateSyncObjects();

	void CreateDescriptorPool();

	void CreateCommandPool();


	bool framebufferResized = false;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	VkSampler m_TextureSampler;

public:
	
	VkDescriptorPool descriptorPool;
	VkCommandPool commandPool;


	//vulkan memory allocator
	VmaAllocator allocator;

	//window
	Window* window;

	//configuration
	bool enableValidationLayers;
	std::vector<const char*> validationLayers;
	std::vector<const char*> requiredExtentions;
	std::vector<const char*> deviceExtensions;
	PFN_vkDebugUtilsMessengerCallbackEXT debugCallback;


	 bool VK_KHR_get_memory_requirements2_enabled = false;
	 bool VK_KHR_get_physical_device_properties2_enabled = false;
	 bool VK_KHR_dedicated_allocation_enabled = false;
	 bool VK_KHR_bind_memory2_enabled = false;
	 bool VK_EXT_memory_budget_enabled = false;

	VkQueueFlags queueFlags;

	
	//vulkan data
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	
	Familys familyIndices;
	Queues queues;

	VkSwapchainKHR swapChain;
	std::vector<GraphicsImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	CommandBuffer singleTimedCommand;
	const int MAX_FRAMES_IN_FLIGHT = 3;
	size_t currentFrame = 0;

	std::vector<BufferUpdateCallback> bufferUpdateCallbacks;
	
	void Init();

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	CommandBuffer CreateCommandBuffer(bool oneTimeSubmit = true, bool begin = true, bool primary = true);

	void BeginSingleTimedCommandBuffer();

	void EndAndSubmitSingleTimeCommandBuffer();

	//Image
	GraphicsImage CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkImageType type = VK_IMAGE_TYPE_2D, uint32_t layers = 1);

	GraphicsImage CreateTexture2D(uint32_t sizeX, uint32_t sizeY, void* data);

	void CreateImageView(GraphicsImage& image, VkFormat format, VkImageAspectFlags flag = VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D);

	Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags memoryFlag = VMA_ALLOCATION_CREATE_MAPPED_BIT);

	void CopyDataToGUPBuffer(void* data, Buffer& buffer, size_t size, size_t offset = 0);

	void CopyDataToBuffer(void* data, Buffer& buffer, size_t size, size_t offset = 0);

	void CmdCopyBufferToBuffer(VkCommandBuffer cmd, Buffer srcBuffer, Buffer dstBuffer);

	void DestroyBuffer(Buffer& buffer);

	void CreateVertexBuffer(Mesh& mesh);

	void CreateIndexBuffer(Mesh& mesh);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	void TransitionImageLayout(GraphicsImage& image, VkImageLayout newLayout, VkImageSubresourceRange subSourceRange = { VK_IMAGE_ASPECT_COLOR_BIT ,0,1,0,1 });
	//present

	uint32_t DrawFrameAndPresent(std::vector<CommandBuffer*> commandbuffers, GraphicsImage& renderTarget);

	void RecreateSwapChain();
};