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
s3eResult AdjustMarmaladeInit()
{
    //Add any generic initialisation code here
    return AdjustMarmaladeInit_platform();
}

void AdjustMarmaladeTerminate()
{
    //Add any generic termination code here
    AdjustMarmaladeTerminate_platform();
}

void adjust_CleanupResponseDataCallback(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {

    adjust_response_data * rd = (adjust_response_data *) completeData;
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

char* adjust_CopyString(const char* source)
{
    if (source == NULL) {
        return NULL;
    }

    char * target = (char *) malloc(sizeof(char) * strlen(source));
    strcpy(target, source);

    return target;
}

s3eResult adjust_AppDidLaunch(const char* appToken, const char* environment, const char* logLevel, bool eventBuffering)
{
    const char* sdkPrefix = "marmalade3.4.0";
    return adjust_AppDidLaunch_platform(appToken, environment, sdkPrefix, logLevel, eventBuffering);
}

s3eResult adjust_TrackEvent(const char* eventToken, const adjust_param_type* params)
{
    return adjust_TrackEvent_platform(eventToken, params);
}

s3eResult adjust_TrackEventIphone(const char* eventToken, const char** params_array, int param_size)
{
    return adjust_TrackEventIphone_platform(eventToken, params_array, param_size);
}

s3eResult adjust_TrackRevenue(double cents, const char* eventToken, const adjust_param_type* params)
{
    return adjust_TrackRevenue_platform(cents, eventToken, params);
}

s3eResult adjust_TrackRevenueIphone(double cents, const char* eventToken, const char** params_array, int param_size)
{
    return adjust_TrackRevenueIphone_platform(cents, eventToken, params_array, param_size);
}

s3eResult adjust_SetEnabled(bool enabled)
{
    return adjust_SetEnabled_platform(enabled);
}

s3eResult adjust_IsEnabled(bool& isEnabled_out)
{
    return adjust_IsEnabled_platform(isEnabled_out);
}

s3eResult adjust_SetResponseDelegate(adjust_response_data_delegate delegateFn)
{
    return adjust_SetResponseDelegate_platform(delegateFn);
}
