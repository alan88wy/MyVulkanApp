#include "Window.h"

namespace mge {
	MgeWindow::MgeWindow()
	{
		width = 800;
		height = 600;
		windowName = "My Game Engine";

	}

	MgeWindow::MgeWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }
	{
		width = w;
		height = h;
		windowName = name;
	}

	int MgeWindow::initWindow()
	{

		// Initialize glfw

		if (!glfwInit())
		{
			printf("Error Initialising GLFW");
			glfwTerminate();
			return EXIT_FAILURE;
		}

		// Setup GLFW Windows Properties

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // Disable window resize

		mainWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

		if (!mainWindow)
		{
			printf("Error creating GLFW window!");
			glfwTerminate();
			return EXIT_FAILURE;
		}

		glfwSetWindowUserPointer(mainWindow, this);
		glfwSetFramebufferSizeCallback(mainWindow, frameBufferResizeCallback);


		return EXIT_SUCCESS;
	}

	void MgeWindow::frameBufferResizeCallback( GLFWwindow *window, int width, int height)
	{
		auto app = reinterpret_cast<MgeWindow*>(glfwGetWindowUserPointer(window));
		app->frameBufferResize = true;
	}

	void MgeWindow::initVulkan()
	{
		createInstance();

		setupDebugMessenger();

		createSurface();

		pickPhysicalDevice();

		createLogicalDevice();

		createSwapChain();

		createImageViews();

		createRenderPass();

		createGraphicsPipeline();

		createFrameBuffers();

		// GPU
		createCommandPool();

		createCommandBuffers();

		createSyncObjects();
	}

	void MgeWindow::mainLoop()
	{

		while (!getShouldClose())
		{
			glfwPollEvents();

			drawFrame();
		}

		vkDeviceWaitIdle(device);  // Need to do this. Even after the while loop finished, the drawing could still going on.

	}

	// Vulkan Initialization

	// Create Instance

	void MgeWindow::createInstance()
	{

		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// Now, to create an instance we'll first have to fill in a struct with some information about our application. 

		VkApplicationInfo appInfo{};

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "SoSy Game Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		// A lot of information in Vulkan is passed through structs instead of function parameters and we'll have to fill 
		// in one more struct to provide sufficient information for creating an instance. This next struct is not optional 
		// and tells the Vulkan driver which global extensions and validation layers we want to use. Global here means that 
		// they apply to the entire program and not a specific device, which will become clear in the next few chapters.

		VkInstanceCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();

		createInfo.enabledExtensionCount = static_cast<unsigned int> (extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);

			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// Create Instance

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vulkan instance!");
		}


		/*


		glm::mat4 matrix;
		glm::vec4 vec;
		auto test = matrix * vec;

		*/

	}

	bool MgeWindow::checkValidationLayerSupport() {

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> MgeWindow::getRequiredExtensions()
	{

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void MgeWindow::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	// Setup Debugger

	void MgeWindow::setupDebugMessenger()
	{
		if (!enableValidationLayers) {
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT(createInfo);

		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to setup debug messenger");
		}
	}

	VkResult MgeWindow::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void MgeWindow::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	// Create Surface

	void MgeWindow::createSurface() {

		if (glfwCreateWindowSurface(instance, mainWindow, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}


	void MgeWindow::pickPhysicalDevice()
	{
		/*
		* Listing the graphics cards is very similar to listing extensions
		* and starts with querying just the number.
		*/

		unsigned int deviceCount = 0;

		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		// If there are 0 devices with Vulkan support then there is no point going further.

		if (deviceCount == 0)
		{
			throw std::runtime_error("Failed to find any GPUs with Vulkan support!");
		}

		// Otherwise we can now allocate an array to hold all of the VkPhysicalDevice handles.

		std::vector<VkPhysicalDevice> devices(deviceCount);

		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// And we'll check if any of the physical devices meet the requirements that we'll add to that function.
		for (const auto& device : devices)
		{
			if (isDeviceSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Failed to find a suitable GPU");
		}

		/*
		* Instead of just checking if a device is suitable or not and going with the first one,
		* you could also give each device a score and pick the highest one. That way you could favor
		* a dedicated graphics card by giving it a higher score, but fall back to an integrated GPU if
		* that's the only available one. You could implement something like that as follows:
		*/

		// Use an ordered map to automatically sort candidates by increasing score
		/*
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices) {
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0) {
			physicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		*/

	}

	bool MgeWindow::isDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;

		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isCompleted() && extensionsSupported && swapChainAdequate;

		/*

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader;

		*/


	}

	bool MgeWindow::checkDeviceExtensionSupport(VkPhysicalDevice device) {

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void MgeWindow::createLogicalDevice()
	{

		/*
		* The creation of a logical device involves specifying a bunch of details in structs again,
		of which the first one will be VkDeviceQueueCreateInfo. This structure describes the number
		of queues we want for a single queue family. Right now we're only interested in a queue with
		graphics capabilities.
		*/

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<unsigned int> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		/*
		* Vulkan lets you assign priorities to queues to influence the scheduling of command buffer
		execution using floating point numbers between 0.0 and 1.0. This is required even if there
		is only a single queue:
		*/

		float queuePriority = 1.0f;

		for (unsigned int queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}


		/*
		Specifying used device features

		These are the features that we queried support for with vkGetPhysicalDeviceFeatures.
		*/

		VkPhysicalDeviceFeatures deviceFeatures{};

		/*
		* Creating the logical device
		* With the previous two structures in place, we can start filling in the main VkDeviceCreateInfo structure.
		*/

		VkDeviceCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// First add pointers to the queue creation info and device features structs:

		createInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		/*
		* specify extensions and validation layers. The difference is that these are device specific this time.
		* An example of a device specific extension is VK_KHR_swapchain, which allows you to present rendered
		* images from that device to windows.
		*/

		createInfo.enabledExtensionCount = static_cast<unsigned int> (deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		/*
		* That's it, we're now ready to instantiate the logical device with a call to the appropriately named
		* vkCreateDevice function.
		*/

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device!");
		}

		/*
		* We can use the vkGetDeviceQueue function to retrieve queue handles for each queue family.
		* The parameters are the logical device, queue family, queue index and a pointer to the variable to store
		* the queue handle in. Because we're only creating a single queue from this family, we'll simply use index 0.
		*/

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	}

	MgeWindow::QueueFamilyIndices MgeWindow::findQueueFamilies(VkPhysicalDevice device) const
	{
		/*
		* in Vulkan, anything from drawing to uploading textures, requires commands
		* to be submitted to a queue. There are different types of queues that originate
		* from different queue families and each family of queues allows only a subset of commands.
		* For example, there could be a queue family that only allows processing of compute commands
		* or one that only allows memory transfer related commands.
		*
		* We need to check which queue families are supported by the device and which one of these
		* supports the commands that we want to use. For that purpose we'll add a new function
		* findQueueFamilies that looks for all the queue families we need.
		*/

		QueueFamilyIndices indices;

		/*
		* The process of retrieving the list of queue families is exactly
		* what you expect and uses vkGetPhysicalDeviceQueueFamilyProperties:
		*/

		unsigned int queueFamilyCount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		/*
		* The VkQueueFamilyProperties struct contains some details about the queue family, including
		* the type of operations that are supported and the number of queues that can be created based
		* on that family. We need to find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT.
		*/

		int i = 0;

		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;

			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.isCompleted())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	// Create Swapchain

	void MgeWindow::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		// createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	MgeWindow::SwapChainSupportDetails MgeWindow::querySwapChainSupport(VkPhysicalDevice device) {
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

	VkSurfaceFormatKHR MgeWindow::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR MgeWindow::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D MgeWindow::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(mainWindow, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	// Create ImageView

	void MgeWindow::createImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());

		for (unsigned long long i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create Image View ");
			}

		}
	}

	// Create Graphics Pipeline

	std::vector<char> MgeWindow::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error(" Failed to open file : " + filename);
		}

		unsigned long long fileSize = (unsigned long long) file.tellg();

		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;

	}

	void MgeWindow::createRenderPass()
	{
		VkAttachmentDescription colorAttachment{};

		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};

		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};

		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};

		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency; 

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass");
		}

	}

	void MgeWindow::createGraphicsPipeline()
	{
		auto vertShaderCode = ReadFile(vertShaderFile);
		auto fragShaderCode = ReadFile(fragShaderFile);

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// Vulkan Graphics Pipeline Layout

		// 1. Create Vertex Input

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		// 2. Input Assembly

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// 3. Viewport and Scissor

		VkViewport viewport{};

		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};

		scissor.offset = { 0,0 };
		scissor.extent = swapChainExtent;

		// Dynamic State

		VkPipelineViewportStateCreateInfo viewportState{};

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// Rasterization

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		// Multi Sampling

		VkPipelineMultisampleStateCreateInfo multiSampling{};
		multiSampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSampling.sampleShadingEnable = VK_FALSE;
		multiSampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// Blending

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Pipeline Layout");
		}

		// end of pipeline

		// Create Graphics Pipeline

		VkGraphicsPipelineCreateInfo pipelineInfo{};

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multiSampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	void MgeWindow::createFrameBuffers()
	{
		swapChainFrameBuffers.resize(swapChainImageViews.size());

		for (unsigned long long i = 0; i < swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] =
			{
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo frameBufferInfo{};

			frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferInfo.renderPass = renderPass;
			frameBufferInfo.attachmentCount = 1;
			frameBufferInfo.pAttachments = attachments;
			frameBufferInfo.width = swapChainExtent.width;
			frameBufferInfo.height = swapChainExtent.height;
			frameBufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create frame buffer!");
			}
		}
	}

	// GPU
	void MgeWindow::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool!");
		}
	}

	void MgeWindow::createCommandBuffers()
	{
		commandBuffers.resize(swapChainFrameBuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<unsigned int>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (unsigned long long i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{ };
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed t o begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapChainFrameBuffers[i];
			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = swapChainExtent;

			VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			// bind graphic pipeline
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer");
			}

		}
	}

	
	void MgeWindow::createSyncObjects()
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (unsigned long long i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}

	}

	void MgeWindow::drawFrame()
	{
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		unsigned int imageIndex;

		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap image!");
		}

			
		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}

		// match inflight images to the current frame
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemephores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemephores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer");
		}

		// submit result to swapchain and be able to show on screen

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		// Speficy swapchain to present images to and index for each swapchain
		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResize)
		{
			frameBufferResize = false;

			recreateSwapChain();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to present swap image!");
		}

		// move to next frame
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	}
	
	// end of GPU

	VkShaderModule MgeWindow::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const unsigned int*>(code.data());

		VkShaderModule shaderModule;

		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}

		return shaderModule;

	}

	// Other funcions

	VKAPI_ATTR VkBool32 VKAPI_CALL MgeWindow::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData, void* pUserData)
	{
		std::cerr << "Validation Layer : " << pCallBackData->pMessage << std::endl;

		return VK_FALSE;
	};

	int MgeWindow::rateDeviceSuitability(VkPhysicalDevice device) {

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Application can't function without geometry shaders
		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}


	// Cleaning Up

	void MgeWindow::cleanUpSwapChain()
	{
		for (auto frameBuffer : swapChainFrameBuffers)
		{
			vkDestroyFramebuffer(device, frameBuffer, nullptr);
		}

		vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(commandBuffers.size()), commandBuffers.data());

		vkDestroyPipeline(device, graphicsPipeline, nullptr);

		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	void MgeWindow::cleanUp()
	{
		cleanUpSwapChain();

		for (unsigned long long i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroyDevice(device, nullptr);

		if (enableValidationLayers)
		{
			destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface, nullptr);

		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(mainWindow);

		glfwTerminate();

	}

	void MgeWindow::recreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(mainWindow, &width, &height);

		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(mainWindow, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);

		cleanUpSwapChain();

		// re-create swapchain
		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		createCommandBuffers();

		imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	}

}