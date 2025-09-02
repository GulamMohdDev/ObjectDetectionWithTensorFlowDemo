//
// Created by EE214654 on 8/22/2025.
//
#include <jni.h>

jintArray convertImageIntoGrayscale(
        JNIEnv *env,
        jintArray bitmapPixels,
        jint height,
        jint width
) {
    jint *pixelData = env->GetIntArrayElements(bitmapPixels, nullptr);

    for (int i = 0; i < width * height; i++) {
        jint alpha = (pixelData[i] & 0xFF000000);
        jint red = (pixelData[i] & 0x00FF0000) >> 16;
        jint green = (pixelData[i] & 0x0000FF00) >> 8;
        jint blue = (pixelData[i] & 0x000000FF);

        jint gray = (red * 0.21 + green * 0.72 + blue * 0.07);
        gray = alpha | (gray << 16) | (gray << 8) | gray;
        pixelData[i] = gray;
    }

    jintArray result = env->NewIntArray(width * height);
    env->SetIntArrayRegion(result, 0, width * height, pixelData);

    env->ReleaseIntArrayElements(bitmapPixels, pixelData, 0);

    return result;
}
