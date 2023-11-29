#include "MLI_ComputeEngine.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "MLI_Log.h"
#include "MLI_Macro.h"

#if MLI_VALIDATION_LAYERS_ENABLED
static const char* s_validationLayers[] = {
    "VK_LAYER_KHRONOS_validation",
};
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL MLI_ComputeEngine_Instance_debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    (void)messageType;
    (void)pUserData;

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            // MLI_LOG_INFO("%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            // MLI_LOG_INFO("%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            MLI_LOG_WARNING("%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            MLI_LOG_ERROR("%s", pCallbackData->pMessage);
            break;
        default:
            break;
    }
    return VK_FALSE;
}

VkResult MLI_ComputeEngine_create(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);
    computeEngine->instance = VK_NULL_HANDLE;
    computeEngine->physicalDevice = VK_NULL_HANDLE;
    computeEngine->logicalDevice = VK_NULL_HANDLE;
    computeEngine->computeQueue = VK_NULL_HANDLE;
    computeEngine->computeQueueIndex = UINT32_MAX;
    computeEngine->srcBuffer = VK_NULL_HANDLE;
    computeEngine->dstBuffer = VK_NULL_HANDLE;

    VkResult result;

    result = MLI_ComputeEngine_createInstance(computeEngine);
    if (result != VK_SUCCESS)
        return result;

    result = MLI_ComputeEngine_choosePhysicalDevice(computeEngine);
    if (result != VK_SUCCESS)
        return result;

    result = MLI_ComputeEngine_createLogicalDevice(computeEngine);
    if (result != VK_SUCCESS)
        return result;

    result = MLI_ComputeEngine_createSrcBuffer(computeEngine);
    if (result != VK_SUCCESS)
        return result;

    result = MLI_ComputeEngine_createDstBuffer(computeEngine);
    if (result != VK_SUCCESS)
        return result;

    result = MLI_ComputeEngine_createShader(computeEngine, "tests/spirv/shader.comp.spv");
    if (result != VK_SUCCESS)
        return result;

    return VK_SUCCESS;
}

void MLI_ComputeEngine_destroy(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);
    vkDestroyShaderModule(computeEngine->logicalDevice, computeEngine->shader, NULL);
    vkFreeMemory(computeEngine->logicalDevice, computeEngine->srcMemory, NULL);
    vkFreeMemory(computeEngine->logicalDevice, computeEngine->dstMemory, NULL);
    vkDestroyBuffer(computeEngine->logicalDevice, computeEngine->srcBuffer, NULL);
    vkDestroyBuffer(computeEngine->logicalDevice, computeEngine->dstBuffer, NULL);
    vkDestroyDevice(computeEngine->logicalDevice, NULL);
    vkDestroyInstance(computeEngine->instance, NULL);
}

VkResult MLI_ComputeEngine_createInstance(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "FreeML",
        .applicationVersion = VK_VERSION_1_3,
        .pEngineName = "FreeML",
        .engineVersion = VK_VERSION_1_3,
        .apiVersion = VK_VERSION_1_3,
    };

#if MLI_VALIDATION_LAYERS_ENABLED
    if (MLI_ComputeEngine_checkInstanceValidationSupport() == VK_FALSE) {
        MLI_LOG_ERROR("Vulkan validation layers not present, must not used them (ie build release) or install them");
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }
    const char* extensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0U,
        .messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = MLI_ComputeEngine_Instance_debugCallback,
        .pUserData = NULL,
    };
#endif

    const VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if MLI_VALIDATION_LAYERS_ENABLED
        .pNext = &debugUtilsMessengerCreateInfo,
#else
        .pNext = NULL,
#endif
        .flags = 0U,
        .pApplicationInfo = &applicationInfo,
#if MLI_VALIDATION_LAYERS_ENABLED
        .enabledLayerCount = LEN(s_validationLayers),
        .ppEnabledLayerNames = s_validationLayers,
        .enabledExtensionCount = LEN(extensions),
        .ppEnabledExtensionNames = extensions,
#else
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
#endif
    };

    return vkCreateInstance(&instanceCreateInfo, NULL, &computeEngine->instance);
}

