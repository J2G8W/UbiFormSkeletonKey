#include <jni.h>
#include <string>
#include <UbiForm/Component.h>
#include <list>

Component *component = nullptr;

void writeToText(const std::string &textToWrite, JNIEnv *env,
                 jobject textObject) {
    if (env != nullptr) {
        jclass TextViewClass = env->FindClass(
                "com/example/ubiformskeletonkey/GeneralConnectedActivity");
        jmethodID setText = env->GetMethodID(TextViewClass, "updateMainOutput",
                                             "(Ljava/lang/String;)V");
        jstring msg = env->NewStringUTF(textToWrite.c_str());
        env->CallVoidMethod(textObject, setText, msg);
    }
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_startComponent(JNIEnv *env, jobject thiz,
                                                                  jstring ip_address,
                                                                  jstring name) {
    try {
        // Reckon it is fixable
        if (component == nullptr) {
            jboolean isCopy = false;
            std::string componentUrl = env->GetStringUTFChars(ip_address, &isCopy);
            std::string componentName = env->GetStringUTFChars(name, &isCopy);
            component = new Component(componentUrl);
            component->getComponentManifest().setName(componentName);

            std::shared_ptr<EndpointSchema> notificationPublisherSchema = std::make_shared<EndpointSchema>();
            notificationPublisherSchema->addProperty("appName", ValueType::String);
            notificationPublisherSchema->addRequired("appName");
            notificationPublisherSchema->addProperty("messageText", ValueType::String);
            notificationPublisherSchema->addRequired("messageText");
            notificationPublisherSchema->addProperty("messageTitle", ValueType::String);
            notificationPublisherSchema->addRequired("messageTitle");
            notificationPublisherSchema->addProperty("phoneName", ValueType::String);
            notificationPublisherSchema->addProperty("time", ValueType::String);

            EndpointSchema colourSchema;
            colourSchema.addProperty("r", ValueType::Number);
            colourSchema.addProperty("g", ValueType::Number);
            colourSchema.addProperty("b", ValueType::Number);
            notificationPublisherSchema->setSubObject("colour", colourSchema);

            component->getComponentManifest().addEndpoint(ConnectionParadigm::Publisher,
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
}

std::list<std::string>
getOrderedUrls(const std::string &selfAddress, ComponentRepresentation &rep) {
    auto selfSubnet = selfAddress.substr(0, selfAddress.rfind('.'));
    std::list<std::string> urls;
    for (const auto &url: rep.getAllUrls()) {
        if (url.rfind(selfSubnet, 0) == 0) {
            urls.push_front(url);
        } else {
            urls.push_back(url);
        }
    }
    return urls;
}


extern "C"
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
        auto selfAddress = component->getSelfAddress();
        std::list<std::string> urls = getOrderedUrls(selfAddress, *rep);

        for (const auto &url : urls) {
            try {
                writeToText("Trying to connect on: " + url, env, activity_object);
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
Java_com_example_ubiformskeletonkey_UbiFormService_requestCloseEndpointsOfType(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jstring url,
                                                                               jstring endpoint_type,
                                                                               jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    std::string endpointType = env->GetStringUTFChars(endpoint_type, &isCopy);
    try {
        component->getBackgroundRequester().requestCloseEndpointsOfType(componentUrl, endpointType);
        writeToText("Successfully closed endpoints", env, activity_object);
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
Java_com_example_ubiformskeletonkey_UbiFormService_getEndpointDescriptors(JNIEnv *env, jobject thiz,
                                                                          jstring url,
                                                                          jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(url, &isCopy);
    try {
        auto endpoints = component->getBackgroundRequester().requestEndpointInfo(componentUrl);
        jobjectArray ret = env->NewObjectArray(endpoints.size(),
                                               env->FindClass("java/lang/String"),
                                               env->NewStringUTF(""));
        int i = 0;
        for (const auto &endpoint : endpoints) {
            std::string text = "ID: " + endpoint->getString("id") + "\nEndpoint Type: " +
                               endpoint->getString("endpointType") +
                               "\nCommunications Paradigm: " +
                               endpoint->getString("connectionParadigm") +
                               "\nCurrent State: " + endpoint->getString("endpointState");
            if (endpoint->hasMember("listenPort")) {
                text += "\nListening on port: " +
                        std::to_string(endpoint->getInteger("listenPort"));
            } else if (endpoint->hasMember("dialUrl")) {
                text += "\nDialled: " + endpoint->getString("dialUrl");
            }
            text += "\n";
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
Java_com_example_ubiformskeletonkey_UbiFormService_requestCloseEndpointsOfID(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jstring correct_component_url,
                                                                             jstring endpoint_id,
                                                                             jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(correct_component_url, &isCopy);
    std::string endpointId = env->GetStringUTFChars(endpoint_id, &isCopy);
    try {
        component->getBackgroundRequester().requestCloseEndpointOfId(componentUrl, endpointId);
        writeToText("Successfully closed endpoint", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_publishNotification(JNIEnv *env, jobject thiz,
                                                                       jstring app_name,
                                                                       jstring message_title,
                                                                       jstring message_text,
                                                                       jbyteArray icon_image) {
    jboolean isCopy = false;
    std::string appName = env->GetStringUTFChars(app_name, &isCopy);
    std::string notificationTitle = env->GetStringUTFChars(message_title, &isCopy);
    std::string extraText = env->GetStringUTFChars(message_text, &isCopy);


    auto endpoints = component->getEndpointsByType("notificationPublisher");
    if (!endpoints->empty()) {
        auto publisherEndpoint = component->castToDataSenderEndpoint(endpoints->at(0));
        EndpointMessage endpointMessage;
        endpointMessage.addMember("appName", appName);
        endpointMessage.addMember("messageTitle", notificationTitle);
        endpointMessage.addMember("messageText", extraText);
        endpointMessage.addMember("phoneName", component->getComponentManifest().getName());

        std::unique_ptr<EndpointMessage> colour = std::make_unique<EndpointMessage>();
        colour->addMember("r", 255);
        colour->addMember("g", 0);
        colour->addMember("b", 0);
        endpointMessage.addMoveObject("colour", std::move(colour));
        try {
            publisherEndpoint->sendMessage(endpointMessage);
            //writeToText("Success sending message " + sm.stringify(), env, activity_object);
        } catch (std::logic_error &e) {
            //writeToText(e.what(),env, activity_object);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestCreateAndListen(JNIEnv *env, jobject thiz,
                                                                          jstring component_url,
                                                                          jstring endpoint_type,
                                                                          jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(component_url, &isCopy);
    std::string endpointType = env->GetStringUTFChars(endpoint_type, &isCopy);
    try {
        int port = component->getBackgroundRequester().requestToCreateAndListen(componentUrl,
                                                                                endpointType);
        writeToText("Successfully started " + endpointType + " on port " + std::to_string(port),
                    env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_requestCreateAndDial(JNIEnv *env, jobject thiz,
                                                                        jstring component_url,
                                                                        jstring endpoint_type,
                                                                        jstring dial_url,
                                                                        jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(component_url, &isCopy);
    std::string endpointType = env->GetStringUTFChars(endpoint_type, &isCopy);
    std::string dialUrl = env->GetStringUTFChars(dial_url, &isCopy);
    try {
        std::vector<std::string> dialsUrls(1, dialUrl);
        component->getBackgroundRequester().requestToCreateAndDial(componentUrl, endpointType,
                                                                   dialsUrls);
        writeToText("Successfully started " + endpointType + " dialing " + dialUrl, env,
                    activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_gracefullyCloseRDH(JNIEnv *env, jobject thiz,
                                                                      jstring new_rdh_url,
                                                                      jobject activity_object) {
    jboolean isCopy = false;
    std::string newRdhUrl = env->GetStringUTFChars(new_rdh_url, &isCopy);

    std::vector<std::shared_ptr<ComponentRepresentation>> connections;
    try {
        connections = component->getResourceDiscoveryHubConnections();
    } catch (std::logic_error &e) {
        writeToText("Resource Discovery Hub not open", env, activity_object);
        return;
    }

    int port;
    try {
        port = component->getBackgroundRequester().requestCreateRDH(newRdhUrl);
    } catch (std::logic_error &e) {
        writeToText("Error creating RDH: " + std::string(e.what()), env, activity_object);
        return;
    }

    std::string rdhAddress = newRdhUrl.substr(0, newRdhUrl.rfind(':')) + ":" + std::to_string(port);

    std::string oldRdh = component->getSelfAddress() + ":" +
                         std::to_string(component->getResourceDiscoveryHubPort());

    std::vector<std::string> failed;
    for (const auto &connection:connections) {
        bool removed = false;
        bool added = false;
        for (const auto &url: getOrderedUrls(component->getSelfAddress(), *connection)) {
            try {
                if (!removed) {
                    component->getBackgroundRequester().requestRemoveRDH(
                            url + ":" + std::to_string(connection->getPort()),
                            oldRdh);
                }
                removed = true;
            } catch (NngError &e) {
                // Not the right URL
                continue;
            } catch (RemoteError &e) {
                // Right URL, but previous RDH wasn't right, still add right one
            }
            try {
                if (!added) {
                    component->getBackgroundRequester().requestAddRDH(
                            url + ":" + std::to_string(connection->getPort()), rdhAddress);
                }
                added = true;
                break;
            } catch (std::logic_error &e) {
                continue;
            }
        }
        if (!added) { failed.push_back(connection->getName()); }
    }

    if (failed.empty()) {
        writeToText("Successfully transferred all components", env, activity_object);
    } else {
        std::string returnMsg = "Failed to transfer: ";
        for (const auto &name:failed) {
            returnMsg.append(name + ", ");
        }
        writeToText(returnMsg, env, activity_object);
    }

    component->closeResourceDiscoveryHub();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_request3rdPartyListenThenRemoteDial(JNIEnv *env,
                                                                                       jobject thiz,
                                                                                       jstring listen_address,
                                                                                       jstring listen_endpoint_type,
                                                                                       jstring dial_endpoint_type,
                                                                                       jstring dialer_address,
                                                                                       jobject activity_object) {
    jboolean isCopy = false;
    std::string listenAddress = env->GetStringUTFChars(listen_address, &isCopy);
    std::string listenEndpointType = env->GetStringUTFChars(listen_endpoint_type, &isCopy);
    std::string dialEndpointType = env->GetStringUTFChars(dial_endpoint_type, &isCopy);
    std::string dialAddress = env->GetStringUTFChars(dialer_address, &isCopy);

    try {
        component->getBackgroundRequester().request3rdPartyListenThenRemoteDial(listenAddress,
                                                                                listenEndpointType,
                                                                                dialEndpointType,
                                                                                dialAddress);
        writeToText("Success 3rd party request", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_request3rdPartyRemoteListenThenDial(JNIEnv *env,
                                                                                       jobject thiz,
                                                                                       jstring requester_address,
                                                                                       jstring requester_endpoint_type,
                                                                                       jstring remote_endpoint_type,
                                                                                       jstring remote_address,
                                                                                       jint remote_port,
                                                                                       jobject activity_object) {
    jboolean isCopy = false;
    std::string requesterAddress = env->GetStringUTFChars(requester_address, &isCopy);
    std::string requestEndpointType = env->GetStringUTFChars(requester_endpoint_type, &isCopy);
    std::string remoteEndpointType = env->GetStringUTFChars(remote_endpoint_type, &isCopy);
    std::string remoteAddress = env->GetStringUTFChars(remote_address, &isCopy);

    try {
        component->getBackgroundRequester().request3rdPartyRemoteListenThenDial(requesterAddress,
                                                                                requestEndpointType,
                                                                                remoteEndpointType,
                                                                                remoteAddress,
                                                                                (int) remote_port);
        writeToText("Success 3rd party request", env, activity_object);
    } catch (std::logic_error &e) {
        writeToText(e.what(), env, activity_object);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ubiformskeletonkey_UbiFormService_addNewEndpointSchemaBasedOnOtherDevice(
        JNIEnv *env, jobject thiz, jstring component_url, jstring new_endpoint_type,
        jstring rdh_url, jstring third_party_component_id, jstring remote_endpoint_type,
        jobject activity_object) {
    jboolean isCopy = false;
    std::string componentUrl = env->GetStringUTFChars(component_url, &isCopy);
    std::string newEndpointType = env->GetStringUTFChars(new_endpoint_type, &isCopy);
    std::string rdhUrl = env->GetStringUTFChars(rdh_url, &isCopy);
    std::string remoteComponentId = env->GetStringUTFChars(third_party_component_id, &isCopy);
    std::string remoteEndpointType = env->GetStringUTFChars(remote_endpoint_type, &isCopy);
    std::unique_ptr<ComponentRepresentation> manifest;
    try {
        manifest = component->getResourceDiscoveryConnectionEndpoint().getComponentById(rdhUrl,
                                                                                        remoteComponentId);
    } catch (std::logic_error &e) {
        writeToText("Error getting component manifest" + std::string(e.what()), env,
                    activity_object);
        return;
    }

    if (!manifest->hasEndpoint(remoteEndpointType)) {
        writeToText("Manifest has no type " + remoteEndpointType + "  " + manifest->stringify(),
                    env, activity_object);
        return;
    }

    ConnectionParadigm remoteParadigm = convertToConnectionParadigm(
            manifest->getConnectionParadigm(remoteEndpointType));
    ConnectionParadigm newParadigm;


    switch (remoteParadigm) {
        case Pair:
            newParadigm = ConnectionParadigm::Pair;
            break;
        case Publisher:
            newParadigm = ConnectionParadigm::Subscriber;
            break;
        case Subscriber:
            newParadigm = ConnectionParadigm::Publisher;
            break;
        case Reply:
            newParadigm = ConnectionParadigm::Request;
            break;
        case Request:
            newParadigm = ConnectionParadigm::Reply;
            break;
    }

    std::shared_ptr<EndpointSchema> recvSchema = nullptr;
    std::shared_ptr<EndpointSchema> sendSchema = nullptr;
    switch (newParadigm) {
        case Publisher:
            sendSchema = manifest->getReceiverSchema(remoteEndpointType);
            break;
        case Subscriber:
            recvSchema = manifest->getSenderSchema(remoteEndpointType);
            break;
        case Pair:
        case Reply:
        case Request:
            sendSchema = manifest->getReceiverSchema(remoteEndpointType);
            recvSchema = manifest->getSenderSchema(remoteEndpointType);
            break;
    }

    try {
        component->getBackgroundRequester().requestChangeEndpoint(componentUrl, newParadigm,
                                                                  newEndpointType, recvSchema.get(),
                                                                  sendSchema.get());
    } catch (std::logic_error &e) {
        writeToText("Couldn't change endpoint: " + std::string(e.what()), env, activity_object);
        return;
    }

    writeToText("Success", env, activity_object);
}