#include <jni.h>
#include <string>
#include <UbiForm/Component.h>

Component* component = nullptr;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_startComponent(JNIEnv *env, jobject thiz,
                                                                  jstring ip_address) {
    try {
        // Starting is fine, but get error trying to start new thread
        // Reckon it is fixable
        if(component == nullptr) {
            jboolean isCopy = false;
            std::string componentUrl = env->GetStringUTFChars(ip_address, &isCopy);
            component = new Component(componentUrl);
        }
        if(component->getBackgroundPort() == -1) {
            component->startBackgroundListen();
        }
        std::string returnString = "Component listening successfully\nAddress: " + component->getSelfAddress()
                                   + "\nPort: " + std::to_string(component->getBackgroundPort());
        return env->NewStringUTF(returnString.c_str());
    }catch (std::logic_error &e){
        std::string returnString = "Error with component startup: " + std::string(e.what());
        return env->NewStringUTF(returnString.c_str());
    }
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_startRDH(JNIEnv *env, jobject thiz) {
    try {
        if(component == nullptr){
            throw std::logic_error("No component available");
        }
        component->startResourceDiscoveryHub();
        auto hubs = component->getResourceDiscoveryConnectionEndpoint().getResourceDiscoveryHubs();
        if(hubs.empty()){
            return env->NewStringUTF("No hub started");
        }
        std::string returnString = "Resource Discovery Hub started\nAddress: " + hubs.at(0);
        return env->NewStringUTF(returnString.c_str());
    }catch (std::logic_error &e){
        std::string returnString = "Error with RDH startup: " + std::string(e.what());
        return env->NewStringUTF(returnString.c_str());
    }
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getComponentAddress(JNIEnv *env, jobject thiz) {
    std::string address = component->getSelfAddress() + ":" + std::to_string(component->getBackgroundPort());
    return env->NewStringUTF(address.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_endComponent(JNIEnv *env, jobject thiz) {
    // If component is nullptr delete does nothing
    delete component;
    component = nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_addRDH(JNIEnv *env, jobject thiz, jstring url) {
    try{
        jboolean isCopy = false;
        std::string rdhUrl = env->GetStringUTFChars(url, &isCopy);
        component->getResourceDiscoveryConnectionEndpoint().registerWithHub(rdhUrl);
        return true;
    } catch (std::logic_error &e) {
        return false;
    }
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getRDHUrls(JNIEnv *env, jobject thiz) {
    auto rdhs = component->getResourceDiscoveryConnectionEndpoint().getResourceDiscoveryHubs();
    jobjectArray ret = env->NewObjectArray(rdhs.size(),
                                           env->FindClass("java/lang/String"),
                                           env->NewStringUTF(""));
    for(int i =0; i < rdhs.size() ; i++){
        env->SetObjectArrayElement(ret,i, env->NewStringUTF(rdhs.at(i).c_str()));
    }
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_updateManifestWithHubs(JNIEnv *env,
                                                                          jobject thiz) {
    component->getResourceDiscoveryConnectionEndpoint().updateManifestWithHubs();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_deregisterFromAllHubs(JNIEnv *env,
                                                                         jobject thiz) {
    component->getResourceDiscoveryConnectionEndpoint().deRegisterFromAllHubs();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_closeRDH(JNIEnv *env, jobject thiz) {
    component->closeResourceDiscoveryHub();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_openRDH(JNIEnv *env, jobject thiz) {
    component->startResourceDiscoveryHub();
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getRdhAddress(JNIEnv *env, jobject thiz) {
    try {
        std::string address = component->getSelfAddress() + ":" +
                              std::to_string(component->getResourceDiscoveryHubPort());
        return env->NewStringUTF(address.c_str());
    } catch (std::logic_error &e) {
        return env->NewStringUTF("Error no RDH exists");
    }
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getComponents(JNIEnv *env, jobject thiz, jstring rdh_url) {
    try{
        jboolean isCopy = false;
        std::string rdhUrl = env->GetStringUTFChars(rdh_url, &isCopy);
        auto ids = component->getResourceDiscoveryConnectionEndpoint().getComponentIdsFromHub(rdhUrl);
        jobjectArray ret = env->NewObjectArray(ids.size(),
                                               env->FindClass("java/lang/String"),
                                               env->NewStringUTF(""));
        for(int i =0; i < ids.size() ; i++){
            env->SetObjectArrayElement(ret,i, env->NewStringUTF(ids.at(i).c_str()));
        }
        return ret;
    } catch (std::logic_error &e) {
        return env->NewObjectArray(0,env->FindClass("java/lang/String"),
                                   env->NewStringUTF(""));
    }
}