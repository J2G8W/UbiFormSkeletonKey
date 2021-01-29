#include <jni.h>
#include <string>
#include <UbiForm/Component.h>

Component *component = nullptr;

void writeToText(const std::string &textToWrite, JNIEnv *env,
                 jobject textObject) {
    jclass TextViewClass = env->FindClass(
            "com/example/ubiformskeletonkey/GeneralConnectedActivity");
    jmethodID setText = env->GetMethodID(TextViewClass, "updateMainOutput",
                                         "(Ljava/lang/String;)V");
    jstring msg = env->NewStringUTF(textToWrite.c_str());
    env->CallVoidMethod(textObject, setText, msg);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_startComponent(JNIEnv *env, jobject thiz,
                                                                  jstring ip_address) {
    try {
        // Starting is fine, but get error trying to start new thread
        // Reckon it is fixable
        if (component == nullptr) {
            jboolean isCopy = false;
            std::string componentUrl = env->GetStringUTFChars(ip_address, &isCopy);
            component = new Component(componentUrl);

            std::shared_ptr<EndpointSchema> notificationPublisherSchema = std::make_shared<EndpointSchema>();
            notificationPublisherSchema->addProperty("title", ValueType::String);
            notificationPublisherSchema->addRequired("title");
            notificationPublisherSchema->addProperty("extraText", ValueType::String);
            notificationPublisherSchema->addRequired("extraText");
            component->getComponentManifest().addEndpoint(SocketType::Publisher,
                                                          "notificationPublisher",
                                                          nullptr, notificationPublisherSchema);

        }
        if (component->getBackgroundPort() == -1) {
            component->startBackgroundListen();
        }
        return env->NewStringUTF("");
    } catch (std::logic_error &e) {
        std::string returnString = "Error with component startup: " + std::string(e.what());
        return env->NewStringUTF(returnString.c_str());
    }
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_startRDH(JNIEnv *env, jobject thiz) {
    try {
        if (component == nullptr) {
            throw std::logic_error("No component available");
        }
        component->startResourceDiscoveryHub();
        auto hubs = component->getResourceDiscoveryConnectionEndpoint().getResourceDiscoveryHubs();
        if (hubs.empty()) {
            return env->NewStringUTF("No hub started");
        }
        return env->NewStringUTF("");
    } catch (std::logic_error &e) {
        std::string returnString = "Error with RDH startup: " + std::string(e.what());
        return env->NewStringUTF(returnString.c_str());
    }
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getComponentAddress(JNIEnv *env, jobject thiz) {
    std::string address =
            component->getSelfAddress() + ":" + std::to_string(component->getBackgroundPort());
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
Java_com_example_ubiformskeletonkey_UbiFormService_addRDH(JNIEnv *env, jobject thiz, jstring url,
                                                          jobject activity_object) {
    try {
        jboolean isCopy = false;
        std::string rdhUrl = env->GetStringUTFChars(url, &isCopy);
        component->getResourceDiscoveryConnectionEndpoint().registerWithHub(rdhUrl);
        writeToText("Success adding Resouce Discovery Hub", env, activity_object);
        return true;
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
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
    for (int i = 0; i < rdhs.size(); i++) {
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(rdhs.at(i).c_str()));
    }
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_updateManifestWithHubs(JNIEnv *env,
                                                                          jobject thiz,
                                                                          jobject activity_object) {
    component->getResourceDiscoveryConnectionEndpoint().updateManifestWithHubs();
    writeToText("Succesfully updated manifest with hubs", env, activity_object);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_deregisterFromAllHubs(JNIEnv *env,
                                                                         jobject thiz,
                                                                         jobject activity_object) {
    component->getResourceDiscoveryConnectionEndpoint().deRegisterFromAllHubs();
    writeToText("Successfully deregistered from hubs", env, activity_object);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_closeRDH(JNIEnv *env, jobject thiz,
                                                            jobject activity_object) {
    component->closeResourceDiscoveryHub();
    writeToText("Successfully closed Resource Discovery Hub", env, activity_object);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_openRDH(JNIEnv *env, jobject thiz,
                                                           jobject activity_object) {
    component->startResourceDiscoveryHub();
    writeToText("Successfully opened Resource Discovery Hub", env, activity_object);
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
Java_com_example_ubiformskeletonkey_UbiFormService_getComponentsFromRDH(JNIEnv *env, jobject thiz,
                                                                        jstring rdh_url,
                                                                        jobject activity_object) {
    try {
        jboolean isCopy = false;
        std::string rdhUrl = env->GetStringUTFChars(rdh_url, &isCopy);
        auto ids = component->getResourceDiscoveryConnectionEndpoint().getComponentIdsFromHub(
                rdhUrl);
        jobjectArray ret = env->NewObjectArray(ids.size(),
                                               env->FindClass("java/lang/String"),
                                               env->NewStringUTF(""));
        for (int i = 0; i < ids.size(); i++) {
            env->SetObjectArrayElement(ret, i, env->NewStringUTF(ids.at(i).c_str()));
        }
        return ret;
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
        return env->NewObjectArray(0, env->FindClass("java/lang/String"),
                                   env->NewStringUTF(""));
    }
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getCorrectRemoteAddress(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring rdh_url,
                                                                           jstring component_id,
                                                                           jobject activity_object) {
    jboolean isCopy = false;
    std::string rdhUrl = env->GetStringUTFChars(rdh_url, &isCopy);
    std::string id = env->GetStringUTFChars(component_id, &isCopy);
    try {
        auto rep = component->getResourceDiscoveryConnectionEndpoint().getComponentById(rdhUrl, id);

        std::string correctUrl;
        bool found = false;
        int port = rep->getPort();
        for (const auto &url : rep->getAllUrls()) {
            try {
                component->getBackgroundRequester().requestLocationsOfRDH(
                        url + ":" + std::to_string(port));
                correctUrl = url + ":" + std::to_string(port);
                found = true;
                break;
            } catch (std::logic_error &e) {
                continue;
            }
        }
        if (found) {
            return env->NewStringUTF(correctUrl.c_str());
        } else {
            writeToText("No URL was successfully connected to", env, activity_object);
        }
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
    return env->NewStringUTF("");
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestCloseSocketsOfType(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jstring url,
                                                                             jstring endpoint_type,
                                                                             jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    std::string endpointType = env->GetStringUTFChars(endpoint_type, &isCopy);
    try {
        component->getBackgroundRequester().requestCloseSocketOfType(componentUrl, endpointType);
        writeToText("Successfully closed sockets", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestCreateRDH(JNIEnv *env, jobject thiz,
                                                                    jstring url,
                                                                    jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    try {
        component->getBackgroundRequester().requestCreateRDH(componentUrl);
        writeToText("Successfully requested the creation of RDH", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestCloseRDH(JNIEnv *env, jobject thiz,
                                                                   jstring url,
                                                                   jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    try {
        component->getBackgroundRequester().requestCloseRDH(componentUrl);
        writeToText("Successfully closed RDH", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestAddRDH(JNIEnv *env, jobject thiz,
                                                                 jstring url, jstring rdh,
                                                                 jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    std::string rdhUrl = env->GetStringUTFChars(rdh, &isCopy);
    try {
        component->getBackgroundRequester().requestAddRDH(componentUrl, rdhUrl);
        writeToText("Successfull added RDH", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestRemoveRDH(JNIEnv *env, jobject thiz,
                                                                    jstring url, jstring rdh,
                                                                    jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    std::string rdhUrl = env->GetStringUTFChars(rdh, &isCopy);
    try {
        component->getBackgroundRequester().requestRemoveRDH(componentUrl, rdhUrl);
        writeToText("Successfully removed RDH", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_getSocketDescriptors(JNIEnv *env, jobject thiz,
                                                                        jstring url,
                                                                        jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    try {
        auto sockets = component->getBackgroundRequester().requestEndpointInfo(componentUrl);
        jobjectArray ret = env->NewObjectArray(sockets.size(),
                                               env->FindClass("java/lang/String"),
                                               env->NewStringUTF(""));
        int i = 0;
        for (const auto &socket : sockets) {
            std::string text = "ID: " + socket->getString("id") + "\nEndpoint Type: " +
                               socket->getString("endpointType") +
                               "\nSocket Type: " + socket->getString("socketType");
            if (socket->hasMember("listenPort")) {
                text += "\nListening on port: " + std::to_string(socket->getInteger("listenPort"));
            }
            else if (socket->hasMember("dialUrl")) {
                text += "\nDialled: " + socket->getString("dialUrl");
            }
            env->SetObjectArrayElement(ret, i++, env->NewStringUTF(text.c_str()));
        }
        return ret;
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
        return env->NewObjectArray(0, env->FindClass("java/lang/String"), env->NewStringUTF(""));
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestComponentManifest(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jstring url,
                                                                            jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    try {
        std::string manifest =
                component->getBackgroundRequester().requestComponentManifest(
                        componentUrl)->prettyStringify();
        return env->NewStringUTF(manifest.c_str());
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
        return env->NewStringUTF("");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestChangeComponentManifest(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jstring url,
                                                                                  jstring manifest,
                                                                                  jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    std::string manifestText = env->GetStringUTFChars(manifest, &isCopy);
    try {
        ComponentManifest componentManifest(manifestText.c_str(), component->getSystemSchemas());
        component->getBackgroundRequester().requestUpdateComponentManifest(componentUrl,
                                                                           componentManifest);
        writeToText("Successfully updated manifest", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_publishNotification(JNIEnv *env, jobject thiz,
                                                                       jstring title,
                                                                       jstring extra_text, jbyteArray icon_image) {
    jboolean isCopy = false;
    std::string notificationTitle = (title == nullptr) ? "No Title" : env->GetStringUTFChars(title,
                                                                                             &isCopy);
    std::string extraText = (extra_text == nullptr) ? "No Extra Text" : env->GetStringUTFChars(
            extra_text, &isCopy);

    jbyte* bufferPtr = env->GetByteArrayElements(icon_image, &isCopy);
    jsize lengthOfArray = env->GetArrayLength(icon_image);
    std::string iconText((char*) bufferPtr, lengthOfArray);

    auto endpoints = component->getEndpointsByType("notificationPublisher");
    if (!endpoints->empty()) {
        auto publisherEndpoint = component->castToDataSenderEndpoint(endpoints->at(0));
        SocketMessage sm;
        sm.addMember("title", notificationTitle);
        sm.addMember("extraText", extraText);
        sm.addMember("icon",iconText);
        try {
            publisherEndpoint->sendMessage(sm);
            //writeToText("Success sending message " + sm.stringify(), env, activity_object);
        } catch (std::logic_error &e) {
            //writeToText(e.what(),env, activity_object);
        }
    }
}