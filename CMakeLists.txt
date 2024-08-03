cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(this MyVulkanApp)

project(${this} CXX C)

# Set C++ standard to a proper standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# add_executable(MyVulkanApp src/MyVulkanApp.cpp)

set(VULKAN_LIB_LIST "vulkan-1")

# set(VENDORS "./Vendors")

# target_include_directories(${PROJECT_NAME} PUBLIC
	            # Headers/
                    # Vendors/assimp/include/
                    # Vendors/bullet/src/
                    # Vendors/glad/include/
                    # Vendors/glfw/include/
                    # Vendors/glm/
                    # Vendors/stb/
	# 	    $(VENDORS)/include/
	# 	    $(VULKAN_SDK)/include/
	# )


# Adding Libraries 
if(MSVC OR MSYS OR MINGW)

	# set(CMAKE_C_COMPILER "C:\\Users\\alan\\scoop\\apps\\mingw\\current\\bin\\gcc.exe")
	# set(CMAKE_CXX_COMPILER "C:\\Users\\alan\\scoop\\apps\\mingw\\current\\bin\\g++.exe")

	set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
	set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
	set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
	set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )

	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

    # set(ENV{VULKAN_SDK} "C:\\ExternalTools\\Vulkan\\1.3.283.0")

    set(GLFW_LIB "..\\..\\ExternalLibs\\lib\\GLFW")
    set(PROJ_INCLUDE "..\\..\\ExternalLibs\\include")

    find_package(Vulkan REQUIRED)

    # vulkan-1 library for build Vulkan application.
	set(VULKAN_LIB_LIST "vulkan-1")

	# Add any required preprocessor definitions here
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

    list(APPEND CMAKE_PREFIX_PATH "${GLFW_LIB}")
    find_library(glfw NAMES glfw3 REQUIRED)
    
    #find_package(PkgConfig REQUIRED)
    #pkg_search_module(GLM REQUIRED glm)
    # include_directories(${GLM_INCLUDE_DIRS})
  

    target_include_directories(${PROJECT_NAME} PUBLIC "${VULKAN_INCLUDE}")
    target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_INCLUDE}")
    target_include_directories(${PROJECT_NAME} PUBLIC "${PROJ_INCLUDE}")

    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${glfw})
    target_link_libraries(${PROJECT_NAME} ${GLM_LIBRARY_DIRS})
	
else()
	include_directories(
		    # Headers/
                    # Vendors/assimp/include/
                    # Vendors/bullet/src/
                    # Vendors/glad/include/
                    # Vendors/glfw/include/
                    # Vendors/glm/
                    # Vendors/stb/
                    # $(Vendors)/include/

	)
	list(APPEND CMAKE_PREFIX_PATH "/usr/lib")
	find_package(glfw3 REQUIRED)
	find_package(Vulkan REQUIRED)
	find_package(glm REQUIRED)
endif()

file(GLOB PROJECT_HEADERS src/*.h)

#                           Headers/*.hpp)
file(GLOB PROJECT_SOURCES src/*.cpp)
# file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# file(GLOB PROJECT_SHADERS Shaders/*.comp
#                           Shaders/*.frag
#                           Shaders/*.geom
#                           Shaders/*.vert)
# file(GLOB PROJECT_CONFIGS CMakeLists.txt
#                           Readme.md
#                          .gitattributes
#                          .gitignore
#                          .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
# source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
# source_group("Vendors" FILES ${VENDORS_SOURCES})

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# Copy contents of src/shaders to the binary location
# add_custom_target(assets
	# COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/shaders
	# ${CMAKE_CURRENT_BINARY_DIR}/src/shaders
#)

# Dirty way to add all source files in the ./src directory
# file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
 
# Create the executable
# add_definitions(-DGLFW_INCLUDE_NONE
#                 -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} )

# Link the debug and release libraries to the project
# target_link_libraries( ${PROJECT_NAME} ${VULKAN_LIB_LIST} )

# Link our executable against the libraries
# if(MSVC OR MSYS OR MINGW)
	# target_link_libraries(${PROJECT_NAME} glfw ${VendorsLib}\\glfw3.lib)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# Include Vulkan header files from Vulkan SDK
	# target_include_directories(${PROJECT_NAME} PUBLIC ${VULKAN}/include)
	# target_include_directories(${PROJECT_NAME} PUBLIC ${VENDORS}/include)
	# include_directories(${VENDORS}/include/)
	# include_directories(${VULKAN}/include/)

	# Link directory for vulkan-1
	# link_directories(${VULKAN}/Bin;${VULKAN}/lib;)
	#target_link_libraries(${PROJECT_NAME} ${VULKAN}/lib)
	#target_link_libraries(${PROJECT_NAME} ${VENDORS}/lib)
else()
	target_link_libraries(${PROJECT_NAME} glfw)
    target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
    target_link_libraries(${PROJECT_NAME} glm::glm)
endif()

# Adding dependencies eg Shaders etc
# add_dependencies(${PROJECT_NAME} assets)

# Define project properties
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Define C++ version to be used for building the project
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# Define C version to be used for building the project
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)


