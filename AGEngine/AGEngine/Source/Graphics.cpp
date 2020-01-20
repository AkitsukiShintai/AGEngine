#include <PCH.h>
#include <Graphics.h>

POINT mouseLastPos;
POINT mouseCurrentPos;

static VKAPI_ATTR VkBool32 VKAPI_CALL DefaultDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT m_essageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT m_essageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (m_essageType & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		//assert(false);
	}
	return VK_FALSE;
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Graphics* g = Graphics::GetInstance();
	switch (msg)
	{
	case WM_CREATE:
		// This is intentionally assigned here because we are now inside CreateWindow, before it returns.
		g->winWindowHandle = hWnd;
		return 0;

	case WM_DESTROY:
		//FinalizeApplication();
		PostQuitMessage(0);
		return 0;

		// This prevents app from freezing when left Alt is pressed
		// (which normally enters modal menu loop).
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return 0;

	case WM_SIZE:
		if ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))
			//HandlePossibleSizeChange();
		return 0;

	case WM_EXITSIZEMOVE:
		//HandlePossibleSizeChange();
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			Graphics::GetInstance()->close = true;
			break;
		case 'T':
			
			break;
		case 'S':
			
			break;
		}
		return 0;
	case WM_SETCURSOR:	case WM_DEVICECHANGE:
	case WM_MOUSEMOVE:
		mouseLastPos = mouseCurrentPos;
		GetCursorPos(&mouseCurrentPos);
		for (auto t : g->mouseUpdateCallbacks)
		{
			t(mouseCurrentPos.x - mouseLastPos.x, mouseCurrentPos.y - mouseLastPos.y);
		}
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Graphics::Graphics() :
	close(false),
	windowName("DefaultName"),
	width(800),
	height(600),
	//glfwWindow(nullptr),
	applicationName("AGEngine"),
	enableValidationLayers(true),
	validationLayers(),
	requiredExtentions(),
	deviceExtensions(),
	debugCallback(),
	queueFlags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT),
	winWindowHandle(nullptr),
	winAppliocationInstance(nullptr),
	instance(VK_NULL_HANDLE),
	physicalDevice(VK_NULL_HANDLE),
	device(VK_NULL_HANDLE),
	debugMessenger(VK_NULL_HANDLE),
	surface(VK_NULL_HANDLE),
	familyIndices()

{
};


bool Graphics::CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}
std::vector<const char*> Graphics::GetRequiredExtensions() {
	std::vector<const char*> enabledInstanceExtensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
#ifdef GLFW
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		enabledInstanceExtensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers) {
		enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
#else
	uint32_t availableInstanceExtensionCount = 0;
	ERR_GUARD_VULKAN(vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, nullptr));
	std::vector<VkExtensionProperties> availableInstanceExtensions(availableInstanceExtensionCount);
	if (availableInstanceExtensionCount > 0)
	{
		ERR_GUARD_VULKAN(vkEnumerateInstanceExtensionProperties(nullptr, &availableInstanceExtensionCount, availableInstanceExtensions.data()));
	}

	
	enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	//enabledInstanceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	if (enableValidationLayers == true)
	{
		enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		enabledInstanceExtensions.push_back("VK_EXT_debug_report");
	}

	for (const auto& extensionProperties : availableInstanceExtensions)
	{
		if (strcmp(extensionProperties.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0)
		{
			enabledInstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
			VK_KHR_get_physical_device_properties2_enabled = true;
		}
	}
#endif


	return enabledInstanceExtensions;
}
VkResult Graphics::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Graphics::CreateMyWindow() {
	/*glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindow = glfwCreateWindow(width, height, applicationName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(glfwWindow, this);*/

	HINSTANCE g_hAppInstance = (HINSTANCE)GetModuleHandle(NULL);

	WNDCLASSEX wndClassDesc = { sizeof(WNDCLASSEX) };
	wndClassDesc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wndClassDesc.hbrBackground = NULL;
	wndClassDesc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndClassDesc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClassDesc.hInstance = g_hAppInstance;
	wndClassDesc.lpfnWndProc = WndProc;
	wndClassDesc.lpszClassName = applicationName.c_str();

	const ATOM hWndClass = RegisterClassEx(&wndClassDesc);
	assert(hWndClass);

	const DWORD style = WS_OVERLAPPEDWINDOW;
	const DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	RECT rect = { 0, 0, width, height };
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);
	
	CreateWindowEx(
		exStyle, applicationName.c_str(), applicationName.c_str(), style,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, g_hAppInstance, NULL);
	ShowWindow(winWindowHandle, SW_SHOW);
	SetForegroundWindow(winWindowHandle);
	SetFocus(winWindowHandle);
}