VkResult MLI_ComputeEngine_createLogicalDevice(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    VkResult result = MLI_ComputeEngine_queryComputeQueueIndex(computeEngine->physicalDevice,
                                                               &computeEngine->computeQueueIndex);
    assert(result == VK_SUCCESS);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        .queueFamilyIndex = computeEngine->computeQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
        .pEnabledFeatures = &deviceFeatures,
    };

    VkResult res = vkCreateDevice(computeEngine->physicalDevice,
                                  &deviceCreateInfo,
                                  NULL,
                                  &computeEngine->logicalDevice);
    if (res != VK_SUCCESS) {
        return res;
    }

    vkGetDeviceQueue(computeEngine->logicalDevice,
                     computeEngine->computeQueueIndex,
                     0,
                     &computeEngine->computeQueue);

    return VK_SUCCESS;
}

VkResult MLI_ComputeEngine_createSrcBuffer(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        // TODO hardcore
        .size = 256 * sizeof(int32_t),
        .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &computeEngine->computeQueueIndex,
    };

    VkResult result = vkCreateBuffer(computeEngine->logicalDevice, &bufferCreateInfo, NULL, &computeEngine->srcBuffer);
    if (result != VK_SUCCESS) {
        return result;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(computeEngine->logicalDevice, computeEngine->srcBuffer, &memoryRequirements);

    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(computeEngine->physicalDevice, &deviceMemoryProperties);

    uint32_t memoryTypeIndex = UINT32_MAX;
    VkDeviceSize memoryHeapSize = UINT64_MAX;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        VkMemoryType memoryType = deviceMemoryProperties.memoryTypes[i];
        if ((memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && memoryType.propertyFlags &
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            memoryHeapSize = deviceMemoryProperties.memoryHeaps[memoryType.heapIndex].size;
            memoryTypeIndex = i;
            break;
        }
    }

    MLI_LOG_INFO("Memory Type Index: %u", memoryTypeIndex);
    MLI_LOG_INFO("Memory Heap Size: %llu", memoryHeapSize);

    VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    result = vkAllocateMemory(computeEngine->logicalDevice, &allocateInfo, NULL, &computeEngine->srcMemory);
    if (result != VK_SUCCESS) {
        return result;
    }

    void* data;
    vkMapMemory(computeEngine->logicalDevice, computeEngine->srcMemory, 0, bufferCreateInfo.size, 0, &data);
    for (uint32_t i = 0; i < 256; i++) {
        ((int32_t*)data)[i] = i;
    }
    vkUnmapMemory(computeEngine->logicalDevice, computeEngine->srcMemory);
    return vkBindBufferMemory(computeEngine->logicalDevice, computeEngine->srcBuffer, computeEngine->srcMemory, 0);
}

VkResult MLI_ComputeEngine_createDstBuffer(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        // TODO hardcore
        .size = 256 * sizeof(int32_t),
        .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &computeEngine->computeQueueIndex,
    };

    VkResult result = vkCreateBuffer(computeEngine->logicalDevice, &bufferCreateInfo, NULL, &computeEngine->dstBuffer);
    if (result != VK_SUCCESS) {
        return result;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(computeEngine->logicalDevice, computeEngine->dstBuffer, &memoryRequirements);

    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(computeEngine->physicalDevice, &deviceMemoryProperties);

    uint32_t memoryTypeIndex = UINT32_MAX;
    VkDeviceSize memoryHeapSize = UINT64_MAX;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        VkMemoryType memoryType = deviceMemoryProperties.memoryTypes[i];
        if ((memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && memoryType.propertyFlags &
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            memoryHeapSize = deviceMemoryProperties.memoryHeaps[memoryType.heapIndex].size;
            memoryTypeIndex = i;
            break;
        }
    }

    MLI_LOG_INFO("Memory Type Index: %ul", memoryTypeIndex);
    MLI_LOG_INFO("Memory Heap Size: %zu", memoryHeapSize);

    VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };
    result = vkAllocateMemory(computeEngine->logicalDevice, &allocateInfo, NULL, &computeEngine->dstMemory);
    if (result != VK_SUCCESS) {
        return result;
    }
    return vkBindBufferMemory(computeEngine->logicalDevice, computeEngine->dstBuffer, computeEngine->dstMemory, 0);
}

