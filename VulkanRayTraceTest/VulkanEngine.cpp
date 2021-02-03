#include "VulkanEngine.h"
#include <stdexcept>
#include <set>

VulkanEngine::VulkanEngine()
{
}

VulkanEngine::VulkanEngine(GLFWwindow* window)
{
	ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

	DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

	VkApplicationInfo VulkanInfo = {};
	VulkanInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	VulkanInfo.pApplicationName = "Vulkan Graphics";
	VulkanInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	VulkanInfo.pEngineName = "No Engine";
	VulkanInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	VulkanInfo.apiVersion = VK_API_VERSION_1_2;

	std::vector<const char*> ExtensionList = getRequiredExtensions();
	VkInstanceCreateInfo VulkanCreateInfo = {};
	VulkanCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	VulkanCreateInfo.pApplicationInfo = &VulkanInfo;
	VulkanCreateInfo.enabledExtensionCount = static_cast<uint32_t>(ExtensionList.size());
	VulkanCreateInfo.ppEnabledExtensionNames = ExtensionList.data();

#ifdef NDEBUG
	VulkanCreateInfo.enabledLayerCount = 0;
	VulkanCreateInfo.pNext = nullptr;
#else
	VkDebugUtilsMessengerCreateInfoEXT DebugInfo;
	VulkanDebug.CreateDebugMessengerInfo(DebugInfo);

	VulkanCreateInfo.enabledLayerCount = static_cast<unsigned int>(ValidationLayers.size());
	VulkanCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
	VulkanCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugInfo;
#endif

	if (vkCreateInstance(&VulkanCreateInfo, nullptr, &Instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

	VulkanDebug.SetUpDebugger(Instance);

	if (glfwCreateWindowSurface(Instance, window, nullptr, &Surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

	for (const auto& gpudevice : devices) {
		if (isDeviceSuitable(gpudevice))
		{
			PhysicalDevice = gpudevice;
			break;
		}
	}

	if (PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	SetUPDeviceFeatures(window);
	SwapChain = VulkanSwapChain(window, Device, PhysicalDevice, Surface);

	InitializeCommandPool();
	InitializeSyncObjects();
}

VulkanEngine::~VulkanEngine()
{

}

void VulkanEngine::GetInstanceLayerProperties()
{
	uint32_t LayerCount;
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	VulkanLayers.resize(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, VulkanLayers.data());
}

void VulkanEngine::GetPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

	for (const auto& gpudevice : devices) {
		if (isDeviceSuitable(gpudevice))
		{
			PhysicalDevice = gpudevice;
			break;
		}
	}
}

void VulkanEngine::FindQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			GraphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &presentSupport);

		if (presentSupport) {
			PresentFamily = i;
		}

		if (GraphicsFamily != -1 &&
			PresentFamily != -1)
		{
			break;
		}

		i++;
	}
}

void VulkanEngine::SetUPDeviceFeatures(GLFWwindow* window)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { static_cast<uint32_t>(GraphicsFamily), static_cast<uint32_t>(PresentFamily) };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;

	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddresFeatures{};
	BufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	BufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeatures IndexFeatures{};
	IndexFeatures.runtimeDescriptorArray = VK_TRUE;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR RayTracingPipelineFeatures{};
	RayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	RayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	RayTracingPipelineFeatures.pNext = &BufferDeviceAddresFeatures;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeatures{};
	AccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	AccelerationStructureFeatures.accelerationStructure = VK_TRUE;
	AccelerationStructureFeatures.pNext = &RayTracingPipelineFeatures;

	VkPhysicalDeviceDescriptorIndexingFeatures PhysicalDeviceDescriptorIndexingFeatures{};
	PhysicalDeviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	PhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
	PhysicalDeviceDescriptorIndexingFeatures.pNext = &AccelerationStructureFeatures;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR RayTracingPipelineProperties = {};
	RayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR RayTracinDeviceProperties = {};
	RayTracinDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	RayTracinDeviceProperties.pNext = &RayTracingPipelineProperties;

	VkPhysicalDeviceFeatures2 deviceFeatures2{};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.features = deviceFeatures;
	deviceFeatures2.pNext = &PhysicalDeviceDescriptorIndexingFeatures;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	createInfo.pNext = &deviceFeatures2;

#ifdef NDEBUG
	createInfo.enabledLayerCount = 0;
#else
	createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
	createInfo.ppEnabledLayerNames = ValidationLayers.data();
#endif

	if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(Device, GraphicsFamily, 0, &GraphicsQueue);
	vkGetDeviceQueue(Device, PresentFamily, 0, &PresentQueue);
}

