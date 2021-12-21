#include "Vulkanengine.h"

std::shared_ptr<VulkanEngine> EnginePtr::enginePtr = nullptr;

VulkanEngine::VulkanEngine()
{
}

VulkanEngine::VulkanEngine(std::shared_ptr<VulkanWindow> window)
{
	ScreenResoulation = glm::ivec2(WindowPtr::GetWindowPtr()->GetWindowWidth(), WindowPtr::GetWindowPtr()->GetWindowHeight());

	ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

	DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
	//DeviceExtensions.emplace_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
	//DeviceExtensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);
	DeviceExtensions.emplace_back(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME);

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

	std::vector<VkValidationFeatureEnableEXT> enabledList = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT, VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT, VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT };
	std::vector<VkValidationFeatureDisableEXT> disabledList = {
	VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT, VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT,
		VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT, VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT };

	VkValidationFeaturesEXT ValidationFeatures{};
	ValidationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	ValidationFeatures.disabledValidationFeatureCount = static_cast<uint32_t>(enabledList.size());
	ValidationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(disabledList.size());
	ValidationFeatures.pEnabledValidationFeatures = enabledList.data();
	ValidationFeatures.pDisabledValidationFeatures = disabledList.data();
	ValidationFeatures.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugInfo;

	VulkanCreateInfo.enabledLayerCount = static_cast<unsigned int>(ValidationLayers.size());
	VulkanCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
	VulkanCreateInfo.pNext = &ValidationFeatures;
	//VulkanCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugInfo;

#endif

	if (vkCreateInstance(&VulkanCreateInfo, nullptr, &Instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
	VulkanDebug.SetUpDebugger(Instance);

	if (glfwCreateWindowSurface(Instance, window->GetWindowPtr(), nullptr, &Surface) != VK_SUCCESS) {
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
			MaxSampleCount = GetMaxUsableSampleCount(gpudevice);
			break;
		}
	}

	if (PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	SetUpDeviceFeatures(window->GetWindowPtr());
	SwapChain = VulkanSwapChain(window->GetWindowPtr(), Device, PhysicalDevice, Surface);

	InitializeCommandPool();
	InitializeSyncObjects();

	VulkanPtr::SetVulkanInstance(Instance);
	VulkanPtr::SetDeviceInstance(Device);
	VulkanPtr::SetPhysicalDevice(PhysicalDevice);
	VulkanPtr::SetWindowSurface(Surface);
	VulkanPtr::SetGraphicsQueue(GraphicsQueue);
	VulkanPtr::SetPresentQueue(PresentQueue);
	VulkanPtr::SetCommandPool(CommandPool);

	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(Device, "vkGetBufferDeviceAddressKHR"));
	vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(Device, "vkCmdBuildAccelerationStructuresKHR"));
	vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(Device, "vkBuildAccelerationStructuresKHR"));
	vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(Device, "vkCreateAccelerationStructureKHR"));
	vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(Device, "vkDestroyAccelerationStructureKHR"));
	vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(Device, "vkGetAccelerationStructureBuildSizesKHR"));
	vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(Device, "vkGetAccelerationStructureDeviceAddressKHR"));
	vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(Device, "vkCmdTraceRaysKHR"));
	vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(Device, "vkGetRayTracingShaderGroupHandlesKHR"));
	vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(Device, "vkCreateRayTracingPipelinesKHR"));
}

VulkanEngine::~VulkanEngine()
{

}

uint32_t VulkanEngine::GenerateObjID()
{
	NextObjID++;
	return NextObjID;
}

uint32_t VulkanEngine::GenerateID()
{
	NextID++;
	return NextID;
}

float VulkanEngine::VulkanTimer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto  currentTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
}

VkSampleCountFlagBits VulkanEngine::GetMaxUsableSampleCount(VkPhysicalDevice GPUDevice)
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(GPUDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
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

void VulkanEngine::SetUpDeviceFeatures(GLFWwindow* window)
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

	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddresFeatures{};
	BufferDeviceAddresFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	BufferDeviceAddresFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR RayTracingPipelineFeatures{};
	RayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	RayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
	RayTracingPipelineFeatures.pNext = &BufferDeviceAddresFeatures;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR RayTracingPipelineProperties = {};
	RayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	RayTracingPipelineProperties.pNext = &RayTracingPipelineFeatures;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR RayTracinDeviceProperties = {};
	RayTracinDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	RayTracinDeviceProperties.pNext = &RayTracingPipelineProperties;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeatures{};
	AccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	AccelerationStructureFeatures.accelerationStructure = VK_TRUE;
	AccelerationStructureFeatures.pNext = &RayTracinDeviceProperties;

	VkPhysicalDeviceDescriptorIndexingFeatures PhysicalDeviceDescriptorIndexingFeatures{};
	PhysicalDeviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	PhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
	PhysicalDeviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	PhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
	PhysicalDeviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
	PhysicalDeviceDescriptorIndexingFeatures.pNext = &AccelerationStructureFeatures;

	VkPhysicalDeviceRobustness2FeaturesEXT  PhysicalDeviceRobustness2Features{};
	PhysicalDeviceRobustness2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
	PhysicalDeviceRobustness2Features.nullDescriptor = VK_TRUE;
	PhysicalDeviceRobustness2Features.pNext = &PhysicalDeviceDescriptorIndexingFeatures;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;
	deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
	deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	deviceFeatures.sampleRateShading = VK_TRUE;

	VkPhysicalDeviceFeatures2 deviceFeatures2{};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.features = deviceFeatures;
	deviceFeatures2.pNext = &PhysicalDeviceRobustness2Features;

	VkPhysicalDeviceVulkan11Features PhysicalDeviceVulkan11Features{};
	PhysicalDeviceVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	PhysicalDeviceVulkan11Features.multiview = VK_TRUE;
	PhysicalDeviceVulkan11Features.pNext = &deviceFeatures2;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	createInfo.pNext = &PhysicalDeviceVulkan11Features;

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
	std::set<std::string> extensionsNotSupported = CheckDeviceExtensionSupport(GPUDevice);
	VkPhysicalDeviceFeatures supportedFeatures = GetPhysicalDeviceFeatures(GPUDevice);
	std::vector<VkSurfaceFormatKHR> SurfaceFormatList = GetSurfaceFormatList(GPUDevice);
	std::vector<VkPresentModeKHR> PresentModeList = GetPresentModeList(GPUDevice, Surface);

	if (GraphicsFamily != -1 &&
		PresentFamily != -1 &&
		SurfaceFormatList.size() != 0 &&
		PresentModeList.size() != 0 &&
		supportedFeatures.samplerAnisotropy)
	{
		VulkanCompatible = true;
	}

	RayTracingFeatureCompatible = CheckRayTracingCompatiblity(GPUDevice);

	return VulkanCompatible;
}