void Graphics::CreateInstance() {
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::exception("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VoidEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VMA_VULKAN_VERSION == 1001000 ? VK_API_VERSION_1_1 : VK_API_VERSION_1_0;;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (requiredExtentions.size() == 0)
	{
		requiredExtentions = GetRequiredExtensions();
	}
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtentions.size());
	createInfo.ppEnabledExtensionNames = requiredExtentions.data();
	createInfo.enabledLayerCount = 0;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	VkResult r = vkCreateInstance(&createInfo, nullptr, &instance);
	if (r!= VK_SUCCESS) {
		throw std::exception("failed to create instance!");
	}
}
void Graphics::CreateDebugCallback() {

	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	if (debugCallback != nullptr)
	{
		createInfo.pfnUserCallback = debugCallback;

	}
	else
	{
		createInfo.pfnUserCallback = DefaultDebugCallback;
	}
	createInfo.pUserData = nullptr; // Option
	VkResult r = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);

	if (r != VK_SUCCESS) {
		throw std::exception("failed to set up debug call back");
	}
}

void Graphics::CreateSurface() {
	if (winWindowHandle != nullptr)
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = winWindowHandle;
		createInfo.hinstance = winAppliocationInstance;

		if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::exception("failed to create window surface!");
		}
	}
	else
	{
#ifdef GLFW

		if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface) != VK_SUCCESS) {
			throw std::exception("failed to create window surface!");
		}
#endif // GLFW

	}
}

