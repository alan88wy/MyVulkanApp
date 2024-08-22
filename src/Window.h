#pragma once

// #define NDEBUG

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
#include <fstream>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace mge {
	class MgeWindow
	{
	public:
		MgeWindow();

		MgeWindow(int w, int h, std::string name);

		int initWindow();

		void initVulkan();

		void mainLoop();

		void cleanUp();

		// ~Window();

	private:

		// Variable & struc Declaration

		GLFWwindow* mainWindow;

		int width, height;

		std::string windowName;

		VkInstance instance;

		VkDebugUtilsMessengerEXT debugMessenger;

		/*
		* Window surface
		*
		* Since Vulkan is a platform agnostic API, it can not interface directly with the window system
		* on its own. To establish the connection between Vulkan and the window system to present results
		* to the screen, we need to use the WSI (Window System Integration) extensions. In this chapter we'll
		* discuss the first one, which is VK_KHR_surface. It exposes a VkSurfaceKHR object that represents an
		* abstract type of surface to present rendered images to. The surface in our program will be backed by
		* the window that we've already opened with GLFW.
		*
		* The VK_KHR_surface extension is an instance level extension and we've actually already enabled it,
		* because it's included in the list returned by glfwGetRequiredInstanceExtensions. The list also includes
		* some other WSI extensions
		*
		* Window surface creation
		*
		* Start by adding a surface class member right below the debug callback.
		*
		* Although the VkSurfaceKHR object and its usage is platform agnostic, its creation isn't because it depends
		* on window system details.
		*/

		VkSurfaceKHR surface;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device;  // Logical Device
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<VkImageView> swapChainImageViews;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};


		struct QueueFamilyIndices
		{
			std::optional<unsigned int> graphicsFamily;
			std::optional<uint32_t> presentFamily;  // Not all device can present

			bool isCompleted() const
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		// End of Variable and struc declaration
		
		// Vulkan Initialization functions


		// Create Instance

		void createInstance();

		bool checkValidationLayerSupport();

		std::vector<const char*> getRequiredExtensions();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		// End of Create Instance

		// Setting Up Debugger

		void setupDebugMessenger();

		VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		// End of Debugger

		void createSurface();

		void pickPhysicalDevice();

		void createLogicalDevice();

		void createSwapChain();

		// End of Vulkan Initialization Function


		/*
		* *But what if a queue family is not available? We could throw an exception in findQueueFamilies,
		* but this function is not really the right place to make decisions about device suitability.
		* For example, we may prefer devices with a dedicated transfer queue family, but not require it.
		* Therefore we need some way of indicating whether a particular queue family was found.
		*
		* It's not really possible to use a magic value to indicate the nonexistence of a queue family,
		* since any value of uint32_t could in theory be a valid queue family index including 0.
		* Luckily C++17 introduced a data structure to distinguish between the case of a value existing or not.
		*
		* std::optional is a wrapper that contains no value until you assign something to it. At any point
		you can query if it contains a value or not by calling its has_value() member function.
		That means that we can change the logic to
		*/

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

		bool isDeviceSuitable(VkPhysicalDevice device);

		int rateDeviceSuitability(VkPhysicalDevice device);

		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		// Create Swapchain

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);


		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }


		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData, void* pUserData);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		// Image views
		void createImageViews();

		void createGraphicsPipeline();

		VkShaderModule createShaderModule(const std::vector<char>& code);

		static std::vector<char> ReadFile(const std::string& filename);

	};
}
