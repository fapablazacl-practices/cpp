
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

static const int ScreenWidth = 1024;
static const int ScreenHeight = 768;

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

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

class Application {
public:
    Application() {
            // initialize glfw
        ::glfwInit();
        ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
        m_window = ::glfwCreateWindow(ScreenWidth, ScreenHeight, "Vulkan 01", nullptr, nullptr);
        assert(m_window);

        const auto extensions = this->enumerateExtensions();
        const auto layers = this->enumerateLayers();

        displayInfo(extensions, layers);

        // create vulkan instance
        auto requiredExtensions = this->getRequiredExtensions();

        VkApplicationInfo appInfo = this->createApplicationInfo();
        VkInstanceCreateInfo instanceInfo = this->createInstanceInfo(&appInfo, layers, enabledLayers, requiredExtensions);

        VkResult result = ::vkCreateInstance(&instanceInfo, nullptr, &m_instance);
    
        m_debugCallback = setupDebug(m_instance);

        this->pickPhysicalDevice();

        m_device = this->createDevice();

        vkGetDeviceQueue(m_device, m_familyIndex, 0, &m_queue);

        assert(result == VK_SUCCESS);
    }

    int run() {
        while (!::glfwWindowShouldClose(m_window)) {
            ::glfwPollEvents();
        }

        return 0;
    }

    ~Application() {
        ::DestroyDebugReportCallbackEXT(m_instance, m_debugCallback, nullptr);

        ::vkDestroyInstance(m_instance, nullptr);
        ::glfwDestroyWindow(m_window);
        ::glfwTerminate();
    }

protected:

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
    
        createInfo.enabledExtensionCount = (uint32_t)enableExtensions.size();
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

    VkDebugReportCallbackEXT setupDebug(VkInstance instance) {
        VkDebugReportCallbackEXT  callback;

        VkDebugReportCallbackCreateInfoEXT createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = debugCallback;

        VkResult result = ::CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);

        assert(result == VK_SUCCESS);

        return callback;
    }

    std::vector<VkPhysicalDevice> enumeratePhysicalDevices() const {
        uint32_t deviceCount;

        ::vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        ::vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        return devices;
    }

    int searchPhysicalDevice(const std::vector<VkQueueFamilyProperties> &families, VkQueueFlags flags) const {
        int result = -1;

        for (size_t i=0; i<families.size(); i++) {
            const auto &family = families[i];

            if (family.queueCount > 0 && family.queueFlags&flags) {
                result = (int)i;
                break;
            }
        }

        return result;
    }

    void pickPhysicalDevice() {
        auto devices = this->enumeratePhysicalDevices();

        for (const auto device : devices) {
            const auto families = this->enumerateFamilies(device);
            const int familyIndex = searchPhysicalDevice(families, VK_QUEUE_GRAPHICS_BIT);

            if (familyIndex >= 0) {
                m_familyIndex = familyIndex;
                m_physicalDevice = device;
            }
        }
    }

    std::vector<VkQueueFamilyProperties> enumerateFamilies(VkPhysicalDevice physicalDevice) const {
        uint32_t propertyCount;

        ::vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, nullptr);
        std::vector<VkQueueFamilyProperties> properties(propertyCount);

        ::vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, properties.data());

        return properties;
    }

    VkDevice createDevice() {
        const auto families = this->enumerateFamilies(m_physicalDevice);
        const int familyIndex = this->searchPhysicalDevice(families, VK_QUEUE_GRAPHICS_BIT);

        const float priority = 1.0f;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = familyIndex;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &priority;
        
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 1;
        createInfo.enabledLayerCount = (uint32_t)enabledLayers.size();
        createInfo.ppEnabledLayerNames = enabledLayers.data();

        VkDevice device;
        VkResult result = ::vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &device);

        assert(result == VK_SUCCESS);

        return device;
    }

private:
    GLFWwindow *m_window = nullptr;
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT m_debugCallback;
    int m_familyIndex = -1;
    VkQueue m_queue;
};

int main(int argc, char **argv) {
    Application app;
    return app.run();
}
