//
// Created by EE214654 on 8/21/2025.
//
#include "system_utils.h"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <string>
#include <android/hardware_buffer.h>
#include <android/log.h>
#include <dlfcn.h> // For dynamic linking (NNAPI)
//#include <neuralnetworks/NeuralNetworks.h>

using namespace std;

namespace {
    const int SAMPLE_WINDOW = 10; // Number of frames to average over
    vector<double> frameTimes; // Store frame times
    double lastTime = 0.0; // Last frame timestamp
    float currentFps = 0.0; // Current FPS
}

CpuStats readCpuStats() {
    CpuStats stats = {0};
    ifstream statFile("/proc/stat");
    string line;

    if (statFile.is_open()) {
        getline(statFile, line); // Read the first line starting with "cpu"
        istringstream iss(line);
        string cpu;
        iss >> cpu >> stats.user >> stats.nice >> stats.system >> stats.idle
            >> stats.iowait >> stats.irq >> stats.softirq;
        statFile.close();
    }
    return stats;
}

float calculateCpuUsage() {
    // First sample
    CpuStats stats1 = readCpuStats();

    // Wait for 100ms to get a meaningful difference
    usleep(100000);

    // Second sample
    CpuStats stats2 = readCpuStats();

    // Calculate total and idle time
    long prevTotal = stats1.user + stats1.nice + stats1.system + stats1.idle +
                     stats1.iowait + stats1.irq + stats1.softirq;
    long currTotal = stats2.user + stats2.nice + stats2.system + stats2.idle +
                     stats2.iowait + stats2.irq + stats2.softirq;

    long prevIdle = stats1.idle + stats1.iowait;
    long currIdle = stats2.idle + stats2.iowait;

    // Calculate CPU usage percentage
    long totalDiff = currTotal - prevTotal;
    long idleDiff = currIdle - prevIdle;

    return (totalDiff > 0) ? (100.0 * (totalDiff - idleDiff) / totalDiff) : 0.0;
}

void initFrameRate() {
    frameTimes.clear();
    lastTime = 0.0;
    currentFps = 0.0;

    // Initialize lastTime with current time
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    lastTime = ts.tv_sec + ts.tv_nsec / 1e9;
}

void updateFrame() {
    // Get current time
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double currentTime = ts.tv_sec + ts.tv_nsec / 1e9;

    // Calculate time delta
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Store delta time
    frameTimes.push_back(deltaTime);
    if (frameTimes.size() > SAMPLE_WINDOW) {
        frameTimes.erase(frameTimes.begin());
    }

    // Calculate average frame time and FPS
    if (!frameTimes.empty()) {
        double totalTime = 0.0;
        for (double dt : frameTimes) {
            totalTime += dt;
        }
        double avgFrameTime = totalTime / frameTimes.size();
        currentFps = (avgFrameTime > 0) ? (1.0 / avgFrameTime) : 0.0;
    }
}

float getFrameRate() {
    return currentFps;
}

long getMemoryUsage() {
    ifstream statusFile("/proc/self/status");
    string line;
    long vmRss = 0;

    if (statusFile.is_open()) {
        while (getline(statusFile, line)) {
            if (line.find("VmRSS:") != string::npos) {
                // Extract the VmRSS value (Resident Set Size in KB)
                string value = line.substr(line.find(":") + 1);
                value.erase(0, value.find_first_not_of(" \t")); // Trim leading whitespace
                size_t endPos = value.find("kB");
                if (endPos != string::npos) {
                    value = value.substr(0, endPos);
                    value.erase(value.find_last_not_of(" \t") + 1); // Trim trailing whitespace
                    try {
                        vmRss = stol(value);
                    } catch (...) {
                        vmRss = 0; // Error handling
                    }
                }
                break;
            }
        }
        statusFile.close();
    }
    return vmRss; // Return RSS in KB
}

long getVirtualMemory() {
    ifstream statusFile("/proc/self/status");
    string line;
    long vmSize = 0;

    if (statusFile.is_open()) {
        while (getline(statusFile, line)) {
            if (line.find("VmSize:") != string::npos) {
                string value = line.substr(line.find(":") + 1);
                value.erase(0, value.find_first_not_of(" \t")); // Trim leading whitespace
                size_t endPos = value.find("kB");
                if (endPos != string::npos) {
                    value = value.substr(0, endPos);
                    value.erase(value.find_last_not_of(" \t") + 1); // Trim trailing whitespace
                    try {
                        vmSize = stol(value);
                    } catch (...) {
                        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", "Failed to parse VmSize");
                        vmSize = 0;
                    }
                }
                break;
            }
        }
        statusFile.close();
    } else {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Failed to open /proc/self/status for VmSize");
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "App Virtual Memory: %ld KB", vmSize);
    return vmSize; // Return VmSize in KB
}

long getTotalMemory() {
    ifstream meminfoFile("/proc/meminfo");
    string line;
    long memTotal = 0;

    if (meminfoFile.is_open()) {
        while (getline(meminfoFile, line)) {
            if (line.find("MemTotal:") != string::npos) {
                // Extract the MemTotal value (total system memory in KB)
                string value = line.substr(line.find(":") + 1);
                value.erase(0, value.find_first_not_of(" \t")); // Trim leading whitespace
                size_t endPos = value.find("kB");
                if (endPos != string::npos) {
                    value = value.substr(0, endPos);
                    value.erase(value.find_last_not_of(" \t") + 1); // Trim trailing whitespace
                    try {
                        memTotal = stol(value);
                    } catch (...) {
                        memTotal = 0; // Error handling
                    }
                }
                break;
            }
        }
        meminfoFile.close();
    }
    return memTotal; // Return total memory in KB
}

