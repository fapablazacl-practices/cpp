
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>
#include <vector>

const std::vector<const char*> enabledLayers = {
#if defined(_DEBUG)
    "VK_LAYER_LUNARG_standard_validation"
#endif
};

const std::vector<const char *> enabledExtensions = {
    "VK_EXT_debug_report"
};

std::vector<VkLayerProperties> enumerateLayers() {
    std::uint32_t layerCount;
    
    ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layers(layerCount);
    ::vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    
    return layers;   
}

bool checkLayer(const std::vector<VkLayerProperties> &layers, const std::string &layerName) {    
    for (const VkLayerProperties &layer : layers) {
        if (layerName == layer.layerName) {
            return true;
        }
    }    
    
    return false;
}

bool checkLayers(const std::vector<VkLayerProperties> &layers, const std::vector<const char*> &layerNames) {
    for (const char *layerName : layerNames) {
        if (!checkLayer(layers, layerName)) {
            return false;
        }
    }

    return true;
}

std::vector<VkExtensionProperties> enumerateExtensions() {
    std::uint32_t propertiesCount;
    std::vector<VkExtensionProperties> properties;
    
    ::vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
    properties.resize(propertiesCount);
    ::vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.data());

    return properties;
}

VkApplicationInfo createApplicationInfo() {
    VkApplicationInfo appInfo = {};
    
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan 01";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "N/A";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    return appInfo;
}

std::vector<const char *> getRequiredExtensions() {

    // populate extensions
    std::vector<const char *> extensions;

    unsigned glfwExtensionCount = 0;
    const char **glfwExtensions = ::glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned i=0; i<glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

    // append application required extensions
    extensions.insert(extensions.end(), enabledExtensions.begin(), enabledExtensions.end());

    return extensions;
}

VkInstanceCreateInfo createInstanceInfo(const VkApplicationInfo *appInfo, const std::vector<VkLayerProperties> &layers, const std::vector<const char *> &enableLayers, const std::vector<const char *> &enableExtensions) {
    // initialize vulkan
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = appInfo;
    
    createInfo.enabledExtensionCount = (std::uint32_t)enableExtensions.size();
    createInfo.ppEnabledExtensionNames = enableExtensions.data();

    if (checkLayers(layers, enableLayers)) {
        createInfo.enabledLayerCount = (uint32_t)enableLayers.size();
        createInfo.ppEnabledLayerNames = enableLayers.data();
    }

    return createInfo;
}

void displayInfo(const std::vector<VkExtensionProperties> &extensions, const std::vector<VkLayerProperties> &layers) {
    // display extension info
    std::cout << "Available extensions:" << std::endl;
    
    for (const VkExtensionProperties &property : extensions) {
        std::cout << "\t" << property.extensionName << std::endl;
    }
    
    // display available layer information 
    std::cout << "Available layers:" << std::endl;

    for (const VkLayerProperties &layer: layers) {
        std::cout << "\t" << layer.layerName << std::endl;
    }
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags, 
    VkDebugReportObjectTypeEXT objectType, 
    uint64_t object,
    size_t location,
    int32_t code,
    const char *layerPrefix, 
    const char *message, 
    void *userData
) {
    std::cerr << "Validation Layer: " << message << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT (
    VkInstance instance, 
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugReportCallbackEXT* pCallback) {

    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

    if (func) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VkDebugReportCallbackEXT  setupDebug(VkInstance instance) {
    VkDebugReportCallbackEXT  callback;

    VkDebugReportCallbackCreateInfoEXT createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;

    VkResult result = CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);

    assert(result == VK_SUCCESS);

    return callback;
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

    const auto extensions = enumerateExtensions();
    const auto layers = enumerateLayers();

    displayInfo(extensions, layers);

    // create vulkan instance
    auto requiredExtensions = getRequiredExtensions();

    VkApplicationInfo appInfo = ::createApplicationInfo();
    VkInstanceCreateInfo instanceInfo = ::createInstanceInfo(&appInfo, layers, enabledLayers, requiredExtensions);

    VkInstance instance;
    VkResult result = ::vkCreateInstance(&instanceInfo, nullptr, &instance);
    
    assert(result == VK_SUCCESS);
    
    while (!::glfwWindowShouldClose(window)) {
        ::glfwPollEvents();
    }
    
    ::glfwDestroyWindow(window);
    ::glfwTerminate();
    
    return 0;
}
