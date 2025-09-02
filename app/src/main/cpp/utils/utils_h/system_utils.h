//
// Created by EE214654 on 8/21/2025.
//

#ifndef DEMOAIMODEL_SYSTEM_UTILS_H
#define DEMOAIMODEL_SYSTEM_UTILS_H

#define LOG_TAG "System Utils" // Your custom log tag
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

struct CpuStats {
    long user, nice, system, idle, iowait, irq, softirq;
};

// Function to read CPU stats from /proc/stat
CpuStats readCpuStats();

// Function to calculate CPU usage percentage
float calculateCpuUsage();

// Initialize frame rate tracking
void initFrameRate();

// Update frame rate calculation (call per frame)
void updateFrame();

// Get current frame rate (FPS)
float getFrameRate();

// Get Current Memory Usage
long getMemoryUsage();

long getVirtualMemory();

long getTotalMemory();

// Get Current GPU Usage
// Get GPU usage percentage (returns -1 if unavailable)
float getGpuUsage();

// Check if NPU is available and used (returns 1 if available, 0 if not, -1 on error)
int checkNpuAvailability();




#endif //DEMOAIMODEL_SYSTEM_UTILS_H
