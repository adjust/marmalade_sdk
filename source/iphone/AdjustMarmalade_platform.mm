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

	const char * source = [string UTF8String];
    char * target = adjust_CopyString(source);
    return target;
}

- (adjust_response_data*) getResponseData:(AIResponseData *) adjustResponseData {
	adjust_response_data* rd = new adjust_response_data();

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

	adjust_response_data* rd = [self getResponseData:responseData];

    s3eEdkCallbacksEnqueue(S3E_DEVICE_ADJUST,
                        S3E_ADJUST_CALLBACK_ADJUST_RESPONSE_DATA,
                        rd,
                        sizeof(*rd),
                        NULL,
                        S3E_FALSE,
                        &adjust_CleanupResponseDataCallback,
                        (void*)rd);
}

+ (NSDictionary*) ConvertToNSDictionary:(const char**) params_array param_size:(int) param_size {
    if (param_size == 0) {
        return nil;
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
    return params_nsdictionary;
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

s3eResult adjust_AppDidLaunch_platform(const char* appToken, const char* environment, const char* sdkPrefix, const char* logLevel, bool eventBuffering)
{
	NSString* sAppToken = [NSString stringWithUTF8String: appToken];
	NSString* sEnvironment = [NSString stringWithUTF8String: environment];
	NSString* sSdkPrefix = [NSString stringWithUTF8String: sdkPrefix];
	//AILogLevel eLogLevel = (AILogLevel)logLevel;
	// TODO parse log level
	AILogLevel eLogLevel = (AILogLevel)AILogLevelVerbose;
	BOOL bEventBuffering = (BOOL) eventBuffering;

    [Adjust appDidLaunch:sAppToken];
    [Adjust setEnvironment:sEnvironment];
    [Adjust setLogLevel:eLogLevel];
    [Adjust setSdkPrefix:sSdkPrefix];

    return (s3eResult)0;
}

s3eResult adjust_TrackEvent_platform(const char* eventToken, const adjust_param_type* params)
{
    return (s3eResult)1;
}

s3eResult adjust_TrackEventIphone_platform(const char* eventToken, const char** params_array, int param_size)
{
	NSString *sEventToken = [NSString stringWithUTF8String: eventToken];

	if (param_size == 0) {
		[Adjust trackEvent:sEventToken];
		return (s3eResult)0;
	}

	NSDictionary * params_nsdictionary = [AdjustMarmalade_platform ConvertToNSDictionary:params_array param_size:param_size];

    [Adjust trackEvent:sEventToken withParameters:params_nsdictionary];

    return (s3eResult)0;
}

s3eResult adjust_TrackRevenue_platform(double cents, const char* eventToken, const adjust_param_type* params)
{
    return (s3eResult)1;
}

s3eResult adjust_TrackRevenueIphone_platform(double cents, const char* eventToken, const char** params_array, int param_size)
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
    NSDictionary * params_nsdictionary = [AdjustMarmalade_platform ConvertToNSDictionary:params_array param_size:param_size];

    [Adjust trackRevenue:cents forEvent:sEventToken withParameters:params_nsdictionary];

    return (s3eResult)0;
}

s3eResult adjust_SetEnabled_platform(bool enabled)
{
    BOOL bEnabled = (BOOL) enabled;
    [Adjust setEnabled:bEnabled];

    return (s3eResult)0;
}

s3eResult adjust_IsEnabled_platform(bool& isEnabled_out)
{
    BOOL isEnabled = [Adjust isEnabled];
    isEnabled_out = (bool) isEnabled;

    return (s3eResult)0;
}

s3eResult adjust_SetResponseDelegate_platform(adjust_response_data_delegate delegateFn)
{
    EDK_CALLBACK_REG(ADJUST, ADJUST_RESPONSE_DATA, (s3eCallback)delegateFn, NULL, false);

	adjustMarmaladeInstance = [[AdjustMarmalade_platform alloc] init];
    [Adjust setDelegate:adjustMarmaladeInstance];
    
    return (s3eResult)0;
}