std::vector<const char*> VulkanEngine::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	return extensions;
}

std::set<std::string> VulkanEngine::CheckDeviceExtensionSupport(VkPhysicalDevice GPUDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(GPUDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(GPUDevice, nullptr, &extensionCount, availableExtensions.data());
	for (auto availableExtension : availableExtensions)
	{
		FeatureList.emplace_back(availableExtension.extensionName);
	}

	std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	for (auto extension : requiredExtensions)
	{
		std::cout << extension << " is Required." << std::endl;
	}

	return requiredExtensions;
}

bool VulkanEngine::CheckRayTracingCompatiblity(VkPhysicalDevice GPUDevice)
{
	if (!RayTracingFeatureCompatible)
	{
		VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeatures{};
		AccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR RayTracingPipelineFeatures{};
		RayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		RayTracingPipelineFeatures.pNext = &AccelerationStructureFeatures;

		VkPhysicalDeviceFeatures2 DeviceFeatures2{};
		DeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		DeviceFeatures2.pNext = &RayTracingPipelineFeatures;
		vkGetPhysicalDeviceFeatures2(GPUDevice, &DeviceFeatures2);

		if (RayTracingPipelineFeatures.rayTracingPipeline == VK_TRUE &&
			AccelerationStructureFeatures.accelerationStructure == VK_TRUE)
		{
			if (std::find(FeatureList.begin(), FeatureList.end(), VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) != FeatureList.end() &&
				std::find(FeatureList.begin(), FeatureList.end(), VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)   != FeatureList.end())
			{
				RayTracingFeatureCompatible = true;
				DeviceExtensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
				DeviceExtensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
			}
			else
			{
				RayTracingFeatureCompatible = false;
			}
		}
		else
		{
			std::cout << "GPU/MotherBoard isn't ray tracing compatible." << std::endl;
		}
	}

	return RayTracingFeatureCompatible;
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

	if (vkCreateCommandPool(Device, &poolInfo, nullptr, &CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

void VulkanEngine::InitializeSyncObjects()
{
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	AcquireImageSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	PresentImageSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreTypeCreateInfo timelineCreateInfo;
	timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	timelineCreateInfo.pNext = NULL;
	timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_BINARY;
	timelineCreateInfo.initialValue = 0;

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = &timelineCreateInfo;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &AcquireImageSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &PresentImageSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(Device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void VulkanEngine::Destroy()
{
	SwapChain.Destroy(Device);

	vkDestroyCommandPool(Device, CommandPool, nullptr);
	CommandPool = VK_NULL_HANDLE;

	for (size_t x = 0; x < MAX_FRAMES_IN_FLIGHT; x++)
	{
		vkDestroySemaphore(Device, AcquireImageSemaphores[x], nullptr);
		vkDestroySemaphore(Device, PresentImageSemaphores[x], nullptr);
		vkDestroyFence(Device, inFlightFences[x], nullptr);

		AcquireImageSemaphores[x] = VK_NULL_HANDLE;
		PresentImageSemaphores[x] = VK_NULL_HANDLE;
		inFlightFences[x] = VK_NULL_HANDLE;
	}

	vkDestroyDevice(Device, nullptr);
	Device = VK_NULL_HANDLE;

	VulkanDebug.CleanUp(Instance);

	vkDestroySurfaceKHR(Instance, Surface, nullptr);
	vkDestroyInstance(Instance, nullptr);
}

VkCommandBuffer  VulkanEngine::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(Device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

VkCommandBuffer  VulkanEngine::beginSingleTimeCommands(VkCommandPool& commandPool) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
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

	vkFreeCommandBuffers(Device, CommandPool, 1, &commandBuffer);
}

void  VulkanEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool& commandPool) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQueue);

	vkFreeCommandBuffers(Device, commandPool, 1, &commandBuffer);
}

uint32_t VulkanEngine::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

uint64_t VulkanEngine::GetBufferDeviceAddress(VkBuffer buffer)
{
	VkBufferDeviceAddressInfoKHR BufferDevice{};
	BufferDevice.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	BufferDevice.buffer = buffer;
	return vkGetBufferDeviceAddressKHR(Device, &BufferDevice);
}

uint32_t VulkanEngine::GetAlignedSize(uint32_t value, uint32_t alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

void VulkanEngine::Mat4Logger(std::string name, glm::mat4 matrix)
{
	std::cout << name << std::endl;
	std::cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3] << std::endl;
	std::cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3] << std::endl;
	std::cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3] << std::endl;
	std::cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[3][3] << std::endl;
	std::cout << std::endl << std::endl;
}