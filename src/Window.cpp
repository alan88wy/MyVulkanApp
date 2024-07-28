#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}

int Window::InitWindow()
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mainWindow = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);

	if (!mainWindow)
	{
		printf("Error creating GLFW window!");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Window::SetupDebugMessenger()
{
	if (!enableValidationLayers) {
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))
	{
		throw std::runtime_error("Failed to setup debug messenger");
	}
}

std::vector<const char *> Window::GetRequiredExtensions()
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

void Window::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

void Window::InitVulkan()
{
	CreateInstance();

	SetupDebugMessenger();

	 CreateSurface();

	PickPhysicalDevice();

	CreateLogicalDevice();

}

void Window::PickPhysicalDevice()
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
	for (const auto& device: devices)
	{
		if (IsDeviceSuitable(device))
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

int Window::RateDeviceSuitability(VkPhysicalDevice device) {
	
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

bool Window::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return indices.IsCompleted();

	/*

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;
	
	*/


}

Window::QueueFamilyIndices Window::FindQueueFamilies(VkPhysicalDevice device)
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

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data() );

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

		if (indices.IsCompleted())
		{
			break;
		}

		i++;
	}

	return indices;
}

void Window::CreateLogicalDevice()
{

	/*
	* The creation of a logical device involves specifying a bunch of details in structs again, 
	of which the first one will be VkDeviceQueueCreateInfo. This structure describes the number 
	of queues we want for a single queue family. Right now we're only interested in a queue with 
	graphics capabilities.
	*/

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<unsigned int> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();

	/*
	* The currently available drivers will only allow you to create a small number of queues for 
	each queue family and you don't really need more than one. That's because you can create all 
	of the command buffers on multiple threads and then submit them all at once on the main thread 
	with a single low-overhead call.
	*/

	queueCreateInfo.queueCount = 1;

	/*
	* Vulkan lets you assign priorities to queues to influence the scheduling of command buffer 
	execution using floating point numbers between 0.0 and 1.0. This is required even if there 
	is only a single queue:
	*/

	float queuePriority = 1.0f;

	for (unsigned int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	 queueCreateInfo.pQueuePriorities = &queuePriority;

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
	//createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	// First add pointers to the queue creation info and device features structs:

	/*createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;*/

	createInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	/*
	* specify extensions and validation layers. The difference is that these are device specific this time.
	* An example of a device specific extension is VK_KHR_swapchain, which allows you to present rendered 
	* images from that device to windows.
	*/

	createInfo.enabledExtensionCount = 0;

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

void Window::CreateSurface() {

	 /*VkWin32SurfaceCreateInfoKHR createInfo{};
	 createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	 createInfo.hwnd = glfwGetWin32Window(mainWindow);
	 createInfo.hinstance = GetModuleHandle(nullptr);*/

	if (glfwCreateWindowSurface(instance, mainWindow, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

void Window::CreateInstance()
{
	//
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested but not available");
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

	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<unsigned int> (extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);

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

void Window::MainLoop()
{

	while (!GetShouldClose())
	{
		glfwPollEvents();
	}
}

void Window::CleanUp()
{

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(mainWindow);

	glfwTerminate();




}

VkResult Window::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
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

void Window::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "VkDestroyUtilsMessangerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

bool Window::CheckValidationLayerSupport() {

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

Window::~Window()
{
}