/*
 * android-specific implementation of the AdjustMarmalade extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "AdjustMarmalade_internal.h"

#include <jni.h>
#include "IwDebug.h"
#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include "rapidjson/document.h"

static jobject g_Obj;
static jmethodID g_adjust_Start;
static jmethodID g_adjust_TrackEvent;
static jmethodID g_adjust_SetEnabled;
static jmethodID g_adjust_IsEnabled;
static jmethodID g_adjust_SetOfflineMode;
static jmethodID g_adjust_SetDeviceToken;
static jmethodID g_adjust_SendFirstPackages;
static jmethodID g_adjust_AddSessionCallbackParameter;
static jmethodID g_adjust_AddSessionPartnerParameter;
static jmethodID g_adjust_RemoveSessionCallbackParameter;
static jmethodID g_adjust_RemoveSessionPartnerParameter;
static jmethodID g_adjust_ResetSessionCallbackParameters;
static jmethodID g_adjust_ResetSessionPartnerParameters;
static jmethodID g_adjust_OnPause;
static jmethodID g_adjust_OnResume;
static jmethodID g_adjust_SetReferrer;
static jmethodID g_adjust_GetGoogleAdId;
static jmethodID g_adjust_GetIdfa;
static jmethodID g_adjust_GetAdid;
static jmethodID g_adjust_GetAttribution;

char* get_json_string(rapidjson::Document &jsonDoc, const char* member_name) {
    if (!jsonDoc.HasMember(member_name)) {
        return NULL;
    }

    const char* source = jsonDoc[member_name].GetString();
    char* target = adjust_CopyString(source);
    
    return target;
}

adjust_attribution_data* get_attribution_data(const char* jsonCString) {
    adjust_attribution_data* attribution;
    rapidjson::Document jsonDoc;

    if (jsonDoc.Parse<0>(jsonCString).HasParseError()) {
        IwTrace(ADJUSTMARMALADE,("Error parsing attribution data json string!"));
        return NULL;
    }

    attribution = new adjust_attribution_data();
    
    attribution->tracker_token   = get_json_string(jsonDoc, "tracker_token");
    attribution->tracker_name    = get_json_string(jsonDoc, "tracker_name");
    attribution->network         = get_json_string(jsonDoc, "network");
    attribution->campaign        = get_json_string(jsonDoc, "campaign");
    attribution->ad_group        = get_json_string(jsonDoc, "ad_group");
    attribution->creative        = get_json_string(jsonDoc, "creative");
    attribution->click_label     = get_json_string(jsonDoc, "click_label");
    attribution->adid            = get_json_string(jsonDoc, "adid");

    return attribution;
}

adjust_session_success_data* get_session_success_data(const char* jsonCString) {
    adjust_session_success_data* sessionSuccess;
    rapidjson::Document jsonDoc;

    if (jsonDoc.Parse<0>(jsonCString).HasParseError()) {
        IwTrace(ADJUSTMARMALADE,("Error parsing session success data json string!"));
        return NULL;
    }

    sessionSuccess = new adjust_session_success_data();
    
    sessionSuccess->message       = get_json_string(jsonDoc, "message");
    sessionSuccess->timestamp     = get_json_string(jsonDoc, "timestamp");
    sessionSuccess->adid          = get_json_string(jsonDoc, "adid");
    sessionSuccess->json_response = get_json_string(jsonDoc, "json_response");

    return sessionSuccess;
}

adjust_session_failure_data* get_session_failure_data(const char* jsonCString) {
    adjust_session_failure_data* sessionFailure;
    rapidjson::Document jsonDoc;

    if (jsonDoc.Parse<0>(jsonCString).HasParseError()) {
        IwTrace(ADJUSTMARMALADE,("Error parsing session failure data json string!"));
        return NULL;
    }

    sessionFailure = new adjust_session_failure_data();
    
    sessionFailure->message       = get_json_string(jsonDoc, "message");
    sessionFailure->timestamp     = get_json_string(jsonDoc, "timestamp");
    sessionFailure->adid          = get_json_string(jsonDoc, "adid");
    sessionFailure->will_retry    = get_json_string(jsonDoc, "will_retry");
    sessionFailure->json_response = get_json_string(jsonDoc, "json_response");

    return sessionFailure;
}

adjust_event_success_data* get_event_success_data(const char* jsonCString) {
    adjust_event_success_data* eventSuccess;
    rapidjson::Document jsonDoc;

    if (jsonDoc.Parse<0>(jsonCString).HasParseError()) {
        IwTrace(ADJUSTMARMALADE,("Error parsing event success data json string!"));
        return NULL;
    }

    eventSuccess = new adjust_event_success_data();
    
    eventSuccess->message       = get_json_string(jsonDoc, "message");
    eventSuccess->timestamp     = get_json_string(jsonDoc, "timestamp");
    eventSuccess->event_token   = get_json_string(jsonDoc, "event_token");
    eventSuccess->adid          = get_json_string(jsonDoc, "adid");
    eventSuccess->json_response = get_json_string(jsonDoc, "json_response");

    return eventSuccess;
}

adjust_event_failure_data* get_event_failure_data(const char* jsonCString) {
    adjust_event_failure_data* eventFailure;
    rapidjson::Document jsonDoc;

    if (jsonDoc.Parse<0>(jsonCString).HasParseError()) {
        IwTrace(ADJUSTMARMALADE,("Error parsing event failure data json string!"));
        return NULL;
    }

    eventFailure = new adjust_event_failure_data();
    
    eventFailure->message       = get_json_string(jsonDoc, "message");
    eventFailure->timestamp     = get_json_string(jsonDoc, "timestamp");
    eventFailure->event_token   = get_json_string(jsonDoc, "event_token");
    eventFailure->adid          = get_json_string(jsonDoc, "adid");
    eventFailure->will_retry    = get_json_string(jsonDoc, "will_retry");
    eventFailure->json_response = get_json_string(jsonDoc, "json_response");

    return eventFailure;
}

void JNICALL the_attribution_callback(JNIEnv* env, jobject obj, jstring attributionString) {
    const char* attributionCString = env->GetStringUTFChars(attributionString, NULL);
    adjust_attribution_data* attribution = get_attribution_data(attributionCString);
    env->ReleaseStringUTFChars(attributionString, attributionCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_ATTRIBUTION_DATA,
                           attribution,
                           sizeof(*attribution),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupAttributionCallback,
                           (void*)attribution);
}

void JNICALL the_session_success_callback(JNIEnv* env, jobject obj, jstring sessionSuccessString) {
    const char* sessionSuccessCString = env->GetStringUTFChars(sessionSuccessString, NULL);
    adjust_session_success_data* session_success = get_session_success_data(sessionSuccessCString);
    env->ReleaseStringUTFChars(sessionSuccessString, sessionSuccessCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_SESSION_SUCCESS_DATA,
                           session_success,
                           sizeof(*session_success),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupSessionSuccessCallback,
                           (void*)session_success);
}

void JNICALL the_session_failure_callback(JNIEnv* env, jobject obj, jstring sessionFailureString) {
    const char* sessionFailureCString = env->GetStringUTFChars(sessionFailureString, NULL);
    adjust_session_failure_data* session_failure = get_session_failure_data(sessionFailureCString);
    env->ReleaseStringUTFChars(sessionFailureString, sessionFailureCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_SESSION_FAILURE_DATA,
                           session_failure,
                           sizeof(*session_failure),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupSessionFailureCallback,
                           (void*)session_failure);
}

void JNICALL the_event_success_callback(JNIEnv* env, jobject obj, jstring eventSuccessString) {
    const char* eventSuccessCString = env->GetStringUTFChars(eventSuccessString, NULL);
    adjust_event_success_data* event_success = get_event_success_data(eventSuccessCString);
    env->ReleaseStringUTFChars(eventSuccessString, eventSuccessCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_EVENT_SUCCESS_DATA,
                           event_success,
                           sizeof(*event_success),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupEventSuccessCallback,
                           (void*)event_success);
}

void JNICALL the_event_failure_callback(JNIEnv* env, jobject obj, jstring eventFailureString) {
    const char* eventFailureCString = env->GetStringUTFChars(eventFailureString, NULL);
    adjust_event_failure_data* event_failure = get_event_failure_data(eventFailureCString);
    env->ReleaseStringUTFChars(eventFailureString, eventFailureCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_EVENT_FAILURE_DATA,
                           event_failure,
                           sizeof(*event_failure),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupEventFailureCallback,
                           (void*)event_failure);
}

void JNICALL the_deeplink_callback(JNIEnv* env, jobject obj, jstring deeplinkString) {
    const char* deeplinkTempCString = env->GetStringUTFChars(deeplinkString, NULL);
    const char* deeplinkCString = adjust_CopyString(deeplinkTempCString);
    env->ReleaseStringUTFChars(deeplinkString, deeplinkTempCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_DEEPLINK_DATA,
                           (char*)deeplinkCString,
                           strlen(deeplinkCString));
}

void JNICALL the_deferred_deeplink_callback(JNIEnv* env, jobject obj, jstring deferredDeeplinkString) {
    const char* deferredDeeplinkTempCString = env->GetStringUTFChars(deferredDeeplinkString, NULL);
    const char* deferredDeeplinkCString = adjust_CopyString(deferredDeeplinkTempCString);
    env->ReleaseStringUTFChars(deferredDeeplinkString, deferredDeeplinkTempCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_DEFERRED_DEEPLINK_DATA,
                           (char*)deferredDeeplinkCString,
                           strlen(deferredDeeplinkCString));
}

void JNICALL the_google_ad_id_callback(JNIEnv* env, jobject obj, jstring googleAdIdString) {
    const char* googleAdIdTempCString = env->GetStringUTFChars(googleAdIdString, NULL);
    const char* googleAdIdCString = adjust_CopyString(googleAdIdTempCString);
    env->ReleaseStringUTFChars(googleAdIdString, googleAdIdTempCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_GOOGLE_AD_ID_DATA,
                           (char*)googleAdIdCString,
                           strlen(googleAdIdCString));
}

void JNICALL the_idfa_callback(JNIEnv* env, jobject obj, jstring idfaString) {
    const char* idfaTempCString = env->GetStringUTFChars(idfaString, NULL);
    const char* idfaCString = adjust_CopyString(idfaTempCString);
    env->ReleaseStringUTFChars(idfaString, idfaTempCString);

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_IDFA_DATA,
                           (char*)idfaCString,
                           strlen(idfaCString));
}

jobject create_global_java_dict(const adjust_param_type* params) {
    if (params == NULL) {
        return NULL;
    }

    // Get JNI env.
    JNIEnv* env = s3eEdkJNIGetEnv();

    // Get the HashMap class.
    jclass dict_cls = s3eEdkAndroidFindClass("java/util/HashMap");

    // Get its initial size constructor.
    jmethodID dict_cons = env->GetMethodID(dict_cls, "<init>", "(I)V");

    // Construct the java class with the default size.
    jobject dict_obj = env->NewObject(dict_cls, dict_cons, params->size());

    // Get the put method.
    jmethodID put_method = env->GetMethodID(dict_cls, "put", 
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    for (adjust_param_type::const_iterator pos = params->begin();pos != params->end(); ++pos) {
        jstring key = env->NewStringUTF(pos->first);
        jstring value = env->NewStringUTF(pos->second);

        env->CallObjectMethod(dict_obj, put_method, key, value);
    }
    
    jobject dict_jglobal = env->NewGlobalRef(dict_obj);

    return dict_jglobal;
}

s3eResult AdjustMarmaladeInit_platform() {
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    const JNINativeMethod methods[] = {
        {"attributionCallback", "(Ljava/lang/String;)V",(void*)&the_attribution_callback},
        {"eventSuccessCallback", "(Ljava/lang/String;)V",(void*)&the_event_success_callback},
        {"eventFailureCallback", "(Ljava/lang/String;)V",(void*)&the_event_failure_callback},
        {"sessionSuccessCallback", "(Ljava/lang/String;)V",(void*)&the_session_success_callback},
        {"sessionFailureCallback", "(Ljava/lang/String;)V",(void*)&the_session_failure_callback},
        {"deeplinkCallback", "(Ljava/lang/String;)V",(void*)&the_deeplink_callback},
        {"deferredDeeplinkCallback", "(Ljava/lang/String;)V",(void*)&the_deferred_deeplink_callback},
        {"googleAdIdCallback", "(Ljava/lang/String;)V",(void*)&the_google_ad_id_callback},
        {"idfaCallback", "(Ljava/lang/String;)V",(void*)&the_idfa_callback}
    };

    // Get the extension class.
    jclass cls = s3eEdkAndroidFindClass("AdjustMarmalade");

    if (!cls) {
        goto fail;
    }

    // Get its constructor.
    cons = env->GetMethodID(cls, "<init>", "()V");

    if (!cons) {
        goto fail;
    }

    // Construct the java class.
    obj = env->NewObject(cls, cons);

    if (!obj)
    {
        goto fail;
    }

    // Get all the extension methods.
    g_adjust_Start = env->GetMethodID(cls, "adjust_Start", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;DZZZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZZZZZZZZ)V");
    
    if (!g_adjust_Start) {
        goto fail;
    }

    g_adjust_TrackEvent = env->GetMethodID(cls, "adjust_TrackEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;DLjava/util/Map;Ljava/util/Map;Z)V");
    
    if (!g_adjust_TrackEvent) {
        goto fail;
    }

    g_adjust_SetEnabled = env->GetMethodID(cls, "adjust_SetEnabled", "(Z)V");
    
    if (!g_adjust_SetEnabled) {
        goto fail;
    }

    g_adjust_IsEnabled = env->GetMethodID(cls, "adjust_IsEnabled", "()Z");
    
    if (!g_adjust_IsEnabled) {
        goto fail;
    }

    g_adjust_SetOfflineMode = env->GetMethodID(cls, "adjust_SetOfflineMode", "(Z)V");
    
    if (!g_adjust_SetOfflineMode) {
        goto fail;
    }

    g_adjust_SetReferrer = env->GetMethodID(cls, "adjust_SetReferrer", "(Ljava/lang/String;)V");
    
    if (!g_adjust_SetReferrer) {
        goto fail;
    }

    g_adjust_SetDeviceToken = env->GetMethodID(cls, "adjust_SetDeviceToken", "(Ljava/lang/String;)V");

    if (!g_adjust_SetDeviceToken) {
        goto fail;
    }

    g_adjust_SendFirstPackages = env->GetMethodID(cls, "adjust_SendFirstPackages", "()V");

    if (!g_adjust_SendFirstPackages) {
        goto fail;
    }

    g_adjust_AddSessionCallbackParameter = env->GetMethodID(cls, "adjust_AddSessionCallbackParameter", "(Ljava/lang/String;Ljava/lang/String;)V");

    if (!g_adjust_AddSessionCallbackParameter) {
        goto fail;
    }

    g_adjust_AddSessionPartnerParameter = env->GetMethodID(cls, "adjust_AddSessionPartnerParameter", "(Ljava/lang/String;Ljava/lang/String;)V");

    if (!g_adjust_AddSessionPartnerParameter) {
        goto fail;
    }

    g_adjust_RemoveSessionCallbackParameter = env->GetMethodID(cls, "adjust_RemoveSessionCallbackParameter", "(Ljava/lang/String;)V");

    if (!g_adjust_RemoveSessionCallbackParameter) {
        goto fail;
    }

    g_adjust_RemoveSessionPartnerParameter = env->GetMethodID(cls, "adjust_RemoveSessionPartnerParameter", "(Ljava/lang/String;)V");

    if (!g_adjust_RemoveSessionPartnerParameter) {
        goto fail;
    }

    g_adjust_ResetSessionCallbackParameters = env->GetMethodID(cls, "adjust_ResetSessionCallbackParameters", "()V");

    if (!g_adjust_ResetSessionCallbackParameters) {
        goto fail;
    }

    g_adjust_ResetSessionPartnerParameters = env->GetMethodID(cls, "adjust_ResetSessionPartnerParameters", "()V");

    if (!g_adjust_ResetSessionPartnerParameters) {
        goto fail;
    }

    g_adjust_GetGoogleAdId = env->GetMethodID(cls, "adjust_GetGoogleAdId", "()V");
    
    if (!g_adjust_GetGoogleAdId) {
        goto fail;
    }

    g_adjust_GetIdfa = env->GetMethodID(cls, "adjust_GetIdfa", "()V");
    
    if (!g_adjust_GetIdfa) {
        goto fail;
    }

    g_adjust_GetAdid = env->GetMethodID(cls, "adjust_GetAdid", "()Ljava/lang/String;");
    
    if (!g_adjust_GetAdid) {
        goto fail;
    }

    g_adjust_GetAttribution = env->GetMethodID(cls, "adjust_GetAttribution", "()Lcom/adjust/sdk/AdjustAttribution;");
    
    if (!g_adjust_GetAttribution) {
        goto fail;
    }

    // Register the native hooks.
    if (env->RegisterNatives(cls, methods,sizeof(methods)/sizeof(methods[0]))) {
        goto fail;
    }

    IwTrace(ADJUSTMARMALADE, ("ADJUSTMARMALADE initialization successful!"));
    
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();

    if (exc) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        IwTrace(AdjustMarmalade, ("Error: One or more java methods could not be found!"));
    }

    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    return S3E_RESULT_ERROR;
}

void AdjustMarmaladeTerminate_platform() {
    // Add any platform-specific termination code here.
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->DeleteGlobalRef(g_Obj);
    g_Obj = NULL;
}

s3eResult adjust_Start_platform(adjust_config* config) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jAppToken = env->NewStringUTF(config->app_token);
    jstring jEnvironment = env->NewStringUTF(config->environment);
    jstring jLogLevel = env->NewStringUTF(config->log_level);
    jstring jSdkPrefix = env->NewStringUTF(config->sdk_prefix);
    jstring jProcessName = env->NewStringUTF(config->process_name);
    jstring jDefaultTracker = env->NewStringUTF(config->default_tracker);
    jstring jUserAgent = env->NewStringUTF(config->user_agent);
    jboolean jIsEventBufferingEnabled = JNI_FALSE;
    jboolean jIsSendingInBackgroundEnabled = JNI_FALSE;
    jboolean jShouldDeferredDeeplinkBeOpened = JNI_FALSE;
    jboolean jIsAttributionCallbackSet = JNI_FALSE;
    jboolean jIsEventSuccessCallbackSet = JNI_FALSE;
    jboolean jIsEventFailureCallbackSet = JNI_FALSE;
    jboolean jIsSessionSuccessCallbackSet = JNI_FALSE;
    jboolean jIsSessionFailureCallbackSet = JNI_FALSE;
    jboolean jIsDeeplinkCallbackSet = JNI_FALSE;
    jboolean jIsDeferredDeeplinkCallbackSet = JNI_FALSE;
    jboolean jIsGoogleAdIdCallbackSet = JNI_FALSE;
    jboolean jIsIdfaCallbackSet = JNI_FALSE;
    jdouble jDelayStart = config->delay_start != NULL ? *(config->delay_start) : -1;

    if (config->is_event_buffering_enabled != NULL) {
        jIsEventBufferingEnabled = (jboolean)(*(config->is_event_buffering_enabled));
    }

    if (config->is_sending_in_background_enabled != NULL) {
        jIsSendingInBackgroundEnabled = (jboolean)(*(config->is_sending_in_background_enabled));
    }

    if (config->should_deferred_deeplink_be_opened != NULL) {
        jShouldDeferredDeeplinkBeOpened = (jboolean)(*(config->should_deferred_deeplink_be_opened));
    }

    if (config->is_attribution_delegate_set != NULL) {
        jIsAttributionCallbackSet = (jboolean)(*(config->is_attribution_delegate_set));

        if (jIsAttributionCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_ATTRIBUTION_DATA, (s3eCallback)config->attribution_callback, NULL, false);
        }
    }

    if (config->is_event_success_delegate_set != NULL) {
        jIsEventSuccessCallbackSet = (jboolean)(*(config->is_event_success_delegate_set));

        if (jIsEventSuccessCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_EVENT_SUCCESS_DATA, (s3eCallback)config->event_success_callback, NULL, false);
        }
    }

    if (config->is_event_failure_delegate_set != NULL) {
        jIsEventFailureCallbackSet = (jboolean)(*(config->is_event_failure_delegate_set));

        if (jIsEventFailureCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_EVENT_FAILURE_DATA, (s3eCallback)config->event_failure_callback, NULL, false);
        }
    }

    if (config->is_session_success_delegate_set != NULL) {
        jIsSessionSuccessCallbackSet = (jboolean)(*(config->is_session_success_delegate_set));

        if (jIsSessionSuccessCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_SESSION_SUCCESS_DATA, (s3eCallback)config->session_success_callback, NULL, false);
        }
    }

    if (config->is_session_failure_delegate_set != NULL) {
        jIsSessionFailureCallbackSet = (jboolean)(*(config->is_session_failure_delegate_set));

        if (jIsSessionFailureCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_SESSION_FAILURE_DATA, (s3eCallback)config->session_failure_callback, NULL, false);
        }
    }

    if (config->is_deeplink_delegate_set != NULL) {
        jIsDeeplinkCallbackSet = (jboolean)(*(config->is_deeplink_delegate_set));

        if (jIsDeeplinkCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_DEEPLINK_DATA, (s3eCallback)config->deeplink_callback, NULL, false);
        }
    }

    if (config->is_deferred_deeplink_delegate_set != NULL) {
        jIsDeferredDeeplinkCallbackSet = (jboolean)(*(config->is_deferred_deeplink_delegate_set));

        if (jIsDeferredDeeplinkCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_DEFERRED_DEEPLINK_DATA, (s3eCallback)config->deferred_deeplink_callback, NULL, false);
        }
    }

    if (config->is_google_ad_id_delegate_set != NULL) {
        jIsGoogleAdIdCallbackSet = (jboolean)(*(config->is_google_ad_id_delegate_set));

        if (jIsGoogleAdIdCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_GOOGLE_AD_ID_DATA, (s3eCallback)config->google_ad_id_callback, NULL, false);
        }
    }

    if (config->is_idfa_delegate_set != NULL) {
        jIsIdfaCallbackSet = (jboolean)(*(config->is_idfa_delegate_set));

        if (jIsIdfaCallbackSet == JNI_TRUE) {
            EDK_CALLBACK_REG(ADJUST, ADJUST_IDFA_DATA, (s3eCallback)config->idfa_callback, NULL, false);
        }
    }

    env->CallVoidMethod(g_Obj, 
        g_adjust_Start,
        jAppToken,
        jEnvironment,
        jLogLevel,
        jSdkPrefix, 
        jDelayStart,
        jIsEventBufferingEnabled,
        jIsSendingInBackgroundEnabled,
        jShouldDeferredDeeplinkBeOpened,
        jProcessName,
        jDefaultTracker,
        jUserAgent,
        jIsAttributionCallbackSet,
        jIsSessionSuccessCallbackSet,
        jIsSessionFailureCallbackSet,
        jIsEventSuccessCallbackSet,
        jIsEventFailureCallbackSet,
        jIsDeeplinkCallbackSet,
        jIsDeferredDeeplinkCallbackSet,
        jIsGoogleAdIdCallbackSet,
        jIsIdfaCallbackSet);

    env->DeleteLocalRef(jAppToken);
    env->DeleteLocalRef(jEnvironment);
    env->DeleteLocalRef(jLogLevel);
    env->DeleteLocalRef(jProcessName);
    env->DeleteLocalRef(jDefaultTracker);
    env->DeleteLocalRef(jUserAgent);

    return (s3eResult)0;
}

s3eResult adjust_TrackEvent_platform(adjust_event* event) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jEventToken = env->NewStringUTF(event->event_token);
    jstring jCurrency = env->NewStringUTF(event->currency);
    jstring jTransactionId = env->NewStringUTF(event->transaction_id);
    jstring jReceipt = env->NewStringUTF(event->receipt);

    jobject jCallbackParams = create_global_java_dict(event->callback_params);
    jobject jPartnerParams = create_global_java_dict(event->partner_params);

    jdouble jRevenue = event->revenue != NULL ? *(event->revenue) : -1;
    jboolean jIsReceiptSet = event->is_receipt_set != NULL ? *(event->is_receipt_set) : JNI_FALSE;

    env->CallVoidMethod(g_Obj, 
        g_adjust_TrackEvent,
        jEventToken,
        jCurrency,
        jTransactionId,
        jReceipt,
        jRevenue,
        jCallbackParams,
        jPartnerParams,
        jIsReceiptSet);

    env->DeleteLocalRef(jEventToken);
    env->DeleteLocalRef(jCurrency);
    env->DeleteLocalRef(jTransactionId);
    env->DeleteLocalRef(jReceipt);
    env->DeleteGlobalRef(jCallbackParams);
    env->DeleteGlobalRef(jPartnerParams);

    return (s3eResult)0;
}

s3eResult adjust_SetEnabled_platform(bool is_enabled) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    env->CallVoidMethod(g_Obj, g_adjust_SetEnabled, is_enabled);
    
    return (s3eResult)0;
}

s3eResult adjust_IsEnabled_platform(bool& is_enabled_out) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jboolean isEnabled_java = env->CallBooleanMethod(g_Obj, g_adjust_IsEnabled);

    is_enabled_out = (bool)isEnabled_java;

    return (s3eResult)0;
}

s3eResult adjust_SetOfflineMode_platform(bool is_offline_mode_enabled) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    env->CallVoidMethod(g_Obj, g_adjust_SetOfflineMode, is_offline_mode_enabled);

    return (s3eResult)0;
}

s3eResult adjust_SetReferrer_platform(const char* referrer) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jReferrer = env->NewStringUTF(referrer);
    
    env->CallVoidMethod(g_Obj, g_adjust_SetReferrer, jReferrer);
    env->DeleteLocalRef(jReferrer);

    return (s3eResult)0;
}

s3eResult adjust_SetDeviceToken_platform(const char* device_token) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jDeviceToken = env->NewStringUTF(device_token);
    
    env->CallVoidMethod(g_Obj, g_adjust_SetDeviceToken, jDeviceToken);
    env->DeleteLocalRef(jDeviceToken);

    return (s3eResult)0;
}

s3eResult adjust_SendFirstPackages_platform() {
    JNIEnv* env = s3eEdkJNIGetEnv();

    env->CallVoidMethod(g_Obj, g_adjust_SendFirstPackages);

    return (s3eResult)0;
}

s3eResult adjust_AddSessionCallbackParameter_platform(const char* key, const char* value) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jKey = env->NewStringUTF(key);
    jstring jValue = env->NewStringUTF(value);
    
    env->CallVoidMethod(g_Obj, g_adjust_AddSessionCallbackParameter, jKey, jValue);
    env->DeleteLocalRef(jKey);
    env->DeleteLocalRef(jValue);

    return (s3eResult)0;
}

s3eResult adjust_AddSessionPartnerParameter_platform(const char* key, const char* value) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jKey = env->NewStringUTF(key);
    jstring jValue = env->NewStringUTF(value);
    
    env->CallVoidMethod(g_Obj, g_adjust_AddSessionPartnerParameter, jKey, jValue);
    env->DeleteLocalRef(jKey);
    env->DeleteLocalRef(jValue);

    return (s3eResult)0;
}

s3eResult adjust_RemoveSessionCallbackParameter_platform(const char* key) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jKey = env->NewStringUTF(key);
    
    env->CallVoidMethod(g_Obj, g_adjust_RemoveSessionCallbackParameter, jKey);
    env->DeleteLocalRef(jKey);

    return (s3eResult)0;
}

s3eResult adjust_RemoveSessionPartnerParameter_platform(const char* key) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jKey = env->NewStringUTF(key);
    
    env->CallVoidMethod(g_Obj, g_adjust_RemoveSessionPartnerParameter, jKey);
    env->DeleteLocalRef(jKey);

    return (s3eResult)0;
}

s3eResult adjust_ResetSessionCallbackParameters_platform() {
    JNIEnv* env = s3eEdkJNIGetEnv();

    env->CallVoidMethod(g_Obj, g_adjust_ResetSessionCallbackParameters);

    return (s3eResult)0;
}
s3eResult adjust_ResetSessionPartnerParameters_platform() {
    JNIEnv* env = s3eEdkJNIGetEnv();

    env->CallVoidMethod(g_Obj, g_adjust_ResetSessionPartnerParameters);

    return (s3eResult)0;
}

s3eResult adjust_GetGoogleAdId_platform() {
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_adjust_GetGoogleAdId);

    return (s3eResult)0;
}

s3eResult adjust_GetIdfa_platform() {
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_adjust_GetIdfa);

    return (s3eResult)0;
}

s3eResult adjust_GetAdid_platform(char** adid) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jstring jAdid = (jstring)env->CallObjectMethod(g_Obj, g_adjust_GetAdid);

    const char* adidCStr = env->GetStringUTFChars(jAdid, NULL);
    *adid = (char*)adjust_CopyString(adidCStr);

    env->ReleaseStringUTFChars(jAdid, adidCStr);
    env->DeleteLocalRef(jAdid);

    return (s3eResult)0;
}

s3eResult adjust_GetAttribution_platform(adjust_attribution_data* attribution) {
    JNIEnv* env = s3eEdkJNIGetEnv();

    jobject jAttribution = env->CallObjectMethod(g_Obj, g_adjust_GetAttribution);
    jclass clsAdjustAttribution = env->FindClass("com/adjust/sdk/AdjustAttribution");

    jfieldID fTrackerTokenID = env->GetFieldID(clsAdjustAttribution, "trackerToken", "Ljava/lang/String;");
    jfieldID fTrackerNameID = env->GetFieldID(clsAdjustAttribution, "trackerName", "Ljava/lang/String;");
    jfieldID fNetworkID = env->GetFieldID(clsAdjustAttribution, "network", "Ljava/lang/String;");
    jfieldID fCampaignID = env->GetFieldID(clsAdjustAttribution, "campaign", "Ljava/lang/String;");
    jfieldID fAdgroupID = env->GetFieldID(clsAdjustAttribution, "adgroup", "Ljava/lang/String;");
    jfieldID fCreativeID = env->GetFieldID(clsAdjustAttribution, "creative", "Ljava/lang/String;");
    jfieldID fClickLabelID = env->GetFieldID(clsAdjustAttribution, "clickLabel", "Ljava/lang/String;");
    jfieldID fAdidID = env->GetFieldID(clsAdjustAttribution, "adid", "Ljava/lang/String;");

    jstring jTrackerToken = (jstring)env->GetObjectField(jAttribution, fTrackerTokenID);
    jstring jTrackerName = (jstring)env->GetObjectField(jAttribution, fTrackerNameID);
    jstring jNetwork = (jstring)env->GetObjectField(jAttribution, fNetworkID);
    jstring jCampaign = (jstring)env->GetObjectField(jAttribution, fCampaignID);
    jstring jAdgroup = (jstring)env->GetObjectField(jAttribution, fAdgroupID);
    jstring jCreative = (jstring)env->GetObjectField(jAttribution, fCreativeID);
    jstring jClickLabel = (jstring)env->GetObjectField(jAttribution, fClickLabelID);
    jstring jAdid = (jstring)env->GetObjectField(jAttribution, fAdidID);

    if (NULL != jTrackerToken) {
        const char* trackerTokenCStr = env->GetStringUTFChars(jTrackerToken, NULL);
        attribution->tracker_token = (char*)adjust_CopyString(trackerTokenCStr);

        env->ReleaseStringUTFChars(jTrackerToken, trackerTokenCStr);
        env->DeleteLocalRef(jTrackerToken);
    } else {
        attribution->tracker_token = NULL;
    }

    if (NULL != jTrackerName) {
        const char* trackerNameCStr = env->GetStringUTFChars(jTrackerName, NULL);
        attribution->tracker_name = (char*)adjust_CopyString(trackerNameCStr);

        env->ReleaseStringUTFChars(jTrackerName, trackerNameCStr);
        env->DeleteLocalRef(jTrackerName);
    } else {
        attribution->tracker_name = NULL;
    }

    if (NULL != jNetwork) {
        const char* networkCStr = env->GetStringUTFChars(jNetwork, NULL);
        attribution->network = (char*)adjust_CopyString(networkCStr);

        env->ReleaseStringUTFChars(jNetwork, networkCStr);
        env->DeleteLocalRef(jNetwork);
    } else {
        attribution->network = NULL;
    }

    if (NULL != jCampaign) {
        const char* campaignCStr = env->GetStringUTFChars(jCampaign, NULL);
        attribution->campaign = (char*)adjust_CopyString(campaignCStr);

        env->ReleaseStringUTFChars(jCampaign, campaignCStr);
        env->DeleteLocalRef(jCampaign);
    } else {
        attribution->campaign = NULL;
    }

    if (NULL != jAdgroup) {
        const char* adgroupCStr = env->GetStringUTFChars(jAdgroup, NULL);
        attribution->ad_group = (char*)adjust_CopyString(adgroupCStr);

        env->ReleaseStringUTFChars(jAdgroup, adgroupCStr);
        env->DeleteLocalRef(jAdgroup);
    } else {
        attribution->ad_group = NULL;
    }

    if (NULL != jCreative) {
        const char* creativeCStr = env->GetStringUTFChars(jCreative, NULL);
        attribution->creative = (char*)adjust_CopyString(creativeCStr);

        env->ReleaseStringUTFChars(jCreative, creativeCStr);
        env->DeleteLocalRef(jCreative);
    } else {
        attribution->creative = NULL;
    }

    if (NULL != jClickLabel) {
        const char* clickLabelCStr = env->GetStringUTFChars(jClickLabel, NULL);
        attribution->click_label = (char*)adjust_CopyString(clickLabelCStr);

        env->ReleaseStringUTFChars(jClickLabel, clickLabelCStr);
        env->DeleteLocalRef(jClickLabel);
    } else {
        attribution->click_label = NULL;
    }

    if (NULL != jAdid) {
        const char* adidCStr = env->GetStringUTFChars(jAdid, NULL);
        attribution->adid = (char*)adjust_CopyString(adidCStr);

        env->ReleaseStringUTFChars(jAdid, adidCStr);
        env->DeleteLocalRef(jAdid);
    } else {
        attribution->adid = NULL;
    }

    env->DeleteLocalRef(jAttribution);

    return (s3eResult)0;
}
