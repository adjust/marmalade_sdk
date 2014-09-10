/*
 * android-specific implementation of the AdjustMarmalade extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "AdjustMarmalade_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"
//#include <exception>
#include <iostream>
#include <sstream>
//using namespace rapidjson;
//#include "jsoncpp/json/json.h"
//#include "cJSON-for-marmalade/cJSON.h"

#define S3E_DEVICE_ADJUST                   S3E_EXT_ADJUSTMARMALADE_HASH

typedef enum s3eAdjustCallback
{
    S3E_ADJUST_CALLBACK_ADJUST_RESPONSE_DATA,
    S3E_ADJUST_CALLBACK_MAX
} s3eAdjustCallback;

static jobject g_Obj;
static jmethodID g_AppDidLaunch;
static jmethodID g_TrackEvent;
static jmethodID g_TrackRevenue;
static jmethodID g_setEnabled;
static jmethodID g_isEnabled;
static jmethodID g_SetResponseDelegate;

//response_data_delegate adjust_delegate;

//extern void trace_response_data(response_data* response);

void trace_response_data_internal(response_data* response) 
{
    if (response == NULL) {
        IwTrace(ADJUSTMARMALADE,("response null"));
        return;
    }

    IwTrace(ADJUSTMARMALADE,("response not null"));
    
    if (!response->activityKind.empty()) {
        IwTrace(ADJUSTMARMALADE,("activityKind"));
        IwTrace(ADJUSTMARMALADE,(response->activityKind.c_str()));
    }
    IwTrace(ADJUSTMARMALADE,("success"));
    IwTrace(ADJUSTMARMALADE,(response->success ? "true" : "false"));

    IwTrace(ADJUSTMARMALADE,("willRetry"));
    IwTrace(ADJUSTMARMALADE,(response->willRetry ? "true" : "false"));

    if (!response->error.empty()) {
        IwTrace(ADJUSTMARMALADE,("error"));
        IwTrace(ADJUSTMARMALADE,(response->error.c_str()));
    }
    if (!response->trackerToken.empty()) {
        IwTrace(ADJUSTMARMALADE,("trackerToken"));
        IwTrace(ADJUSTMARMALADE,(response->trackerToken.c_str()));
    }
    if (!response->trackerName.empty()) {
        IwTrace(ADJUSTMARMALADE,("trackerName"));
        IwTrace(ADJUSTMARMALADE,(response->trackerName.c_str()));
    } 
    if (!response->network.empty()) {
        IwTrace(ADJUSTMARMALADE,("network"));
        IwTrace(ADJUSTMARMALADE,(response->network.c_str()));
    }
    if (!response->campaign.empty()) {
        IwTrace(ADJUSTMARMALADE,("campaign"));
        IwTrace(ADJUSTMARMALADE,(response->campaign.c_str()));
    }
    if (!response->adgroup.empty()) {
        IwTrace(ADJUSTMARMALADE,("adgroup"));
        IwTrace(ADJUSTMARMALADE,(response->adgroup.c_str()));
    }
    if (!response->creative.empty()) {
        IwTrace(ADJUSTMARMALADE,("creative"));
        IwTrace(ADJUSTMARMALADE,(response->creative.c_str()));
    }
}

s3eResult AdjustMarmaladeInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;
    const JNINativeMethod methods[] =
    {
        {"responseDataCallback","(Ljava/lang/String;)V",(void*)&responseData_callback}
    };;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("AdjustMarmalade");
    if (!cls)
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    // Get all the extension methods
    g_AppDidLaunch = env->GetMethodID(cls, "AppDidLaunch", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V");
    if (!g_AppDidLaunch)
        goto fail;

    g_TrackEvent = env->GetMethodID(cls, "TrackEvent", "(Ljava/lang/String;Ljava/util/Map;)V");
    if (!g_TrackEvent)
        goto fail;

    g_TrackRevenue = env->GetMethodID(cls, "TrackRevenue", "(DLjava/lang/String;Ljava/util/Map;)V");
    if (!g_TrackRevenue)
        goto fail;

    g_setEnabled = env->GetMethodID(cls, "setEnabled", "(Z)V");
    if (!g_setEnabled)
        goto fail;

    g_isEnabled = env->GetMethodID(cls, "isEnabled", "()Z");
    if (!g_isEnabled)
        goto fail;

    g_SetResponseDelegate = env->GetMethodID(cls, "SetResponseDelegate", "()V");
    if (!g_SetResponseDelegate)
        goto fail;

    // Register the native hooks
    if (env->RegisterNatives(cls, methods,sizeof(methods)/sizeof(methods[0])))
        goto fail;


    IwTrace(ADJUSTMARMALADE, ("ADJUSTMARMALADE init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(AdjustMarmalade, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;

}

void AdjustMarmaladeTerminate_platform()
{
    // Add any platform-specific termination code here
}

s3eResult AppDidLaunch_platform(const char* appToken, const char* environment, const char* sdkPrefix, const char* logLevel, bool eventBuffering)
{
    // get JNI env
    JNIEnv* env = s3eEdkJNIGetEnv();
    // convert cstring's to jstring's
    jstring appToken_jstr = env->NewStringUTF(appToken);
    jstring environment_jstr = env->NewStringUTF(environment);
    jstring sdkPrefix_jstr = env->NewStringUTF(sdkPrefix);
    jstring logLevel_jstr = env->NewStringUTF(logLevel);
    // call java track app did launch
    env->CallVoidMethod(g_Obj, g_AppDidLaunch, appToken_jstr, environment_jstr, sdkPrefix_jstr, logLevel_jstr, eventBuffering);
    
    return (s3eResult)0;
}

s3eResult TrackEvent_platform(const char* eventToken, const param_type* params)
{    
    // get JNI env
    JNIEnv* env = s3eEdkJNIGetEnv();
    // convert cstring to jstring
    jstring eventToken_jstr = env->NewStringUTF(eventToken);
    // convert to java Dictionary as a global reference
    jobject dict_jglobal = create_global_java_dict(params);
    // call java track event
    env->CallVoidMethod(g_Obj, g_TrackEvent, eventToken_jstr, dict_jglobal); 
    // free global ref
    env->DeleteGlobalRef(dict_jglobal);

    return (s3eResult)0;
}

s3eResult TrackRevenue_platform(double cents, const char* eventToken, const param_type* params)
{
    // get JNI env
    JNIEnv* env = s3eEdkJNIGetEnv();
    // convert cstring to jstring
    jstring eventToken_jstr = env->NewStringUTF(eventToken);
    // convert to java Dictionary as a global reference
    jobject dict_jglobal = create_global_java_dict(params);
    // call java track revenue
    env->CallVoidMethod(g_Obj, g_TrackRevenue, cents, eventToken_jstr, dict_jglobal);
    // free global ref 
    env->DeleteGlobalRef(dict_jglobal);

    return (s3eResult)0;
}

s3eResult SetEnabled_platform(bool enabled)
{
    // get JNI env
    JNIEnv* env = s3eEdkJNIGetEnv();
    // call java set enable
    env->CallVoidMethod(g_Obj, g_setEnabled, enabled);
    
    return (s3eResult)0;
}

s3eResult IsEnabled_platform(bool& isEnabled_out)
{
    // get JNI env
    JNIEnv* env = s3eEdkJNIGetEnv();
    // call java is enabled with boolean return
    jboolean isEnabled_java = env->CallBooleanMethod(g_Obj, g_isEnabled);
    // cast jboolean to cbool and store it in output parameter
    isEnabled_out = (bool) isEnabled_java;

    return (s3eResult)0;
}

s3eResult SetResponseDelegate_platform(response_data_delegate delegateFn)
{
    JNIEnv* env = s3eEdkJNIGetEnv();

    EDK_CALLBACK_REG(ADJUST, ADJUST_RESPONSE_DATA, (s3eCallback)delegateFn, NULL, false);

    env->CallVoidMethod(g_Obj, g_SetResponseDelegate);
    //adjust_delegate = delegateFn;
    //adjust_delegate = trace_response_data_internal;
    IwTrace(ADJUSTMARMALADE,("adjust_delegate set"));

    return (s3eResult)0;
}

jobject create_global_java_dict(const param_type *params)
{
    if (params == NULL) {
        return NULL;
    }

    // get JNI env
    JNIEnv* env = s3eEdkJNIGetEnv();

    // Get the HashMap class
    jclass dict_cls = s3eEdkAndroidFindClass("java/util/HashMap");

    // Get its initial size constructor
    jmethodID dict_cons = env->GetMethodID(dict_cls, "<init>", "(I)V");

    // Construct the java class with the default size
    jobject dict_obj = env->NewObject(dict_cls, dict_cons, params->size());

    // Get the put method
    jmethodID put_method = env->GetMethodID(dict_cls, "put", 
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    for (param_type::const_iterator pos = params->begin();pos != params->end(); ++pos) {
        jstring key = env->NewStringUTF(pos->first);
        jstring value = env->NewStringUTF(pos->second);
        IwTrace(ADJUSTMARMALADE,("CallObjectMethod(dict_obj, put_method, key, value"));

        env->CallObjectMethod(dict_obj, put_method, key, value);
    }
    
    jobject dict_jglobal = env->NewGlobalRef(dict_obj);
    return dict_jglobal;
}

void responseData_callback(JNIEnv* env, jobject obj, jstring responseDataString) 
{
    //if (adjust_delegate == NULL) {
    //    return;
    //}
    IwTrace(ADJUSTMARMALADE,("responseData_callback"));
    const char* responseDataCString = env->GetStringUTFChars(responseDataString, NULL);

    IwTrace(ADJUSTMARMALADE,("GetStringUTFChars(responseDataString, NULL)"));
    IwTrace(ADJUSTMARMALADE,(responseDataCString));

    response_data* rd = get_response_data(responseDataCString);
    IwTrace(ADJUSTMARMALADE,("before adjust_delegate"));

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                            S3E_ADJUST_CALLBACK_ADJUST_RESPONSE_DATA,
                            rd,
                            sizeof(*rd));

    //adjust_delegate(rd);
    //trace_response_data(rd);
    IwTrace(ADJUSTMARMALADE,("after adjust_delegate"));
    delete rd;
}

response_data* get_response_data(const char* jsonCString)
{
    response_data* rd;
    rapidjson::Document jsonDoc;

    if(jsonDoc.Parse<0>(jsonCString).HasParseError()) {
        IwTrace(ADJUSTMARMALADE,("error parsing response data json"));
        return NULL;
    }
    IwTrace(ADJUSTMARMALADE,("parsing response data json"));

    rd = new response_data();
    
    rd->activityKind    = get_json_member(jsonDoc, "activityKind");
    rd->error           = get_json_member(jsonDoc, "error");
    rd->success         = get_json_member(jsonDoc, "success").compare("true") == 0;
    rd->willRetry       = get_json_member(jsonDoc, "willRetry").compare("true") == 0;
    rd->trackerToken    = get_json_member(jsonDoc, "trackerToken");
    rd->trackerName     = get_json_member(jsonDoc, "trackerName");
    rd->network         = get_json_member(jsonDoc, "network");
    rd->campaign        = get_json_member(jsonDoc, "campaign");
    rd->adgroup         = get_json_member(jsonDoc, "adgroup");
    rd->creative        = get_json_member(jsonDoc, "creative");
    return rd;
}

std::string get_json_member(rapidjson::Document &jsonDoc, const char* member_name)
{
    if (jsonDoc.HasMember(member_name)) {
        return std::string(jsonDoc[member_name].GetString());
    } else {
        return std::string();
    }
}