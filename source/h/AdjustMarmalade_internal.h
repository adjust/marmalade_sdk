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

#define S3E_DEVICE_ADJUST                   S3E_EXT_ADJUSTMARMALADE_HASH

typedef enum s3eAdjustCallback
{
    S3E_ADJUST_CALLBACK_ADJUST_ATTRIBUTION_DATA,
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

s3eResult adjust_Start_platform(adjust_config* config);

s3eResult adjust_TrackEvent_platform(adjust_event* event);

s3eResult adjust_SetEnabled_platform(bool is_enabled);

s3eResult adjust_IsEnabled_platform(bool& is_enabled_out);

s3eResult adjust_SetOfflineMode_platform(bool is_offline_mode_enabled);

s3eResult adjust_OnPause_platform();

s3eResult adjust_OnResume_platform();

s3eResult adjust_SetReferrer_platform(const char* referrer);

s3eResult adjust_SetDeviceToken_platform(const char* device_token);

s3eResult adjust_SetAttributionCallback_platform(adjust_attribution_delegate attribution_callback);

char* adjust_CopyString(const char* source);

void adjust_CleanupAttributionCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData);

#endif /* !ADJUSTMARMALADE_INTERNAL_H */