void Graphics::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::exception("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::exception("failed to find a suitable GPU!");
	}

}
SwapChainSupportDetails Graphics::QuerySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
bool Graphics::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	{
		uint32_t propertyCount = 0;
		ERR_GUARD_VULKAN(vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, nullptr));
		if (propertyCount)
		{
			std::vector<VkExtensionProperties> properties{ propertyCount };
			ERR_GUARD_VULKAN(vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, properties.data()));

			for (uint32_t i = 0; i < propertyCount; ++i)
			{
				if (strcmp(properties[i].extensionName, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME) == 0)
				{
					deviceExtensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
					VK_KHR_get_memory_requirements2_enabled = true;
				}
				else if (strcmp(properties[i].extensionName, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME) == 0)
				{
					deviceExtensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
					VK_KHR_dedicated_allocation_enabled = true;
				}
				else if (strcmp(properties[i].extensionName, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME) == 0)
				{
					deviceExtensions.push_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
					VK_KHR_bind_memory2_enabled = true;
				}
				else if (strcmp(properties[i].extensionName, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) == 0)
				{
					deviceExtensions.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
					VK_EXT_memory_budget_enabled = true;
				}
			}
		}
	}
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}
bool Graphics::IsDeviceSuitable(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		return false;
	}
	FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return familyIndices.IsComplete(queueFlags) && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;

}
void Graphics::FindQueueFamilies(VkPhysicalDevice device) {
	if (familyIndices.IsComplete(queueFlags))
	{
		return;
	}
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (size_t i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueCount > 0 && (queueFlags & VK_QUEUE_GRAPHICS_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			familyIndices.graphicsFamily = i;
		}

		if (queueFamilies[i].queueCount > 0 && (queueFlags & VK_QUEUE_COMPUTE_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			familyIndices.computeFamily = i;
		}
		if (queueFamilies[i].queueCount > 0 && (queueFlags & VK_QUEUE_TRANSFER_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			familyIndices.transferFamily = i;
		}
		if (queueFamilies[i].queueCount > 0 && (queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			familyIndices.sparseBindingFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && presentSupport) {
			familyIndices.presentFamily = i;
		}
		if (familyIndices.IsComplete(queueFlags))
		{
			return;
		}
	}
	if (familyIndices.IsComplete(queueFlags))
	{
		std::cerr << "vulkan queue family not find error" << std::endl;
	}
	return;
}

void Graphics::CreateDevice() {

	FindQueueFamilies(physicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { familyIndices.graphicsFamily.value(), familyIndices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;
	deviceFeatures.shaderClipDistance = VK_TRUE;
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::exception("failed to create logical device!");
	}

	GetQueues(familyIndices);

}
void Graphics::CreateMemoryAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = physicalDevice;
	allocatorInfo.device = device;
	allocatorInfo.instance = instance;
	allocatorInfo.vulkanApiVersion = VMA_VULKAN_VERSION == 1001000 ? VK_API_VERSION_1_1 : VK_API_VERSION_1_0;;

	if (VK_KHR_dedicated_allocation_enabled)
	{
		allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
	}
	if (VK_KHR_bind_memory2_enabled)
	{
		allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
	}
#if !defined(VMA_MEMORY_BUDGET) || VMA_MEMORY_BUDGET == 1
	if (VK_EXT_memory_budget_enabled && VK_KHR_get_physical_device_properties2_enabled)
	{
		allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	}
#endif
	ERR_GUARD_VULKAN(vmaCreateAllocator(&allocatorInfo, &allocator));
}
void Graphics::GetQueues(Familys queueFamily) {
	if (queueFlags & VK_QUEUE_GRAPHICS_BIT)
	{
		vkGetDeviceQueue(device, queueFamily.graphicsFamily.value(), 0, &queues.graphicsQueue);
	}

	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		vkGetDeviceQueue(device, queueFamily.computeFamily.value(), 0, &queues.computeQueue);
	}

	if (queueFlags & VK_QUEUE_TRANSFER_BIT)
	{
		vkGetDeviceQueue(device, queueFamily.transferFamily.value(), 0, &queues.transferQueue);
	}

	if (queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
	{
		vkGetDeviceQueue(device, queueFamily.sparseBindingFamily.value(), 0, &queues.sparseBindingQueue);
	}

	vkGetDeviceQueue(device, queueFamily.presentFamily.value(), 0, &queues.presentQueue);
}

void Graphics::CreateSwapChain() {
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { familyIndices.graphicsFamily.value(), familyIndices.presentFamily.value() };

	if (familyIndices.graphicsFamily != familyIndices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	VkResult r = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
	if (r != VK_SUCCESS) {
		throw std::exception("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	std::vector<VkImage> images(imageCount);
	swapChainImages.resize(imageCount);

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());
	//BeginSingleTimedCommandBuffer();
	for (size_t i = 0; i < images.size(); ++i)
	{
		swapChainImages[i].handle = images[i];
		swapChainImages[i].width = width;
		swapChainImages[i].height = height;
		swapChainImages[i].depth = 1;
		swapChainImages[i].format = surfaceFormat.format;
		swapChainImages[i].currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	}
	//EndAndSubmitSingleTimeCommandBuffer();
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

VkSurfaceFormatKHR Graphics::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}
VkPresentModeKHR Graphics::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}
VkExtent2D Graphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		return actualExtent;
	}
	else {
#ifdef GLFW
		int width, height;
		glfwGetFramebufferSize(glfwWindow, &width, &height);
#endif
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		return actualExtent;
	}

}

void Graphics::CreateSwapChainImageViews()
{
	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		//TransitionImageLayout(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		CreateImageView(swapChainImages[i], swapChainImageFormat);
	}
}

void Graphics::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::exception("failed to create synchronization objects for a frame!");
		}
	}
}

void Graphics::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 4> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1000;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1000;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[2].descriptorCount = 1000;
	poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizes[3].descriptorCount = 1000;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1000;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

}

void Graphics::CreateCommandPool()
{
	FindQueueFamilies(physicalDevice);
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = familyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional
	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::exception("failed to create command pool!");
	}
}

