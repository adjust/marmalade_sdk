/*
 * iphone-specific implementation of the AdjustMarmalade extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

#include "s3eEdk.h"
#include "s3eEdk_iphone.h"

#include "AdjustMarmalade_platform.h"
#include "AdjustMarmalade_internal.h"

@implementation AdjustMarmalade_platform

static id<AdjustDelegate> adjustMarmaladeInstance = nil;
static BOOL isAttributionCallbackSet = NO;
static BOOL isSessionSuccessCallbackSet = NO;
static BOOL isSessionFailureCallbackSet = NO;
static BOOL isEventSuccessCallbackSet = NO;
static BOOL isEventFailureCallbackSet = NO;
static BOOL isDeeplinkCallbackSet = NO;
static BOOL isDeferredDeplinkCallbackSet = NO;
static BOOL shouldDeferredDeeplinkBeOpened = YES;
static BOOL isGoogleAdIdCallbackSet = NO;
static BOOL isIdfaCallbackSet = NO;

- (id)init {
    self = [super init];
    return self;
}

- (char*)convertString:(NSString *)string {
    if (string == nil) {
        return NULL;
    }

    const char* source = [string UTF8String];
    char* target = adjust_CopyString(source);
    return target;
}

- (adjust_attribution_data*)getAttributionData:(ADJAttribution *)adjustAttributionData {
    adjust_attribution_data* attribution = new adjust_attribution_data();

    attribution->tracker_token   = [self convertString:adjustAttributionData.trackerToken];
    attribution->tracker_name    = [self convertString:adjustAttributionData.trackerName];
    attribution->network         = [self convertString:adjustAttributionData.network];
    attribution->campaign        = [self convertString:adjustAttributionData.campaign];
    attribution->ad_group        = [self convertString:adjustAttributionData.adgroup];
    attribution->creative        = [self convertString:adjustAttributionData.creative];
    attribution->click_label     = [self convertString:adjustAttributionData.clickLabel];

    return attribution;
}

- (adjust_session_success_data*)getSessionSuccessData:(ADJSessionSuccess *)sessionSuccessData {
    adjust_session_success_data* sessionSuccess = new adjust_session_success_data();

    sessionSuccess->message       = [self convertString:sessionSuccessData.message];
    sessionSuccess->timestamp     = [self convertString:sessionSuccessData.timeStamp];
    sessionSuccess->adid          = [self convertString:sessionSuccessData.adid];

    if (sessionSuccessData.jsonResponse != nil) {
        NSError *error;
        NSString *jsonString = nil;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:sessionSuccessData.jsonResponse
                                                           options:0
                                                             error:&error];
        if (jsonData == nil) {
            sessionSuccess->json_response = [self convertString:@""];
        } else {
            jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            sessionSuccess->json_response = [self convertString:jsonString];
        }
    } else {
        sessionSuccess->json_response = [self convertString:@""];
    }

    return sessionSuccess;
}

- (adjust_session_failure_data*)getSessionFailureData:(ADJSessionFailure *)sessionFailureData {
    adjust_session_failure_data* sessionFailure = new adjust_session_failure_data();

    sessionFailure->message       = [self convertString:sessionFailureData.message];
    sessionFailure->timestamp     = [self convertString:sessionFailureData.timeStamp];
    sessionFailure->adid          = [self convertString:sessionFailureData.adid];

    NSString *booleanString = (sessionFailureData.willRetry) ? @"Yes" : @"No";
    sessionFailure->will_retry    = [self convertString:booleanString];

    if (sessionFailureData.jsonResponse != nil) {
        NSError *error;
        NSString *jsonString = nil;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:sessionFailureData.jsonResponse
                                                           options:0
                                                             error:&error];

        if (jsonData == nil) {
            sessionFailure->json_response = [self convertString:@""];
        } else {
            jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            sessionFailure->json_response = [self convertString:jsonString];
        }
    } else {
        sessionFailure->json_response = [self convertString:@""];
    }

    return sessionFailure;
}

- (adjust_event_success_data*)getEventSuccessData:(ADJEventSuccess *)eventSuccessData {
    adjust_event_success_data* eventSuccess = new adjust_event_success_data();

    eventSuccess->message       = [self convertString:eventSuccessData.message];
    eventSuccess->timestamp     = [self convertString:eventSuccessData.timeStamp];
    eventSuccess->event_token   = [self convertString:eventSuccessData.eventToken];
    eventSuccess->adid          = [self convertString:eventSuccessData.adid];

    if (eventSuccessData.jsonResponse != nil) {
        NSError *error;
        NSString *jsonString = nil;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:eventSuccessData.jsonResponse
                                                           options:0
                                                             error:&error];

        if (jsonData == nil) {
            eventSuccess->json_response = [self convertString:@""];
        } else {
            jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            eventSuccess->json_response = [self convertString:jsonString];
        }
    } else {
        eventSuccess->json_response = [self convertString:@""];
    }

    return eventSuccess;
}

- (adjust_event_failure_data*)getEventFailureData:(ADJEventFailure *)eventFailureData {
    adjust_event_failure_data* eventFailure = new adjust_event_failure_data();

    eventFailure->message       = [self convertString:eventFailureData.message];
    eventFailure->timestamp     = [self convertString:eventFailureData.timeStamp];
    eventFailure->event_token   = [self convertString:eventFailureData.eventToken];
    eventFailure->adid          = [self convertString:eventFailureData.adid];

    NSString *booleanString = (eventFailureData.willRetry) ? @"Yes" : @"No";
    eventFailure->will_retry    = [self convertString:booleanString];

    if (eventFailureData.jsonResponse != nil) {
        NSError *error;
        NSString *jsonString = nil;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:eventFailureData.jsonResponse
                                                           options:0
                                                             error:&error];

        if (jsonData == nil) {
            eventFailure->json_response = [self convertString:@""];
        } else {
            jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            eventFailure->json_response = [self convertString:jsonString];
        }
    } else {
        eventFailure->json_response = [self convertString:@""];
    }

    return eventFailure;
}

- (void)adjustAttributionChanged:(ADJAttribution *)attribution {
    if (isAttributionCallbackSet == NO) {
        return;
    }

    adjust_attribution_data* attributionData = [self getAttributionData:attribution];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_ATTRIBUTION_DATA,
                           attributionData,
                           sizeof(*attributionData),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupAttributionCallback,
                           (void*)attributionData);
}

- (void)adjustSessionTrackingSucceeded:(ADJSessionSuccess *)sessionSuccessResponseData {
    if (isSessionSuccessCallbackSet == NO) {
        return;
    }

    adjust_session_success_data* sessionSuccessData = [self getSessionSuccessData:sessionSuccessResponseData];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_SESSION_SUCCESS_DATA,
                           sessionSuccessData,
                           sizeof(*sessionSuccessData),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupSessionSuccessCallback,
                           (void*)sessionSuccessData);
}

- (void)adjustSessionTrackingFailed:(ADJSessionFailure *)sessionFailureResponseData {
    if (isSessionFailureCallbackSet == NO) {
        return;
    }

    adjust_session_failure_data* sessionFailureData = [self getSessionFailureData:sessionFailureResponseData];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_SESSION_FAILURE_DATA,
                           sessionFailureData,
                           sizeof(*sessionFailureData),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupSessionFailureCallback,
                           (void*)sessionFailureData);
}

- (void)adjustEventTrackingSucceeded:(ADJEventSuccess *)eventSuccessResponseData {
    if (isEventSuccessCallbackSet == NO) {
        return;
    }

    adjust_event_success_data* eventSuccessData = [self getEventSuccessData:eventSuccessResponseData];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_EVENT_SUCCESS_DATA,
                           eventSuccessData,
                           sizeof(*eventSuccessData),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupEventSuccessCallback,
                           (void*)eventSuccessData);
}

- (void)adjustEventTrackingFailed:(ADJEventFailure *)eventFailureResponseData {
    if (isEventFailureCallbackSet == NO) {
        return;
    }

    adjust_event_failure_data* eventFailureData = [self getEventFailureData:eventFailureResponseData];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_EVENT_FAILURE_DATA,
                           eventFailureData,
                           sizeof(*eventFailureData),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupEventFailureCallback,
                           (void*)eventFailureData);
}

- (BOOL)adjustDeeplinkResponse:(NSURL *)deeplink {
    if (isDeferredDeplinkCallbackSet == NO) {
        return shouldDeferredDeeplinkBeOpened;
    }

    NSString *stringDeeplink = [deeplink absoluteString];
    const char* csDeeplink = stringDeeplink != nil ? [stringDeeplink UTF8String] : NULL;

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_DEFERRED_DEEPLINK_DATA,
                           (void*)csDeeplink,
                           (csDeeplink != NULL ? (int)strlen(csDeeplink) + 1 : 0));

    return shouldDeferredDeeplinkBeOpened;
}

@end

BOOL is_string_valid(NSString *string) {
    if (string != nil && ![string isEqualToString:@""]) {
        return YES;
    } else {
        return NO;
    }
}

void assign_delegate_if_not_assigned(ADJConfig *adjustConfig) {
    if (adjustMarmaladeInstance != nil) {
        return;
    }

    adjustMarmaladeInstance = [[AdjustMarmalade_platform alloc] init];
    [adjustConfig setDelegate:(id)adjustMarmaladeInstance];
}

int32 handle_open_url(void* systemData, void* userData) {
    NSURL *url = (NSURL *)systemData;
    [Adjust appWillOpenUrl:url];

    if (isDeeplinkCallbackSet == NO) {
        return 0;
    }

    NSString *stringDeeplink = [url absoluteString];
    const char* csDeeplink = stringDeeplink != nil ? [stringDeeplink UTF8String] : NULL;

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_DEEPLINK_DATA,
                           (void*)csDeeplink,
                           (csDeeplink != NULL ? (int)strlen(csDeeplink) + 1 : 0));

    return 0;
}

// To be added once universal links support is added to the Marmalade.
/*
int32 handle_open_universal_url(void* systemData, void* userData)
{
    if (isDeeplinkCallbackSet == NO)
    {
        return 0;
    }

    NSString *stringDeeplink = @"deeplink";
    const char* csDeeplink = stringDeeplink != nil ? [stringDeeplink UTF8String] : NULL;

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_DEEPLINK_DATA,
                           (void*)csDeeplink,
                           (csDeeplink != NULL ? (int)strlen(csDeeplink) + 1 : 0));

    return 0;
}
*/