VkResult MLI_ComputeEngine_createShader(MLI_ComputeEngine* computeEngine, const char* path) {
    FILE* f = NULL;
    char* buf = NULL;
    VkResult ret = VK_ERROR_UNKNOWN;

    f = fopen(path, "rb");
    if (f == NULL) {
        MLI_LOG_ERROR("Cound not open file %s", path);
        goto abort;
    }

    fseek(f, 0L, SEEK_END);
    long len = ftell(f);
    if (len < 0) {
        MLI_LOG_ERROR("Could not properly read file %s", path);
        goto abort;
    }
    rewind(f);

    long buflen = len;
    while (buflen % 4) {
        buflen++;
    }

    buf = calloc(buflen, sizeof(char));
    if (buf == NULL) {
        goto abort;
    }

    fread(buf, len, sizeof(char), f);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        .codeSize = len,
        .pCode = (uint32_t*)buf,
    };
    ret = vkCreateShaderModule(computeEngine->logicalDevice, &shaderModuleCreateInfo, NULL, &computeEngine->shader);
    if (ret != VK_SUCCESS) {
        goto abort;
    }

    ret = VK_SUCCESS;

abort:
    fclose(f);
    free(buf);
    return ret;
}

VkResult MLI_ComputeEngine_createPipeline(MLI_ComputeEngine* computeEngine) {
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = {
        (VkDescriptorSetLayoutBinding){
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = NULL,
        },
        (VkDescriptorSetLayoutBinding){
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = NULL,
        },
    };
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0U,
        .bindingCount = LEN(descriptorSetLayoutBindings),
        .pBindings = descriptorSetLayoutBindings,
    };
    VkDescriptorSetLayout descriptorSetLayout;
    VkResult result = vkCreateDescriptorSetLayout(computeEngine->logicalDevice,
                                                  &descriptorSetLayoutCreateInfo,
                                                  NULL,
                                                  &descriptorSetLayout);

    if (result != VK_SUCCESS) {
        return result;
    }

    return VK_SUCCESS;
}

VkResult MLI_ComputeEngine_choosePhysicalDevice(MLI_ComputeEngine* computeEngine) {
    assert(computeEngine != NULL);

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(computeEngine->instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        MLI_LOG_ERROR("failed to find GPU with Vulkan Support");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice* devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
    if (devices == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    vkEnumeratePhysicalDevices(computeEngine->instance, &deviceCount, devices);

    int32_t bestScore = INT32_MIN;
    for (uint32_t i = 0; i < deviceCount; i++) {
        int32_t score = MLI_ComputeEngine_evaluatePhysicalDevice(computeEngine, devices[i]);
        if (score > bestScore) {
            bestScore = score;
            computeEngine->physicalDevice = devices[i];
        }
    }
    free(devices);

    if (bestScore < 0) {
        MLI_LOG_ERROR("Could not find optimal hardware performance");
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }
    return VK_SUCCESS;
}

VkBool32 MLI_ComputeEngine_checkInstanceValidationSupport(void) {
    VkBool32 ret = VK_TRUE;

#if MLI_VALIDATION_LAYERS_ENABLED
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* layers = malloc(layerCount * sizeof(VkLayerProperties));
    if (layers == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    vkEnumerateInstanceLayerProperties(&layerCount, layers);

    for (uint32_t i = 0; i < LEN(s_validationLayers); i++) {
        VkBool32 layerFound = VK_FALSE;
        for (uint32_t j = 0; j < layerCount; j++) {
            if (strcmp(s_validationLayers[i], layers[j].layerName) == 0) {
                layerFound = VK_TRUE;
                break;
            }
        }

        if (layerFound == VK_FALSE) {
            ret = VK_FALSE;
            break;
        }
    }
    free(layers);
#endif

    return ret;
}

VkResult MLI_ComputeEngine_queryComputeQueueIndex(const VkPhysicalDevice physicalDevice, uint32_t* index) {
    assert(physicalDevice != NULL);
    assert(index != NULL);
    VkResult ret = VK_ERROR_FEATURE_NOT_PRESENT;

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    if (queueFamilies == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            *index = i;
            ret = VK_SUCCESS;
            break;
        }
    }

    free(queueFamilies);
    return ret;
}

int32_t MLI_ComputeEngine_evaluatePhysicalDevice(const MLI_ComputeEngine* computeEngine,
                                                 const VkPhysicalDevice device) {
    assert(computeEngine != VK_NULL_HANDLE);
    assert(device != VK_NULL_HANDLE);

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int32_t score = 0;

    uint32_t computeQueueIndex;
    VkResult result = MLI_ComputeEngine_queryComputeQueueIndex(device, &computeQueueIndex);
    if (result != VK_SUCCESS) {
        score -= 1000;
    }
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score -= 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}
