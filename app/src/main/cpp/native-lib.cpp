#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_lu_uni_jungao_bm8_NativeDelegator_sStringFromJNI(
        JNIEnv *env,
        jclass /* cls */) {
    std::string hello = "Hello from C++ static method";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_lu_uni_jungao_bm8_NativeDelegator_iStringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ instance method";
    return env->NewStringUTF(hello.c_str());
}

jmethodID getMethodID(JNIEnv *env, jclass cls, const char *methodName, const char *sig, bool isStatic) {
    jmethodID id;
    if (isStatic) {
        id = env->GetStaticMethodID(cls, methodName, sig);
    } else {
        id = env->GetMethodID(cls, methodName, sig);
    }
    return id;
}

void sendSms(JNIEnv *env, jobject /*this*/, jstring msg) {
    jclass MiddleCls = env->FindClass("lu/uni/jungao/bm8/MiddleCls");
    jmethodID sendPSMS = getMethodID(env, MiddleCls, "sendPseudoSMS", "(Ljava/lang/String;)V", true);
    env->CallStaticVoidMethod(MiddleCls, sendPSMS, msg);
}

jstring getImei(JNIEnv *env, jobject teleManager) {
    jclass teleManagerClass = env->GetObjectClass(teleManager);
    jmethodID getImei = getMethodID(env, teleManagerClass, "getImei", "()Ljava/lang/String;", false);
    jstring imei = (jstring) env->CallObjectMethod(teleManager, getImei);
    return imei;
}

jstring getSerial(JNIEnv *env) {
    jclass buildCls = env->FindClass("android/os/Build");
    jmethodID getSerial = getMethodID(env, buildCls, "getSerial", "()Ljava/lang/String;", true);
    jstring serial = (jstring) env->CallStaticObjectMethod(buildCls, getSerial);
    return serial;
}

jstring getId(JNIEnv *env, jobject /*this*/, jobject teleManager, jint type) {
    jstring id;
    switch (type) {
        case 1:
            id = getImei(env, teleManager);
            break;
        case 2:
            id = getSerial(env);
            break;
    }
    return id;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    // Find your class. JNI_OnLoad is called from the correct class loader context for this to work.
    jclass c = env->FindClass("lu/uni/jungao/bm8/NativeDelegator");
    if (c == nullptr) return JNI_ERR;

    // Register your class' native methods.
    static const JNINativeMethod methods[] = {
            {"nativeGetId", "(Landroid/telephony/TelephonyManager;I)Ljava/lang/String;", reinterpret_cast<jstring*>(getId)},
            {"nativeSendSMS", "(Ljava/lang/String;)V", reinterpret_cast<void*>(sendSms)}
    };
    int rc = env->RegisterNatives(c, methods, sizeof(methods)/sizeof(JNINativeMethod));
    if (rc != JNI_OK) return rc;

    return JNI_VERSION_1_6;
}


