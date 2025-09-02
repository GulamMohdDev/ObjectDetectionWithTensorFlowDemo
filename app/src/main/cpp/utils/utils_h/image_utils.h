//
// Created by EE214654 on 8/22/2025.
//

#include <jni.h>

#ifndef BASICOPENCVWITHCAMERA_IMAGE_UTILS_H
#define BASICOPENCVWITHCAMERA_IMAGE_UTILS_H

jintArray convertImageIntoGrayscale(
        JNIEnv *env,
        jintArray bitmapPixels,
        jint height,
        jint width
);

#endif //BASICOPENCVWITHCAMERA_IMAGE_UTILS_H
