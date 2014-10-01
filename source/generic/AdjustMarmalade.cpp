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

void CleanupResponseData_cb(uint32 extID, int32 notification, void *systemData, void *instance, int32 returnCode, void *completeData) {

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

char* CopyString(const char* source)
{
	if (source == NULL) {
		return NULL;
	}

	char * target = (char *) malloc(sizeof(char) * strlen(source));
	strcpy(target, source);

	return target;
}

s3eResult AppDidLaunch(const char* appToken, const char* environment, const char* sdkPrefix, const char* logLevel, bool eventBuffering)
{
	return AppDidLaunch_platform(appToken, environment, sdkPrefix, logLevel, eventBuffering);
}

s3eResult TrackEvent(const char* eventToken, const param_type* params)
{
	return TrackEvent_platform(eventToken, params);
}

s3eResult TrackEventIphone(const char* eventToken, const char** params_array, int param_size)
{
	return TrackEventIphone_platform(eventToken, params_array, param_size);
}

s3eResult TrackRevenue(double cents, const char* eventToken, const param_type* params)
{
	return TrackRevenue_platform(cents, eventToken, params);
}

s3eResult TrackRevenueIphone(double cents, const char* eventToken, const char** params_array, int param_size)
{
	return TrackRevenueIphone_platform(cents, eventToken, params_array, param_size);
}

s3eResult SetEnabled(bool enabled)
{
	return SetEnabled_platform(enabled);
}

s3eResult IsEnabled(bool& isEnabled_out)
{
	return IsEnabled_platform(isEnabled_out);
}

s3eResult SetResponseDelegate(response_data_delegate delegateFn)
{
	return SetResponseDelegate_platform(delegateFn);
}
