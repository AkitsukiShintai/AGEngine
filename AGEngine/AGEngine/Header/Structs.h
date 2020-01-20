#pragma once

struct Image
{
	size_t width;
	size_t height;
	size_t depth;
	void* data;
	Image():width(),height(),depth(1),data(nullptr) {}
};

struct GraphicsImage{
	size_t width;
	size_t height;
	size_t depth;
	VkImage handle;
	VkImageView imageView;
	VkImageLayout currentLayout;
	VkImageLayout oldLayout;
	VkSampler sampler;
	VkFormat format;
	VkImageUsageFlags usage;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
};


struct Familys {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> transferFamily;
	std::optional<uint32_t> sparseBindingFamily;
	bool IsComplete(VkQueueFlags queueFlags) {
		bool pre = presentFamily.has_value();
		bool com = !(queueFlags & VK_QUEUE_COMPUTE_BIT) || computeFamily.has_value();
		bool gra = !(queueFlags & VK_QUEUE_GRAPHICS_BIT) || graphicsFamily.has_value();
		bool tra = !(queueFlags & VK_QUEUE_TRANSFER_BIT) || transferFamily.has_value();
		bool spar = !(queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) || sparseBindingFamily.has_value();
		return pre && com && gra && tra && spar;
	};
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


struct Queues {
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;
	VkQueue transferQueue;
	VkQueue sparseBindingQueue;
};


struct Buffer {
	VkBuffer handle;
	VkBufferUsageFlags usage;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
	uint32_t accurateSize;
	Buffer&  operator = (const Buffer& a) {
		handle = a.handle;
		usage = a.usage;
		allocation = a.allocation;
		allocationInfo = a.allocationInfo;
		accurateSize = a.accurateSize;
		return *this;
	}
};

struct Camera {
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 inverseViewMatrix;
	glm::mat4 inverseProjectionMatrix;
	glm::vec3 position;
};
