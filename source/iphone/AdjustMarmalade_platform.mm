/*
 * iphone-specific implementation of the AdjustMarmalade extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "AdjustMarmalade_internal.h"

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
    return S3E_RESULT_ERROR;
}

s3eResult TrackEvent_platform(const char* eventToken, const param_type* params)
{
    return S3E_RESULT_ERROR;
}

s3eResult TrackRevenue_platform(double cents, const char* eventToken, const param_type* params)
{
    return S3E_RESULT_ERROR;
}

s3eResult SetEnabled_platform(bool enabled)
{
    return S3E_RESULT_ERROR;
}

s3eResult IsEnabled_platform(bool& isEnabled_out)
{
    return S3E_RESULT_ERROR;
}

s3eResult SetResponseDelegate_platform(response_data_delegate delegateFn)
{
    return S3E_RESULT_ERROR;
}
