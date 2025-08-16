#include <jni.h>
#include <string>
#include <android/bitmap.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_gm_nativeandroiddemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C" JNIEXPORT jint JNICALL
Java_com_gm_basicopencvwithcamera_MainActivity_add(JNIEnv *env, jobject thiz, jint a, jint b) {
    return a + b;
}

extern "C" JNIEXPORT jintArray JNICALL
Java_com_gm_basicopencvwithcamera_GrayscaleImageConversion_convertImageIntoGrayscale(JNIEnv *env,jobject thiz,jintArray bitmapPixels,jint height, jint width) {

    jint *pixelData = env -> GetIntArrayElements(bitmapPixels, nullptr);

    for(int i=0;i<width*height;i++) {
        jint alpha = (pixelData[i] & 0xFF000000);
        jint red = (pixelData[i] & 0x00FF0000)>>16;
        jint green = (pixelData[i] & 0x0000FF00)>>8;
        jint blue = (pixelData[i] & 0x000000FF);

        jint gray = (red*0.21+green*0.72+blue*0.07);
        gray = alpha | (gray << 16) | (gray << 8) | gray;
        pixelData[i] = gray;
    }

    jintArray result = env ->NewIntArray(width*height);
    env ->SetIntArrayRegion(result,0,width*height,pixelData);

    env ->ReleaseIntArrayElements(bitmapPixels,pixelData,0);

    return result;



    /*AndroidBitmapInfo info;
    void *pixels;

    AndroidBitmap_getInfo(env, bitmap, &info);
    AndroidBitmap_lockPixels(env, bitmap, &pixels);

    // Assuming RGBA_8888 format
    uint32_t *srcPixels = (uint32_t *) pixels;

    for (int y = 0; y < info.height; ++y) {
        for (int x = 0; x < info.width; ++x) {
            uint32_t pixel = srcPixels[y * info.width + x];

            // Extract R, G, B, A components
            uint8_t alpha = (pixel >> 24) & 0xFF;
            uint8_t red = (pixel >> 16) & 0xFF;
            uint8_t green = (pixel >> 8) & 0xFF;
            uint8_t blue = pixel & 0xFF;

            // Calculate grayscale value (weighted average)
            uint8_t gray = (uint8_t) (0.299 * red + 0.587 * green + 0.114 * blue);

            // Set the new pixel with grayscale values
            srcPixels[y * info.width + x] = (alpha << 24) | (gray << 16) | (gray << 8) | gray;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);*/
}