/*
 Generic implementation of the AdjustMarmalade extension.
 This file should perform any platform-indepedentent functionality
 (e.g. error checking) before calling platform-dependent implementations.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

#include "AdjustMarmalade_internal.h"

s3eResult AdjustMarmaladeInit() {
    // Add any generic initialisation code here.
    return AdjustMarmaladeInit_platform();
}

void AdjustMarmaladeTerminate() {
    // Add any generic termination code here.
    AdjustMarmaladeTerminate_platform();
}

void adjust_CleanupAttributionCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {
    adjust_attribution_data* attribution = (adjust_attribution_data*)completeData;

    if (attribution == NULL) {
        return;
    }

    free(attribution->tracker_token);
    free(attribution->tracker_name);
    free(attribution->network);
    free(attribution->campaign);
    free(attribution->ad_group);
    free(attribution->creative);
    free(attribution->click_label);
    free(attribution->adid);

    delete attribution;
}

void adjust_CleanupEventSuccessCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {
    adjust_event_success_data* event_success = (adjust_event_success_data*)completeData;

    if (event_success == NULL) {
        return;
    }

    free(event_success->message);
    free(event_success->timestamp);
    free(event_success->event_token);
    free(event_success->adid);
    free(event_success->json_response);

    delete event_success;
}

void adjust_CleanupEventFailureCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {
    adjust_event_failure_data* event_failure = (adjust_event_failure_data*)completeData;

    if (event_failure == NULL) {
        return;
    }

    free(event_failure->message);
    free(event_failure->timestamp);
    free(event_failure->event_token);
    free(event_failure->adid);
    free(event_failure->will_retry);
    free(event_failure->json_response);

    delete event_failure;
}

void adjust_CleanupSessionSuccessCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {
    adjust_session_success_data* session_success = (adjust_session_success_data*)completeData;

    if (session_success == NULL) {
        return;
    }

    free(session_success->message);
    free(session_success->timestamp);
    free(session_success->adid);
    free(session_success->json_response);

    delete session_success;
}

void adjust_CleanupSessionFailureCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {
    adjust_session_failure_data* session_failure = (adjust_session_failure_data*)completeData;

    if (session_failure == NULL) {
        return;
    }

    free(session_failure->message);
    free(session_failure->timestamp);
    free(session_failure->adid);
    free(session_failure->will_retry);
    free(session_failure->json_response);

    delete session_failure;
}

char* adjust_CopyString(const char* source) {
    if (source == NULL) {
        return NULL;
    }

    char* target = (char*)malloc(sizeof(char) * (strlen(source) + 1));

    if (target == NULL) {
        return NULL;
    }

    strcpy(target, source);

    return target;
}

s3eResult adjust_Start(adjust_config* config) {
    config->set_sdk_prefix("marmalade4.13.0");
    return adjust_Start_platform(config);
}

s3eResult adjust_TrackEvent(adjust_event* event) {
    return adjust_TrackEvent_platform(event);
}

s3eResult adjust_SetEnabled(bool is_enabled) {
    return adjust_SetEnabled_platform(is_enabled);
}

s3eResult adjust_IsEnabled(bool& is_enabled_out) {
    return adjust_IsEnabled_platform(is_enabled_out);
}

s3eResult adjust_SetOfflineMode(bool is_offline_mode_enabled) {
    return adjust_SetOfflineMode_platform(is_offline_mode_enabled);
}

s3eResult adjust_SetReferrer(const char* referrer) {
    return adjust_SetReferrer_platform(referrer);
}

s3eResult adjust_SetDeviceToken(const char* device_token) {
    return adjust_SetDeviceToken_platform(device_token);
}

s3eResult adjust_SendFirstPackages() {
    return adjust_SendFirstPackages_platform();
}

s3eResult adjust_AddSessionCallbackParameter(const char* key, const char* value) {
    return adjust_AddSessionCallbackParameter_platform(key, value);
}

s3eResult adjust_AddSessionPartnerParameter(const char* key, const char* value) {
    return adjust_AddSessionPartnerParameter_platform(key, value);
}

s3eResult adjust_RemoveSessionCallbackParameter(const char* key) {
    return adjust_RemoveSessionCallbackParameter_platform(key);
}

s3eResult adjust_RemoveSessionPartnerParameter(const char* key) {
    return adjust_RemoveSessionPartnerParameter_platform(key);
}

s3eResult adjust_ResetSessionCallbackParameters() {
    return adjust_ResetSessionCallbackParameters_platform();
}

s3eResult adjust_ResetSessionPartnerParameters() {
    return adjust_ResetSessionPartnerParameters_platform();
}

s3eResult adjust_GdprForgetMe() {
    return adjust_GdprForgetMe_platform();
}

s3eResult adjust_GetGoogleAdId() {
    return adjust_GetGoogleAdId_platform();
}

s3eResult adjust_GetIdfa() {
    return adjust_GetIdfa_platform();
}

s3eResult adjust_GetAdid(char** adid) {
    return adjust_GetAdid_platform(adid);
}

s3eResult adjust_GetAttribution(adjust_attribution_data* attribution) {
    return adjust_GetAttribution_platform(attribution);
}
