
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>
#include <vector>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

std::vector<VkLayerProperties> enumerateLayers() {
    std::uint32_t layerCount;
    
    ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layers(layerCount);
    ::vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    
    return layers;   
}

bool checkLayers(const std::vector<VkLayerProperties> &layers, const std::vector<const char*> &) {
    std::vector<VkLayerProperties> layers = enumerateLayers();
}

bool checkLayer(const std::vector<VkLayerProperties> &layers, const std::string &layerName) {    
    for (const VkLayerProperties &layer : layers) {
        if (layerName == layer.layerName) {
            return true;
        }
    }    
    
    return false;
}

int main() {
    
    // initialize glfw
    const int ScreenWidth = 1024;
    const int ScreenHeight = 768;
    
    ::glfwInit();
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    GLFWwindow* window = ::glfwCreateWindow(ScreenWidth, ScreenHeight, "Vulkan 01", nullptr, nullptr);
    assert(window);

    // initialize vulkan
    VkApplicationInfo appInfo = {};
    
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan 01";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "N/A";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    unsigned extensionCount = 0;
    const char **extensions = ::glfwGetRequiredInstanceExtensions(&extensionCount);
    
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;
    
    // create vulkan instance
    VkInstance instance;
    VkResult result = ::vkCreateInstance(&createInfo, nullptr, &instance);
    
    assert(result == VK_SUCCESS);
    
    // enumerate available extension properties
    std::uint32_t propertiesCount;
    std::vector<VkExtensionProperties> properties;
    
    ::vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
    properties.resize(propertiesCount);
    ::vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.data());
    
    // display info 
    for (const VkExtensionProperties &property : properties) {
        std::cout << "\t" << property.extensionName << std::endl;
    }
    
    // check for validation layers
    if (checkLayer(ValidationLayer)) {
        
    }
    
    while (!::glfwWindowShouldClose(window)) {
        ::glfwPollEvents();
    }
    
    ::glfwDestroyWindow(window);
    ::glfwTerminate();
    
    return 0;
}