void Graphics::Init()
{
	CreateMyWindow();
	CreateInstance();
	CreateDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateDevice();
	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateSyncObjects();
	CreateDescriptorPool();
	CreateCommandPool();
	CreateMemoryAllocator();
	for (int i = 0;i<swapChainImages.size(); i++)
	{
		TransitionImageLayout(swapChainImages[i], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}
}

uint32_t Graphics::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::exception("failed to find suitable memory type!");
	return -1;
}
CommandBuffer Graphics::CreateCommandBuffer(bool oneTimeSubmit, bool begin, bool primary)
{
	CommandBuffer result;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &allocInfo, &result.commandBuffer);
	result.primary = primary;
	result.oneTime = oneTimeSubmit;

	result.beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	result.beginInfo.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT :(primary ? VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	if (begin)
	{
		result.StartCommand();
	}

	return result;
}

void Graphics::BeginSingleTimedCommandBuffer()
{
	assert(singleTimedCommand.commandBuffer == VK_NULL_HANDLE);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &allocInfo, &singleTimedCommand.commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(singleTimedCommand.commandBuffer, &beginInfo);
	return;
}

void Graphics::EndAndSubmitSingleTimeCommandBuffer()
{
	vkEndCommandBuffer(singleTimedCommand.commandBuffer);

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	vkCreateFence(device, &fenceInfo, nullptr, &fence);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &singleTimedCommand.commandBuffer;

	vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, fence);
	vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000);
	ERR_GUARD_VULKAN(vkQueueWaitIdle(queues.graphicsQueue));
	vkDestroyFence(device, fence, nullptr);
	vkFreeCommandBuffers(device, commandPool, 1, &singleTimedCommand.commandBuffer);
	singleTimedCommand.commandBuffer = VK_NULL_HANDLE;
}

GraphicsImage Graphics::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageTiling tiling, VkImageType type, uint32_t layers)

{
	GraphicsImage image = {};
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = type;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = layers;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (layers == 6)
	{
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	VmaAllocationCreateInfo imageAllocCreateInfo = {};
	imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	ERR_GUARD_VULKAN(vmaCreateImage(allocator, &imageInfo, &imageAllocCreateInfo, &image.handle, &image.allocation, nullptr));
	image.width = width;
	image.height = height;
	image.depth = imageInfo.extent.depth;
	image.currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image.format = format;
	image.usage = usage;
	return image;
}

GraphicsImage Graphics::CreateTexture2D(uint32_t sizeX, uint32_t sizeY, void* data)

{
	GraphicsImage image;
	// Create staging buffer.

	const VkDeviceSize imageSize = sizeX * sizeY * 4;

	VkBufferCreateInfo stagingBufInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	stagingBufInfo.size = imageSize;
	stagingBufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingBufAllocCreateInfo = {};
	stagingBufAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	stagingBufAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VkBuffer stagingBuf = VK_NULL_HANDLE;
	VmaAllocation stagingBufAlloc = VK_NULL_HANDLE;
	VmaAllocationInfo stagingBufAllocInfo = {};
	ERR_GUARD_VULKAN(vmaCreateBuffer(allocator, &stagingBufInfo, &stagingBufAllocCreateInfo, &stagingBuf, &stagingBufAlloc, &stagingBufAllocInfo));

	stagingBufAllocInfo.pMappedData = data;

	// No need to flush stagingImage memory because CPU_ONLY memory is always HOST_COHERENT.

	// Create g_hTextureImage in GPU memory.

	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = sizeX;
	imageInfo.extent.height = sizeY;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	VmaAllocationCreateInfo imageAllocCreateInfo = {};
	imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	ERR_GUARD_VULKAN(vmaCreateImage(allocator, &imageInfo, &imageAllocCreateInfo, &image.handle, &image.allocation, &image.allocationInfo));

	// Transition image layouts, copy image.

	BeginSingleTimedCommandBuffer();

	VkImageMemoryBarrier imgMemBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgMemBarrier.subresourceRange.baseMipLevel = 0;
	imgMemBarrier.subresourceRange.levelCount = 1;
	imgMemBarrier.subresourceRange.baseArrayLayer = 0;
	imgMemBarrier.subresourceRange.layerCount = 1;
	imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imgMemBarrier.image = image.handle;
	imgMemBarrier.srcAccessMask = 0;
	imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(
		singleTimedCommand.commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imgMemBarrier);

	VkBufferImageCopy region = {};
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.layerCount = 1;
	region.imageExtent.width = sizeX;
	region.imageExtent.height = sizeY;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(singleTimedCommand.commandBuffer, stagingBuf, image.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imgMemBarrier.image = image.handle;
	imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		singleTimedCommand.commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &imgMemBarrier);

	EndAndSubmitSingleTimeCommandBuffer();

	vmaDestroyBuffer(allocator, stagingBuf, stagingBufAlloc);

	// Create ImageView

	CreateImageView(image, VK_FORMAT_R8G8B8A8_UNORM);

	return image;
}

void Graphics::CreateImageView(GraphicsImage& image, VkFormat format, VkImageAspectFlags flag, VkImageViewType type)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image.handle;
	viewInfo.viewType = type;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = flag;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (type == VK_IMAGE_VIEW_TYPE_CUBE)
	{
		viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		// 6 array layers (faces)
		viewInfo.subresourceRange.layerCount = 6;
		// Set number of mip levels
		viewInfo.subresourceRange.levelCount = 1;
	}
	if (vkCreateImageView(device, &viewInfo, nullptr, &image.imageView) != VK_SUCCESS) {
		throw std::exception("failed to create texture image view!");
	}
}

