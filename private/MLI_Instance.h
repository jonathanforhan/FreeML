#pragma once

#include <vulkan/vulkan.h>
#include "MLI_Fwd.h"

typedef struct MLI_Instance {
    VkInstance instance;
} MLI_Instance;

void MLI_Instance_create(MLI_Instance* instance);
void MLI_Instance_destroy(MLI_Instance* instance);
VkBool32 MLI_Instance_checkValidationSupport(void);