float getGpuUsage() {
    // Try Qualcomm GPU path
    ifstream gpuFile("/sys/class/kgsl/kgsl-3d0/gpubusy");
    if (gpuFile.is_open()) {
        string line;
        getline(gpuFile, line);
        gpuFile.close();

        // Format: "busy_time total_time" (in microseconds)
        istringstream iss(line);
        long busyTime = 0, totalTime = 0;
        iss >> busyTime >> totalTime;
        if (totalTime > 0) {
            return (100.0f * busyTime) / totalTime;
        }
    }

    // Try Mali GPU path
    ifstream maliFile("/sys/class/misc/mali0/device/utilization");
    if (maliFile.is_open()) {
        string line;
        getline(maliFile, line);
        maliFile.close();
        try {
            return stof(line);
        } catch (...) {
            return -1.0f; // Error parsing
        }
    }

    // Return -1 if GPU usage data is unavailable
    return -1.0f;
}

int checkNpuAvailability() {
    // Load NNAPI dynamically (since it's not guaranteed to be available)
    void* nnapiLib = dlopen("libneuralnetworks.so", RTLD_LAZY | RTLD_LOCAL);
    if (!nnapiLib) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to load libneuralnetworks.so");
        return -1; // Error
    }

    // Get ANeuralNetworks_getDeviceCount function
    typedef int (*ANeuralNetworks_getDeviceCount_t)(uint32_t*);
    ANeuralNetworks_getDeviceCount_t getDeviceCount = reinterpret_cast<ANeuralNetworks_getDeviceCount_t>(
            dlsym(nnapiLib, "ANeuralNetworks_getDeviceCount"));
    if (!getDeviceCount) {
        dlclose(nnapiLib);
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to find ANeuralNetworks_getDeviceCount");
        return -1; // Error
    }

    // Check for available devices
    uint32_t numDevices = 0;
    int result = getDeviceCount(&numDevices);
    dlclose(nnapiLib);

    if (result != 0 || numDevices == 0) {
        return 0; // No NPU devices available
    }

    return 1; // NPU is available
}

/*float estimateNpuUsage() {
    // Initialize NNAPI
    ANeuralNetworksModel* model = nullptr;
    ANeuralNetworksCompilation* compilation = nullptr;
    ANeuralNetworksExecution* execution = nullptr;
    float executionTimeMs = -1.0f;

    // Create a simple model: single ADD operation
    if (ANeuralNetworksModel_create(&model) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to create NNAPI model");
        return -1.0f;
    }

    // Define operands
    ANeuralNetworksOperandType floatType = {
            .type = ANEURALNETWORKS_TENSOR_FLOAT32,
            .dimensionCount = 2,
            .dimensions = (uint32_t[]){1, 2}, // 1x2 tensor
            .scale = 0.0f,
            .zeroPoint = 0
    };

    uint32_t input1 = 0, input2 = 1, output = 2;
    if (ANeuralNetworksModel_addOperand(model, &floatType) != ANEURALNETWORKS_NO_ERROR ||
        ANeuralNetworksModel_addOperand(model, &floatType) != ANEURALNETWORKS_NO_ERROR ||
        ANeuralNetworksModel_addOperand(model, &floatType) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to add operands");
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Define ADD operation
    ANeuralNetworksOperationType addOp = ANEURALNETWORKS_ADD;
    uint32_t inputs[] = {input1, input2};
    uint32_t outputs[] = {output};
    if (ANeuralNetworksModel_addOperation(model, addOp, 2, inputs, 1, outputs) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to add operation");
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Identify inputs and outputs
    if (ANeuralNetworksModel_identifyInputsAndOutputs(model, 2, inputs, 1, outputs) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to identify inputs/outputs");
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Compile the model
    if (ANeuralNetworksCompilation_create(model, &compilation) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to create compilation");
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Prefer NPU execution
    ANeuralNetworksCompilation_setPreference(compilation, ANEURALNETWORKS_PREFER_LOW_POWER);
    if (ANeuralNetworksCompilation_finish(compilation) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to finish compilation");
        ANeuralNetworksCompilation_free(compilation);
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Create execution
    if (ANeuralNetworksExecution_create(compilation, &execution) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to create execution");
        ANeuralNetworksCompilation_free(compilation);
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Set input data
    float inputData1[] = {1.0f, 2.0f};
    float inputData2[] = {3.0f, 4.0f};
    float outputData[2];
    if (ANeuralNetworksExecution_setInput(execution, 0, nullptr, inputData1, sizeof(inputData1)) != ANEURALNETWORKS_NO_ERROR ||
        ANeuralNetworksExecution_setInput(execution, 1, nullptr, inputData2, sizeof(inputData2)) != ANEURALNETWORKS_NO_ERROR ||
        ANeuralNetworksExecution_setOutput(execution, 0, nullptr, outputData, sizeof(outputData)) != ANEURALNETWORKS_NO_ERROR) {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to set inputs/outputs");
        ANeuralNetworksExecution_free(execution);
        ANeuralNetworksCompilation_free(compilation);
        ANeuralNetworksModel_free(model);
        return -1.0f;
    }

    // Measure execution time
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (ANeuralNetworksExecution_compute(execution) == ANEURALNETWORKS_NO_ERROR) {
        clock_gettime(CLOCK_MONOTONIC, &end);
        executionTimeMs = (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_nsec - start.tv_nsec) / 1e6f;
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "GPU_NPU_UTILS", "Failed to execute model");
        executionTimeMs = -1.0f;
    }

    // Clean up
    ANeuralNetworksExecution_free(execution);
    ANeuralNetworksCompilation_free(compilation);
    ANeuralNetworksModel_free(model);

    return executionTimeMs; // Return execution time in milliseconds
}*/


