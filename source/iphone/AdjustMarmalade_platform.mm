/*
 * iphone-specific implementation of the AdjustMarmalade extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "AdjustMarmalade_internal.h"

#import "AdjustMarmalade_platform.h"
#import "NSString+AIAdditions.h"
#import "AIAdjustFactory.h"
#include "s3eEdk.h"

#include <stdio.h>
#include <string.h>

#define S3E_DEVICE_ADJUST                   S3E_EXT_ADJUSTMARMALADE_HASH

typedef enum s3eAdjustCallback
{
    S3E_ADJUST_CALLBACK_ADJUST_RESPONSE_DATA,
    S3E_ADJUST_CALLBACK_MAX
} s3eAdjustCallback;

static void response_data_cb_cleanup(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {

	response_data * rd = (response_data *) completeData;
	if (rd == NULL) {
		return;
	}

	free(rd->activityKind);
	free(rd->error);
	free(rd->trackerToken);
	free(rd->trackerName);
	free(rd->network);
	free(rd->campaign);
	free(rd->adgroup);
	free(rd->creative);

	delete rd;
}


@implementation AdjustMarmalade_platform

static id<AdjustDelegate> adjustMarmaladeInstance = nil;

- (id) init {
    self = [super init];
    return self;
}

- (char *) convertString:(NSString *)string {
	
	if (string == nil) {
		return NULL;
	}

	char * target;
	const char * source = [string UTF8String];
	target = (char *) malloc(sizeof(char) * strlen(source));
	strcpy(target, source);
	return target;
}

- (response_data*) getResponseData:(AIResponseData *) adjustResponseData {
	response_data* rd = new response_data();

    rd->success         = (bool) adjustResponseData.success;
    rd->willRetry       = (bool) adjustResponseData.willRetry;
	rd->activityKind    = [self convertString:adjustResponseData.activityKindString];
    rd->error           = [self convertString:adjustResponseData.error ];
    rd->trackerToken    = [self convertString:adjustResponseData.trackerToken];
    rd->trackerName     = [self convertString:adjustResponseData.trackerName];
    rd->network         = [self convertString:adjustResponseData.network];
    rd->campaign        = [self convertString:adjustResponseData.campaign];
    rd->adgroup         = [self convertString:adjustResponseData.adgroup];
    rd->creative        = [self convertString:adjustResponseData.creative];

    return rd;
}

- (void)adjustFinishedTrackingWithResponse:(AIResponseData *)responseData {

	response_data* rd = [self getResponseData:responseData];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                        S3E_ADJUST_CALLBACK_ADJUST_RESPONSE_DATA,
                        rd,
                        sizeof(*rd),
                        NULL,
                        S3E_FALSE,
                        &response_data_cb_cleanup,
                        (void*)rd);
}

@end

s3eResult AdjustMarmaladeInit_platform()
{
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void AdjustMarmaladeTerminate_platform()
{
    // Add any platform-specific termination code here
}

s3eResult AppDidLaunch_platform(const char* appToken, const char* environment, const char* sdkPrefix, const char* logLevel, bool eventBuffering)
{
	NSString* sAppToken = [NSString stringWithUTF8String: appToken];
	NSString* sEnvironment = [NSString stringWithUTF8String: environment];
	NSString* sSdkPrefix = [NSString stringWithUTF8String: sdkPrefix];
	//AILogLevel eLogLevel = (AILogLevel)logLevel;
	// TODO parse log level
	AILogLevel eLogLevel = (AILogLevel)AILogLevelVerbose;
	BOOL bEventBuffering = (BOOL) eventBuffering;

	NSLog(@"%@, %@, %d, %d", sAppToken, sEnvironment, eLogLevel, bEventBuffering);

	[Adjust appDidLaunch:sAppToken];
	[Adjust setEnvironment:sEnvironment];
	[Adjust setLogLevel:eLogLevel];
	[Adjust setSdkPrefix:sSdkPrefix];

    return (s3eResult)0;
}

s3eResult TrackEvent_platform(const char* eventToken, const param_type* params)
{
    return (s3eResult)1;
}

s3eResult TrackEventIphone_platform(const char* eventToken, const char** params_array, int param_size)
{
	NSString *sEventToken = [NSString stringWithUTF8String: eventToken];

	if (param_size == 0) {
		[Adjust trackEvent:sEventToken];
		return (s3eResult)0;
	} 
	NSMutableDictionary *params_mutable_nsdictionary = [[NSMutableDictionary alloc] initWithCapacity:param_size];

	for (int i = 0; i < param_size; i++) {
		const char* keyCstring = params_array[i*2 + 0];
		const char* valueCstring = params_array[i*2 + 1];
		NSString* key = [NSString stringWithUTF8String: keyCstring];
		NSString* value = [NSString stringWithUTF8String: valueCstring];
		
		[params_mutable_nsdictionary setObject:value forKey:key];
	}

	NSDictionary * params_nsdictionary = (NSDictionary *) params_mutable_nsdictionary;

    [Adjust trackEvent:sEventToken withParameters:params_nsdictionary];

    return (s3eResult)0;
}

s3eResult TrackRevenue_platform(double cents, const char* eventToken, const param_type* params)
{
    return (s3eResult)1;
}

s3eResult TrackRevenueIphone_platform(double cents, const char* eventToken, const char** params_array, int param_size)
{
	// without event token and params
    if (eventToken == NULL) {
        [Adjust trackRevenue:cents];
    	return (s3eResult)0;
    }

    NSString *sEventToken = [NSString stringWithUTF8String: eventToken];

    // with event token, but without params
    if (params_array == NULL) {
        [Adjust trackRevenue:cents forEvent:sEventToken];
    	return (s3eResult)0;
	}

	// with both event token and params
	NSMutableDictionary *params_mutable_nsdictionary = [[NSMutableDictionary alloc] initWithCapacity:param_size];

	for (int i = 0; i < param_size; i++) {
		const char* keyCstring = params_array[i*2 + 0];
		const char* valueCstring = params_array[i*2 + 1];
		NSString* key = [NSString stringWithUTF8String: keyCstring];
		NSString* value = [NSString stringWithUTF8String: valueCstring];
		
		[params_mutable_nsdictionary setObject:value forKey:key];
	}

	NSDictionary * params_nsdictionary = (NSDictionary *) params_mutable_nsdictionary;

    [Adjust trackRevenue:cents forEvent:sEventToken withParameters:params_nsdictionary];

    return (s3eResult)0;
}

s3eResult SetEnabled_platform(bool enabled)
{
    BOOL bEnabled = (BOOL) enabled;
    [Adjust setEnabled:bEnabled];

    return (s3eResult)0;
}

s3eResult IsEnabled_platform(bool& isEnabled_out)
{
    BOOL isEnabled = [Adjust isEnabled];
    isEnabled_out = (bool) isEnabled;

    return (s3eResult)0;
}

s3eResult SetResponseDelegate_platform(response_data_delegate delegateFn)
{
    EDK_CALLBACK_REG(ADJUST, ADJUST_RESPONSE_DATA, (s3eCallback)delegateFn, NULL, false);

	adjustMarmaladeInstance = [[AdjustMarmalade_platform alloc] init];
    [Adjust setDelegate:adjustMarmaladeInstance];
    
    return (s3eResult)0;
}