s3eResult AdjustMarmaladeInit_platform() {
    // Subscribe for deep linking.
    s3eEdkCallbacksRegisterInternal(S3E_EDK_INTERNAL,
                                    S3E_EDK_CALLBACK_MAX,
                                    S3E_EDK_IPHONE_HANDLEOPENURL,
                                    (s3eCallback)handle_open_url,
                                    NULL,
                                    S3E_FALSE);

    // To be added once universal links support is added to the Marmalade.
    /*
    // Subscribe for universal deep linking.
    s3eEdkCallbacksRegisterInternal(S3E_EDK_INTERNAL,
                                    S3E_EDK_CALLBACK_MAX,
                                    S3E_EDK_IPHONE_HANDLEOPENURL_EXTRA,
                                    (s3eCallback)handle_open_universal_url,
                                    NULL,
                                    S3E_FALSE);
    */

    return S3E_RESULT_SUCCESS;
}

void AdjustMarmaladeTerminate_platform() {
    s3eEdkCallbacksUnRegister(S3E_EDK_INTERNAL,
                              S3E_EDK_CALLBACK_MAX,
                              S3E_EDK_IPHONE_HANDLEOPENURL,
                              (s3eCallback)handle_open_url);

    // To be added once universal links support is added to the Marmalade.
    /*
    s3eEdkCallbacksUnRegister(S3E_EDK_INTERNAL,
                              S3E_EDK_CALLBACK_MAX,
                              S3E_EDK_IPHONE_HANDLEOPENURL_EXTRA,
                              (s3eCallback)handle_open_universal_url);
    */
}

