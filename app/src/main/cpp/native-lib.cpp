#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include "utils/utils_h/image_utils.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_gm_objectdetectionwithtensorflowdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_gm_objectdetectionwithtensorflowdemo_MainActivity_add(JNIEnv *env, jobject thiz, jint a, jint b) {
    return a + b;
}

extern "C" JNIEXPORT jintArray JNICALL
Java_com_gm_objectdetectionwithtensorflowdemo_GrayscaleImageConversion_convertImageIntoGrayscale(JNIEnv *env,jobject thiz,jintArray bitmapPixels,jint height, jint width) {
    return convertImageIntoGrayscale(env, bitmapPixels, height, width);
}