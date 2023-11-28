#include "MLI_Instance.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "MLI_ComputeEngine.h"
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

void MLI_Instance_create(MLI_Instance* instance) {
    assert(instance != NULL);

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
    if (MLI_Instance_checkValidationSupport() == VK_FALSE) {
        MLI_LOG_ERROR("Vulkan validation layers not present, must not used them (ie build release) or install them");
        exit(EXIT_FAILURE);
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

    VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance->instance);
    if (result != VK_SUCCESS) {
        MLI_LOG_ERROR("Vulkan create instance failure with code %d", result);
        exit(EXIT_FAILURE);
    }
}

void MLI_Instance_destroy(MLI_Instance* instance) {
    assert(instance != NULL);
    vkDestroyInstance(instance->instance, NULL);
}

VkBool32 MLI_Instance_checkValidationSupport(void) {
    VkBool32 ret = VK_TRUE;

#if MLI_VALIDATION_LAYERS_ENABLED
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* layers = malloc(layerCount * sizeof(VkLayerProperties));
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