s3eResult adjust_Start_platform(adjust_config* config) {
    NSString *appToken = config->app_token ? [NSString stringWithUTF8String:config->app_token] : nil;
    NSString *environment = config->environment ? [NSString stringWithUTF8String:config->environment] : nil;
    NSString *logLevel = config->log_level ? [NSString stringWithUTF8String:config->log_level] : nil;
    NSString *sdkPrefix = config->sdk_prefix ? [NSString stringWithUTF8String:config->sdk_prefix] : nil;
    NSString *processName = config->process_name ? [NSString stringWithUTF8String:config->process_name] : nil;
    NSString *defaultTracker = config->default_tracker ? [NSString stringWithUTF8String:config->default_tracker] : nil;
    NSString *userAgent = config->user_agent ? [NSString stringWithUTF8String:config->user_agent] : nil;

    // If no app token nor environment is set, skip initialisation.
    if (is_string_valid(appToken) && is_string_valid(environment)) {
        BOOL isSuppress = NO;

        if (is_string_valid(logLevel)) {
            if ([logLevel isEqualToString:@"suppress"]) {
                isSuppress = YES;
            }
        }

        ADJConfig *adjustConfig = [ADJConfig configWithAppToken:appToken environment:environment allowSuppressLogLevel:isSuppress];

        if (is_string_valid(logLevel)) {
            if (isSuppress) {
                [adjustConfig setLogLevel:ADJLogLevelSuppress];
            } else {
              [adjustConfig setLogLevel:[ADJLogger LogLevelFromString:logLevel]];
            }
        }

        if (is_string_valid(defaultTracker)) {
            [adjustConfig setDefaultTracker:defaultTracker];
        }

        if (is_string_valid(sdkPrefix)) {
            [adjustConfig setSdkPrefix:sdkPrefix];
        }

        if (is_string_valid(userAgent)) {
            [adjustConfig setUserAgent:userAgent];
        }

        if (config->delay_start != NULL) {
            [adjustConfig setDelayStart:(*(config->delay_start))];
        }

        if (config->is_event_buffering_enabled != NULL) {
            [adjustConfig setEventBufferingEnabled:*(config->is_event_buffering_enabled)];
        }

        if (config->is_sending_in_background_enabled != NULL) {
            [adjustConfig setSendInBackground:*(config->is_sending_in_background_enabled)];
        }

        if (config->is_attribution_delegate_set != NULL) {
            if (*(config->is_attribution_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_ATTRIBUTION_DATA, (s3eCallback)config->attribution_callback, NULL, false);

                assign_delegate_if_not_assigned(adjustConfig);
                isAttributionCallbackSet = YES;
            }
        }

        if (config->is_session_success_delegate_set != NULL) {
            if (*(config->is_session_success_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_SESSION_SUCCESS_DATA, (s3eCallback)config->session_success_callback, NULL, false);

                assign_delegate_if_not_assigned(adjustConfig);
                isSessionSuccessCallbackSet = YES;
            }
        }

        if (config->is_session_failure_delegate_set != NULL) {
            if (*(config->is_session_failure_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_SESSION_FAILURE_DATA, (s3eCallback)config->session_failure_callback, NULL, false);

                assign_delegate_if_not_assigned(adjustConfig);
                isSessionFailureCallbackSet = YES;
            }
        }

        if (config->is_event_success_delegate_set != NULL) {
            if (*(config->is_event_success_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_EVENT_SUCCESS_DATA, (s3eCallback)config->event_success_callback, NULL, false);

                assign_delegate_if_not_assigned(adjustConfig);
                isEventSuccessCallbackSet = YES;
            }
        }

        if (config->is_event_failure_delegate_set != NULL) {
            if (*(config->is_event_failure_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_EVENT_FAILURE_DATA, (s3eCallback)config->event_failure_callback, NULL, false);

                assign_delegate_if_not_assigned(adjustConfig);
                isEventFailureCallbackSet = YES;
            }
        }

        if (config->is_deeplink_delegate_set != NULL) {
            if (*(config->is_deeplink_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_DEEPLINK_DATA, (s3eCallback)config->deeplink_callback, NULL, false);
                isDeeplinkCallbackSet = YES;
            }
        }

        if (config->is_deferred_deeplink_delegate_set != NULL) {
            if (*(config->is_deferred_deeplink_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_DEFERRED_DEEPLINK_DATA, (s3eCallback)config->deferred_deeplink_callback, NULL, false);
                isDeferredDeplinkCallbackSet = YES;
            }
        }

        if (config->is_google_ad_id_delegate_set != NULL) {
            if (*(config->is_google_ad_id_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_GOOGLE_AD_ID_DATA, (s3eCallback)config->google_ad_id_callback, NULL, false);
                isGoogleAdIdCallbackSet = YES;
            }
        }

        if (config->is_idfa_delegate_set != NULL) {
            if (*(config->is_idfa_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_IDFA_DATA, (s3eCallback)config->idfa_callback, NULL, false);
                isIdfaCallbackSet = YES;
            }
        }

        [Adjust appDidLaunch:adjustConfig];
        [Adjust trackSubsessionStart];
    }

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_TrackEvent_platform(adjust_event* event) {
    NSString *eventToken = event->event_token ? [NSString stringWithUTF8String:event->event_token] : nil;
    NSString *currency = event->currency ? [NSString stringWithUTF8String:event->currency] : nil;
    NSString *transactionId = event->transaction_id ? [NSString stringWithUTF8String:event->transaction_id] : nil;
    NSData *receipt = event->receipt ? [[NSString stringWithUTF8String:event->receipt] dataUsingEncoding:NSUTF8StringEncoding] : nil;

    if (is_string_valid(eventToken)) {
        ADJEvent *adjustEvent = [ADJEvent eventWithEventToken:eventToken];

        if (event->revenue != NULL) {
            [adjustEvent setRevenue:*(event->revenue) currency:currency];
        }

        if (event->callback_params->size() > 0) {
            for (int i = 0; i < event->callback_params->size(); i++) {
                std::pair<const char*, const char*> param = event->callback_params->at(i);

                NSString *key = [NSString stringWithUTF8String:param.first];
                NSString *value = [NSString stringWithUTF8String:param.second];

                [adjustEvent addCallbackParameter:key value:value];
            }
        }

        if (event->partner_params->size() > 0) {
            for (int i = 0; i < event->partner_params->size(); i++) {
                std::pair<const char*, const char*> param = event->partner_params->at(i);

                NSString *key = [NSString stringWithUTF8String:param.first];
                NSString *value = [NSString stringWithUTF8String:param.second];

                [adjustEvent addPartnerParameter:key value:value];
            }
        }

        if (event->is_receipt_set != NULL) {
            [adjustEvent setReceipt:receipt transactionId:transactionId];
        } else {
            if (is_string_valid(transactionId)) {
                [adjustEvent setTransactionId:transactionId];
            }
        }

        [Adjust trackEvent:adjustEvent];
    }

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetEnabled_platform(bool is_enabled) {
    [Adjust setEnabled:is_enabled];
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_IsEnabled_platform(bool& is_enabled_out) {
    BOOL isEnabled = [Adjust isEnabled];
    is_enabled_out = isEnabled;

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetOfflineMode_platform(bool is_offline_mode_enabled) {
    [Adjust setOfflineMode:is_offline_mode_enabled];
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetReferrer_platform(const char* referrer) {
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetDeviceToken_platform(const char* device_token) {
    NSData *deviceToken = [[NSString stringWithUTF8String:device_token] dataUsingEncoding:NSUTF8StringEncoding];
    [Adjust setDeviceToken:deviceToken];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SendFirstPackages_platform() {
    [Adjust sendFirstPackages];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_AddSessionCallbackParameter_platform(const char* key, const char* value) {
    NSString *sKey = key ? [NSString stringWithUTF8String:key] : nil;
    NSString *sValue = value ? [NSString stringWithUTF8String:value] : nil;

    [Adjust addSessionCallbackParameter:sKey value:sValue];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_AddSessionPartnerParameter_platform(const char* key, const char* value) {
    NSString *sKey = key ? [NSString stringWithUTF8String:key] : nil;
    NSString *sValue = value ? [NSString stringWithUTF8String:value] : nil;

    [Adjust addSessionPartnerParameter:sKey value:sValue];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_RemoveSessionCallbackParameter_platform(const char* key) {
    NSString *sKey = key ? [NSString stringWithUTF8String:key] : nil;

    [Adjust removeSessionCallbackParameter:sKey];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_RemoveSessionPartnerParameter_platform(const char* key) {
    NSString *sKey = key ? [NSString stringWithUTF8String:key] : nil;

    [Adjust removeSessionPartnerParameter:sKey];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_ResetSessionCallbackParameters_platform() {
    [Adjust resetSessionCallbackParameters];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_ResetSessionPartnerParameters_platform() {
    [Adjust resetSessionPartnerParameters];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_GetGoogleAdId_platform() {
    if (isGoogleAdIdCallbackSet) {
        NSString *googleAdId = @"";
        const char* csGoogleAdId = googleAdId != nil ? [googleAdId UTF8String] : NULL;

        s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                               S3E_ADJUST_CALLBACK_ADJUST_GOOGLE_AD_ID_DATA,
                               (void*)csGoogleAdId,
                               (csGoogleAdId != NULL ? (int)strlen(csGoogleAdId) + 1 : 0));
    }
    
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_GetIdfa_platform() {
    if (isIdfaCallbackSet) {
        NSString *idfa = [Adjust idfa];
        const char* csIdfa = idfa != nil ? [idfa UTF8String] : NULL;

        s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                               S3E_ADJUST_CALLBACK_ADJUST_IDFA_DATA,
                               (void*)csIdfa,
                               (csIdfa != NULL ? (int)strlen(csIdfa) + 1 : 0));
    }
    
    return S3E_RESULT_SUCCESS;
}
