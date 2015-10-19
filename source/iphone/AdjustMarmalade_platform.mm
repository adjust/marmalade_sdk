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

- (id) init {
    self = [super init];
    return self;
}

- (char *)convertString:(NSString *)string {
    if (string == nil) {
        return NULL;
    }

    const char* source = [string UTF8String];
    char *target = adjust_CopyString(source);
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

- (void)adjustAttributionChanged:(ADJAttribution *)attribution {
    adjust_attribution_data* attribution_data = [self getAttributionData:attribution];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                           S3E_ADJUST_CALLBACK_ADJUST_ATTRIBUTION_DATA,
                           attribution_data,
                           sizeof(*attribution_data),
                           NULL,
                           S3E_FALSE,
                           &adjust_CleanupAttributionCallback,
                           (void*)attribution_data);
}

@end

BOOL is_string_valid(NSString *string) {
    if (string != nil && ![string isEqualToString:@""]) {
        return YES;
    } else {
        return NO;
    }
}

int32 handleOpenURL(void* systemData, void* userData)
{
    NSURL *url = (NSURL *)systemData;

    [Adjust appWillOpenUrl:url];

    return 0;
}

s3eResult AdjustMarmaladeInit_platform()
{
    // Subscribe for deep linking.
    s3eEdkCallbacksRegisterInternal(S3E_EDK_INTERNAL,
                                    S3E_EDK_CALLBACK_MAX,
                                    S3E_EDK_IPHONE_HANDLEOPENURL,
                                    (s3eCallback)handleOpenURL,
                                    NULL,
                                    S3E_FALSE);

    return S3E_RESULT_SUCCESS;
}

void AdjustMarmaladeTerminate_platform()
{
    s3eEdkCallbacksUnRegister(S3E_EDK_INTERNAL,
                              S3E_EDK_CALLBACK_MAX,
                              S3E_EDK_IPHONE_HANDLEOPENURL,
                              (s3eCallback)handleOpenURL);
}

s3eResult adjust_Start_platform(adjust_config* config)
{
    NSString *appToken = config->app_token ? [NSString stringWithUTF8String:config->app_token] : nil;
    NSString *environment = config->environment ? [NSString stringWithUTF8String:config->environment] : nil;
    NSString *logLevel = config->log_level ? [NSString stringWithUTF8String:config->log_level] : nil;
    NSString *sdkPrefix = config->sdk_prefix ? [NSString stringWithUTF8String:config->sdk_prefix] : nil;
    NSString *processName = config->process_name ? [NSString stringWithUTF8String:config->process_name] : nil;
    NSString *defaultTracker = config->default_tracker ? [NSString stringWithUTF8String:config->default_tracker] : nil;

    // If no app token nor environment is set, skip initialisation.
    if (is_string_valid(appToken) && is_string_valid(environment)) {
        ADJConfig *adjustConfig = [ADJConfig configWithAppToken:appToken environment:environment];

        if (is_string_valid(logLevel)) {
            [adjustConfig setLogLevel:[ADJLogger LogLevelFromString:logLevel]];
        }

        if (is_string_valid(defaultTracker)) {
            [adjustConfig setDefaultTracker:defaultTracker];
        }

        if (is_string_valid(sdkPrefix)) {
            [adjustConfig setSdkPrefix:sdkPrefix];
        }

        if (adjustMarmaladeInstance != nil) {
            [adjustConfig setDelegate:(id)adjustMarmaladeInstance];
        }

        if (config->is_event_buffering_enabled != NULL) {
            [adjustConfig setEventBufferingEnabled:*(config->is_event_buffering_enabled)];
        }

        if (config->is_mac_md5_tracking_enabled != NULL) {
            [adjustConfig setMacMd5TrackingEnabled:*(config->is_mac_md5_tracking_enabled)];
        }

        if (config->is_attribution_delegate_set != NULL) {
            if (*(config->is_attribution_delegate_set) == true) {
                EDK_CALLBACK_REG(ADJUST, ADJUST_ATTRIBUTION_DATA,
                                 (s3eCallback)config->attribution_callback, NULL, false);

                adjustMarmaladeInstance = [[AdjustMarmalade_platform alloc] init];

                [adjustConfig setDelegate:(id)adjustMarmaladeInstance];
            }
        }

        [Adjust appDidLaunch:adjustConfig];
    }

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_TrackEvent_platform(adjust_event* event)
{
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

s3eResult adjust_SetEnabled_platform(bool is_enabled)
{
    [Adjust setEnabled:is_enabled];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_IsEnabled_platform(bool& is_enabled_out)
{
    BOOL isEnabled = [Adjust isEnabled];

    is_enabled_out = isEnabled;

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetOfflineMode_platform(bool is_offline_mode_enabled)
{
    [Adjust setOfflineMode:is_offline_mode_enabled];

    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_OnPause_platform()
{
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_OnResume_platform()
{
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetReferrer_platform(const char* referrer)
{
    return S3E_RESULT_SUCCESS;
}

s3eResult adjust_SetDeviceToken_platform(const char* device_token)
{
    NSData *deviceToken = [[NSString stringWithUTF8String:device_token] dataUsingEncoding:NSUTF8StringEncoding];

    [Adjust setDeviceToken:deviceToken];

    return S3E_RESULT_SUCCESS;
}
