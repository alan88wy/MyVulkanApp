#pragma once

//#define NDEBUG

#define GLFW_INCLUDE_VULKAN
//#define VK_USE_PLATFORM_WIN32_KHR
//#define GLFW_EXPOSE_NATIVE_WIN32

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <GLFW/glfw3native.h>
#include <Vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>

#include <iostream>
#include <vector>
#include <optional>
#include <map>
#include <set>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Window
{
	public:
		Window();

		Window(GLint windowWidth, GLint windowHeight);

		int InitWindow();

		void InitVulkan();

		void MainLoop();

		void CreateInstance();

		void PickPhysicalDevice();

		bool IsDeviceSuitable(VkPhysicalDevice device);

		int RateDeviceSuitability(VkPhysicalDevice device);

		void CreateLogicalDevice();

		void CreateSurface();

		void CleanUp();

		bool GetShouldClose() { return glfwWindowShouldClose(mainWindow); }

		bool CheckValidationLayerSupport();

		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		void SetupDebugMessenger();

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

		std::vector<const char *> GetRequiredExtensions();

		~Window();

	private:
		GLFWwindow* mainWindow;

		VkInstance instance;

		GLint width, height;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device;  // Logical Device
		VkQueue graphicsQueue;
		VkQueue presentQueue;

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

		/*
		* * But what if a queue family is not available? We could throw an exception in findQueueFamilies, 
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

		struct QueueFamilyIndices
		{
			std::optional<unsigned int> graphicsFamily;
			std::optional<uint32_t> presentFamily;  // Not all device can present

			bool IsCompleted() const
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData, void* pUserData)
		{
			std::cerr << "Validation Layer : " << pCallBackData->pMessage << std::endl;

			return VK_FALSE;
		}

};