#include <stdio.h>
#include "ML_ComputeEngine.h"

int main(void) {
    ML_ComputeEngine computeEngine;
    ML_Result result = ML_ComputeEngine_create(&computeEngine);
    if (result != ML_SUCCESS) {
        printf("Error...\n");
    } else {
        printf("Ok...\n");
    }
    ML_ComputeEngine_destroy(computeEngine);
    return 0;
}
