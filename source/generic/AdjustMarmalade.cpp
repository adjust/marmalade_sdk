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

void adjust_CleanupAttributionCallback(uint32 extID, int32 notification, void *systemData, void *instance, 
    int32 returnCode, void *completeData)
{
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

    delete attribution;
}

char* adjust_CopyString(const char* source)
{
    if (source == NULL) {
        return NULL;
    }

    char* target = (char*) malloc(sizeof(char) * strlen(source));
    strcpy(target, source);

    return target;
}

s3eResult adjust_Start(adjust_config* config)
{
    config->set_sdk_prefix("marmalade4.0.1");
    
    return adjust_Start_platform(config);
}

s3eResult adjust_TrackEvent(adjust_event* event)
{
    return adjust_TrackEvent_platform(event);
}

s3eResult adjust_SetEnabled(bool is_enabled)
{
    return adjust_SetEnabled_platform(is_enabled);
}

s3eResult adjust_IsEnabled(bool& is_enabled_out)
{
    return adjust_IsEnabled_platform(is_enabled_out);
}

s3eResult adjust_SetOfflineMode(bool is_offline_mode_enabled)
{
    return adjust_SetOfflineMode_platform(is_offline_mode_enabled);
}

s3eResult adjust_OnPause()
{
    return adjust_OnPause_platform();
}

s3eResult adjust_OnResume()
{
    return adjust_OnResume_platform();
}

s3eResult adjust_SetReferrer(const char* referrer)
{
    return adjust_SetReferrer_platform(referrer);
}

s3eResult adjust_SetDeviceToken(const char* device_token)
{
    return adjust_SetDeviceToken_platform(device_token);
}