bool VulkanEngine::isDeviceSuitable(VkPhysicalDevice GPUDevice)
{
	FindQueueFamilies(GPUDevice, Surface);
	bool extensionsSupported = checkDeviceExtensionSupport(GPUDevice);
	VkPhysicalDeviceFeatures supportedFeatures = GetPhysicalDeviceFeatures(GPUDevice);
	std::vector<VkSurfaceFormatKHR> SurfaceFormatList = GetSurfaceFormatList(GPUDevice);
	std::vector<VkPresentModeKHR> PresentModeList = GetPresentModeList(GPUDevice, Surface);

	return GraphicsFamily != -1 &&
		PresentFamily != -1 &&
		extensionsSupported &&
		SurfaceFormatList.size() != 0 &&
		PresentModeList.size() != 0 &&
		supportedFeatures.samplerAnisotropy;
}

std::vector<const char*> VulkanEngine::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	return extensions;
}

bool VulkanEngine::checkDeviceExtensionSupport(VkPhysicalDevice GPUDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(GPUDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(GPUDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

VkPhysicalDeviceFeatures VulkanEngine::GetPhysicalDeviceFeatures(VkPhysicalDevice GPUDevice)
{
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(GPUDevice, &PhysicalDeviceFeatures);
	return PhysicalDeviceFeatures;
}

std::vector<VkSurfaceFormatKHR> VulkanEngine::GetSurfaceFormatList(VkPhysicalDevice GPUDevice)
{
	uint32_t GPUSurfaceFormatCount;
	std::vector<VkSurfaceFormatKHR> GPUSwapChainFormatCapabilities;

	vkGetPhysicalDeviceSurfaceFormatsKHR(GPUDevice, Surface, &GPUSurfaceFormatCount, nullptr);
	if (GPUSurfaceFormatCount != 0)
	{
		GPUSwapChainFormatCapabilities.resize(GPUSurfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(GPUDevice, Surface, &GPUSurfaceFormatCount, GPUSwapChainFormatCapabilities.data());
	}

	return GPUSwapChainFormatCapabilities;
}

std::vector<VkPresentModeKHR> VulkanEngine::GetPresentModeList(VkPhysicalDevice GPUDevice, VkSurfaceKHR Surface)
{
	uint32_t GPUPresentModeCount;
	std::vector<VkPresentModeKHR> GPUPresentModesList;

	vkGetPhysicalDeviceSurfacePresentModesKHR(GPUDevice, Surface, &GPUPresentModeCount, nullptr);
	if (GPUPresentModeCount != 0)
	{
		GPUPresentModesList.resize(GPUPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(GPUDevice, Surface, &GPUPresentModeCount, GPUPresentModesList.data());
	}

	return GPUPresentModesList;
}

void VulkanEngine::InitializeCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = GraphicsFamily;

	if (vkCreateCommandPool(Device, &poolInfo, nullptr, &RenderCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

void VulkanEngine::InitializeSyncObjects()
{
	vulkanSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(SwapChain.GetSwapChainImageCount(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &vulkanSemaphores[i].ImageAcquiredSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &vulkanSemaphores[i].RenderCompleteSemaphore) != VK_SUCCESS ||
			vkCreateFence(Device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}


VkCommandBuffer  VulkanEngine::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = RenderCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(Device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void  VulkanEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQueue);

	vkFreeCommandBuffers(Device, RenderCommandPool, 1, &commandBuffer);
}