Buffer Graphics::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags memoryFlag)
{
	Buffer buffer;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo vbAllocCreateInfo = {};
	vbAllocCreateInfo.usage = memoryUsage;
	vbAllocCreateInfo.flags = memoryFlag;
	ERR_GUARD_VULKAN(vmaCreateBuffer(allocator, &bufferInfo, &vbAllocCreateInfo, &buffer.handle, &buffer.allocation, &buffer.allocationInfo));
	buffer.usage = usage;
	buffer.accurateSize = size;
	return buffer;
}




void Graphics::CopyDataToGUPBuffer(void* data, Buffer& buffer, size_t size, size_t offset)
{

	Buffer stageginBuffer = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT);

	memcpy(stageginBuffer.allocationInfo.pMappedData, data, size);

	BeginSingleTimedCommandBuffer();
	CmdCopyBufferToBuffer(singleTimedCommand.commandBuffer, stageginBuffer, buffer);
	EndAndSubmitSingleTimeCommandBuffer();
}
void Graphics::CopyDataToBuffer(void* data, Buffer& buffer, size_t size, size_t offset)
{
	memcpy(buffer.allocationInfo.pMappedData, data, size);
}
void Graphics::CmdCopyBufferToBuffer(VkCommandBuffer cmd, Buffer srcBuffer, Buffer dstBuffer)
{

	VkBufferCopy copyRegion = {};
	copyRegion.size = srcBuffer.allocationInfo.size;
	copyRegion.srcOffset = srcBuffer.allocationInfo.offset;
	copyRegion.dstOffset = dstBuffer.allocationInfo.offset;
	vkCmdCopyBuffer(cmd, srcBuffer.handle, dstBuffer.handle, 1, &copyRegion);
}

void Graphics::DestroyBuffer(Buffer& buffer)
{
	vmaDestroyBuffer(allocator, buffer.handle, buffer.allocation);
}

void Graphics::CreateVertexBuffer(Mesh& mesh)
{

	VkDeviceSize bufferSize = sizeof(glm::vec3) * (mesh.positions.size() + mesh.normals.size() + mesh.tangents.size()) + sizeof(glm::vec2) * mesh.texCoords.size();
	Buffer stagingBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	memcpy(stagingBuffer.allocationInfo.pMappedData, mesh.positions.data(), (size_t)mesh.positions.size() * sizeof(glm::vec3));
	memcpy((char*)stagingBuffer.allocationInfo.pMappedData + mesh.normalOffset, mesh.normals.data(), (size_t)mesh.normals.size() * sizeof(glm::vec3));
	memcpy((char*)stagingBuffer.allocationInfo.pMappedData + mesh.tangentOffset, mesh.tangents.data(), (size_t)mesh.tangents.size() * sizeof(glm::vec3));
	memcpy((char*)stagingBuffer.allocationInfo.pMappedData + mesh.texCoordsOffset, mesh.texCoords.data(), (size_t)mesh.texCoords.size() * sizeof(glm::vec2));

	mesh.vertexBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	BeginSingleTimedCommandBuffer();
	CmdCopyBufferToBuffer(singleTimedCommand.commandBuffer, stagingBuffer, mesh.vertexBuffer);
	EndAndSubmitSingleTimeCommandBuffer();
	DestroyBuffer(stagingBuffer);
}

