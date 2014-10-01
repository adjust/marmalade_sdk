/*
 * Internal header for the AdjustMarmalade extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef ADJUSTMARMALADE_INTERNAL_H
#define ADJUSTMARMALADE_INTERNAL_H

#include "s3eTypes.h"
#include "AdjustMarmalade.h"
#include "AdjustMarmalade_autodefs.h"

#include <stdio.h>

#define S3E_DEVICE_ADJUST                   S3E_EXT_ADJUSTMARMALADE_HASH

typedef enum s3eAdjustCallback
{
    S3E_ADJUST_CALLBACK_ADJUST_RESPONSE_DATA,
    S3E_ADJUST_CALLBACK_MAX
} s3eAdjustCallback;

/**
 * Global data for AdjustMarmalade extension.
 */
struct AdjustMarmaladeGlobals
{
};

/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult AdjustMarmaladeInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult AdjustMarmaladeInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void AdjustMarmaladeTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void AdjustMarmaladeTerminate_platform();
s3eResult AppDidLaunch_platform(const char* appToken, const char* environment, const char* sdkPrefix, const char* logLevel, bool eventBuffering);

s3eResult TrackEvent_platform(const char* eventToken, const param_type* params);

s3eResult TrackEventIphone_platform(const char* eventToken, const char** params_array, int param_size);

s3eResult TrackRevenue_platform(double cents, const char* eventToken, const param_type* params);

s3eResult TrackRevenueIphone_platform(double cents, const char* eventToken, const char** params_array, int param_size);

s3eResult SetEnabled_platform(bool enabled);

s3eResult IsEnabled_platform(bool& isEnabled_out);

s3eResult SetResponseDelegate_platform(response_data_delegate delegateFn);

void CleanupResponseData_cb(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData);

char* CopyString(const char* source);
#endif /* !ADJUSTMARMALADE_INTERNAL_H */