void Graphics::CreateIndexBuffer(Mesh& mesh)
{
	VkDeviceSize bufferSize = sizeof(uint32_t) *mesh.indexCount;
	Buffer stagingBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	memcpy(stagingBuffer.allocationInfo.pMappedData, mesh.indices.data(), bufferSize);
	
	mesh.indexBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	BeginSingleTimedCommandBuffer();
	CmdCopyBufferToBuffer(singleTimedCommand.commandBuffer, stagingBuffer, mesh.indexBuffer);
	EndAndSubmitSingleTimeCommandBuffer();
	DestroyBuffer(stagingBuffer);

}

VkShaderModule Graphics::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();

	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::exception("failed to create shader module!");
	}
	return shaderModule;
}

void Graphics::TransitionImageLayout(GraphicsImage& image, VkImageLayout newLayout, VkImageSubresourceRange subSourceRange)
{
	BeginSingleTimedCommandBuffer();
	singleTimedCommand.TransitionImageLayout(image, newLayout, subSourceRange);
	EndAndSubmitSingleTimeCommandBuffer();
}

uint32_t Graphics::DrawFrameAndPresent(std::vector<CommandBuffer*> commandbuffers, GraphicsImage& renderTarget)
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, (std::numeric_limits<uint64_t>::max)(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return -1;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::exception("failed to acquire swap chain image!");
	}
	for (auto& command : commandbuffers)
	{
		command->EndCommand();
	}

	//need to wait queue idle before update data
	vkQueueWaitIdle(queues.graphicsQueue);
	for (auto& f : bufferUpdateCallbacks)
	{
		f();
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	std::vector<VkCommandBuffer> cmds;
	cmds.resize(commandbuffers.size());
	for (int i = 0; i < commandbuffers.size(); i++)
	{
		cmds[i] = commandbuffers[i]->commandBuffer;
	}

	
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = cmds.size();
	submitInfo.pCommandBuffers = cmds.data();
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::exception("failed to submit draw command buffer!");
	}
	VkSemaphore sems[2] = { imageAvailableSemaphores[currentFrame]  ,renderFinishedSemaphores[currentFrame] };
	VkImageCopy copy = {};
	copy.dstOffset = {};
	copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.dstSubresource.baseArrayLayer = 0;
	copy.dstSubresource.layerCount = 1;
	copy.dstSubresource.mipLevel = 0;
	copy.srcOffset = {};
	copy.srcSubresource = copy.dstSubresource;
	copy.extent = { swapChainExtent.width,swapChainExtent.height,0 };
	BeginSingleTimedCommandBuffer();
	singleTimedCommand.CopyImageToImage( swapChainExtent.width, swapChainExtent.height, renderTarget, swapChainImages[imageIndex]);
	
	vkQueueWaitIdle(queues.graphicsQueue);
	EndAndSubmitSingleTimeCommandBuffer();
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 2;
	presentInfo.pWaitSemaphores = sems;
	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	//imageIndex = (imageIndex - 1) % 3;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	result = vkQueuePresentKHR(queues.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::exception("failed to present swap chain image!");
	}

	for (auto& command : commandbuffers)
	{
		if (command->oneTime)
		{
			vkFreeCommandBuffers(device, commandPool, 1, &command->commandBuffer);
			command->commandBuffer = VK_NULL_HANDLE;
		}
	}
	vkQueueWaitIdle(queues.presentQueue);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	return imageIndex;
}
void Graphics::RecreateSwapChain()
{
}
;


