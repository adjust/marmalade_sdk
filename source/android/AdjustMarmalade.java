/*
java implementation of the AdjustMarmalade extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

import java.util.Map;
import android.net.Uri;
import com.adjust.sdk.*;
import org.json.JSONObject;
import org.json.JSONException;
import android.content.Intent;

import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import com.ideaworks3d.marmalade.LoaderActivitySlave;
import com.ideaworks3d.marmalade.SuspendResumeListener;
import com.ideaworks3d.marmalade.SuspendResumeEvent;

public class AdjustMarmalade extends LoaderActivitySlave implements OnAttributionChangedListener, OnSessionTrackingSucceededListener,
                OnSessionTrackingFailedListener, OnEventTrackingSucceededListener, OnEventTrackingFailedListener, OnDeeplinkResponseListener {
    public native void attributionCallback(String attributionString);
    public native void sessionSuccessCallback(String sessionSuccessString);
    public native void sessionFailureCallback(String sessionFailureString);
    public native void eventSuccessCallback(String eventSuccessString);
    public native void eventFailureCallback(String eventFailureString);
    public native void deeplinkCallback(String deferredDeeplinkString);
    public native void deferredDeeplinkCallback(String deferredDeeplinkString);
    public native void googleAdIdCallback(String googleAdIdString);
    public native void idfaCallback(String idfaString);

    private boolean shouldDeferredDeeplinkBeOpened = true;
    private boolean isGoogleAdIdCallbackSet = false;
    private boolean isIdfaCallbackSet = false;
    private boolean isDeeplinkCallbackSet = false;

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        Uri uriData = intent.getData();

        if (uriData == null) {
            return;
        }

        Adjust.appWillOpenUrl(uriData);

        if (isDeeplinkCallbackSet == false) {
            return;
        }

        deeplinkCallback(uriData.toString());
    }

    @Override
    protected void onResume() {
        super.onResume();
        Adjust.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Adjust.onPause();
    }

    public void adjust_Start(String appToken, 
        String environment,
        String logLevel,
        String sdkPrefix,
        double delayStart,
        boolean isEventBufferingEnabled,
        boolean isSendingInBackgroundEnabled,
        boolean shouldDeferredDeeplinkBeOpened,
        String processName,
        String defaultTracker,
        String userAgent,
        boolean isAttributionCallbackSet,
        boolean isSessionSuccessCallbackSet,
        boolean isSessionFailureCallbackSet,
        boolean isEventSuccessCallbackSet,
        boolean isEventFailureCallbackSet,
        boolean isDeeplinkCallbackSet,
        boolean isDeferredDeeplinkCallbackSet,
        boolean isGoogleAdIdCallbackSet,
        boolean isIdfaCallbackSet) {
        if (isStringValid(appToken) && isStringValid(environment)) {
            boolean isSuppress = false;

            if (isStringValid(logLevel)) {
                if (logLevel.equals("suppress")) {
                    isSuppress = true;
                }
            }

            AdjustConfig adjustConfig = new AdjustConfig(LoaderAPI.getActivity(), appToken, environment, isSuppress);

            if (isStringValid(logLevel)) {
                if (logLevel.equals("verbose")) {
                    adjustConfig.setLogLevel(LogLevel.VERBOSE);
                } else if (logLevel.equals("debug")) {
                    adjustConfig.setLogLevel(LogLevel.DEBUG);
                } else if (logLevel.equals("info")) {
                    adjustConfig.setLogLevel(LogLevel.INFO);
                } else if (logLevel.equals("warn")) {
                    adjustConfig.setLogLevel(LogLevel.WARN);
                } else if (logLevel.equals("error")) {
                    adjustConfig.setLogLevel(LogLevel.ERROR);
                } else if (logLevel.equals("assert")) {
                    adjustConfig.setLogLevel(LogLevel.ASSERT);
                } else if (logLevel.equals("suppress")) {
                    adjustConfig.setLogLevel(LogLevel.SUPRESS);
                }
            }

            if (isStringValid(processName)) {
                adjustConfig.setProcessName(processName);
            }

            if (isStringValid(defaultTracker)) {
                adjustConfig.setDefaultTracker(defaultTracker);
            }

            if (isStringValid(sdkPrefix)) {
                adjustConfig.setSdkPrefix(sdkPrefix);
            }

            if (isStringValid(userAgent)) {
                adjustConfig.setUserAgent(userAgent);
            }

            if (delayStart != -1) {
                adjustConfig.setDelayStart(delayStart);
            }

            if (isAttributionCallbackSet) {
                adjustConfig.setOnAttributionChangedListener(this);    
            }

            if (isSessionSuccessCallbackSet) {
                adjustConfig.setOnSessionTrackingSucceededListener(this);
            }

            if (isSessionFailureCallbackSet) {
                adjustConfig.setOnSessionTrackingFailedListener(this);
            }

            if (isEventSuccessCallbackSet) {
                adjustConfig.setOnEventTrackingSucceededListener(this);
            }

            if (isEventFailureCallbackSet) {
                adjustConfig.setOnEventTrackingFailedListener(this);
            }

            if (isDeferredDeeplinkCallbackSet) {
                adjustConfig.setOnDeeplinkResponseListener(this);
            }

            this.shouldDeferredDeeplinkBeOpened = shouldDeferredDeeplinkBeOpened;
            this.isGoogleAdIdCallbackSet = isGoogleAdIdCallbackSet;
            this.isIdfaCallbackSet = isIdfaCallbackSet;
            this.isDeeplinkCallbackSet = isDeeplinkCallbackSet;

            adjustConfig.setEventBufferingEnabled(isEventBufferingEnabled);
            adjustConfig.setSendInBackground(isSendingInBackgroundEnabled);

            Adjust.onCreate(adjustConfig);
            Adjust.onResume();
        }
    }

    public void adjust_TrackEvent(String eventToken,
        String currency,
        String transactionId,
        String receipt,
        double revenue,
        Map<String, String> callbackParams,
        Map<String, String> partnerParams,
        boolean isReceiptSet) {
        if (isStringValid(eventToken)) {
            AdjustEvent adjustEvent = new AdjustEvent(eventToken);

            if (revenue != -1) {
                adjustEvent.setRevenue(revenue, currency);
            }

            for (Map.Entry<String, String> parameter : callbackParams.entrySet()) {
                adjustEvent.addCallbackParameter(parameter.getKey(), parameter.getValue());
            }

            for (Map.Entry<String, String> parameter : partnerParams.entrySet()) {
                adjustEvent.addPartnerParameter(parameter.getKey(), parameter.getValue());
            }

            if (null != transactionId) {
                adjustEvent.setOrderId(transactionId);
            }

            Adjust.trackEvent(adjustEvent);
        }
    }

    public void adjust_SetEnabled(boolean isEnabled) {
        Adjust.setEnabled(isEnabled);
    }

    public boolean adjust_IsEnabled() {
        return Adjust.isEnabled();
    }

    public void adjust_SetOfflineMode(boolean isEnabled) {
        Adjust.setOfflineMode(isEnabled);
    }

    public void adjust_SetReferrer(String referrer) {
        Adjust.setReferrer(referrer);
    }

    public void adjust_SetDeviceToken(String deviceToken) {
        Adjust.setPushToken(deviceToken);
    }

    public void adjust_SendFirstPackages() {
        Adjust.sendFirstPackages();
    }

    public void adjust_AddSessionCallbackParameter(String key, String value) {
        Adjust.addSessionCallbackParameter(key, value);
    }

    public void adjust_AddSessionPartnerParameter(String key, String value) {
        Adjust.addSessionPartnerParameter(key, value);
    }

    public void adjust_RemoveSessionCallbackParameter(String key) {
        Adjust.removeSessionCallbackParameter(key);
    }

    public void adjust_RemoveSessionPartnerParameter(String key) {
        Adjust.removeSessionPartnerParameter(key);
    }

    public void adjust_ResetSessionCallbackParameters() {
        Adjust.resetSessionCallbackParameters();
    }
    public void adjust_ResetSessionPartnerParameters() {
        Adjust.resetSessionPartnerParameters();
    }

    public void adjust_GetGoogleAdId() {
        if (this.isGoogleAdIdCallbackSet) {
            Adjust.getGoogleAdId(LoaderAPI.getActivity(), new OnDeviceIdsRead() {
                @Override
                public void onGoogleAdIdRead(String googleAdId) {
                    if (googleAdId != null) {
                        googleAdIdCallback(googleAdId);
                    } else {
                        googleAdIdCallback("");
                    }
                }
            });
        } else {
            googleAdIdCallback("");
        }
    }

    public void adjust_GetIdfa() {
        if (this.isIdfaCallbackSet) {
            idfaCallback("");
        }
    }

    @Override
    public void onAttributionChanged(AdjustAttribution attribution) {
        JSONObject jsonAttribution = new JSONObject();

        try {
            jsonAttribution.put("tracker_token", attribution.trackerToken);
            jsonAttribution.put("tracker_name", attribution.trackerName);
            jsonAttribution.put("network", attribution.network);
            jsonAttribution.put("campaign", attribution.campaign);
            jsonAttribution.put("ad_group", attribution.adgroup);
            jsonAttribution.put("creative", attribution.creative);
            jsonAttribution.put("click_label", attribution.clickLabel);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        attributionCallback(jsonAttribution.toString());
    }

    @Override
    public void onFinishedSessionTrackingSucceeded(AdjustSessionSuccess sessionSuccess) {
        JSONObject jsonSessionSuccess = new JSONObject();

        try {
            jsonSessionSuccess.put("message", sessionSuccess.message);
            jsonSessionSuccess.put("timestamp", sessionSuccess.timestamp);
            jsonSessionSuccess.put("adid", sessionSuccess.adid);

            if (sessionSuccess.jsonResponse != null) {
                jsonSessionSuccess.put("json_response", sessionSuccess.jsonResponse.toString());
            } else {
                jsonSessionSuccess.put("json_response", "");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        sessionSuccessCallback(jsonSessionSuccess.toString());
    }

    @Override
    public void onFinishedSessionTrackingFailed(AdjustSessionFailure sessionFailure) {
        JSONObject jsonSessionFailure = new JSONObject();

        try {
            jsonSessionFailure.put("message", sessionFailure.message);
            jsonSessionFailure.put("timestamp", sessionFailure.timestamp);
            jsonSessionFailure.put("adid", sessionFailure.adid);
            jsonSessionFailure.put("will_retry", String.valueOf(sessionFailure.willRetry));

            if (sessionFailure.jsonResponse != null) {
                jsonSessionFailure.put("json_response", sessionFailure.jsonResponse.toString());
            } else {
                jsonSessionFailure.put("json_response", "");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        sessionFailureCallback(jsonSessionFailure.toString());
    }

    @Override
    public void onFinishedEventTrackingSucceeded(AdjustEventSuccess eventSuccess) {
        JSONObject jsonEventSuccess = new JSONObject();

        try {
            jsonEventSuccess.put("message", eventSuccess.message);
            jsonEventSuccess.put("timestamp", eventSuccess.timestamp);
            jsonEventSuccess.put("event_token", eventSuccess.eventToken);
            jsonEventSuccess.put("adid", eventSuccess.adid);

            if (eventSuccess.jsonResponse != null) {
                jsonEventSuccess.put("json_response", eventSuccess.jsonResponse.toString());
            } else {
                jsonEventSuccess.put("json_response", "");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        eventSuccessCallback(jsonEventSuccess.toString());
    }

    @Override
    public void onFinishedEventTrackingFailed(AdjustEventFailure eventFailure) {
        JSONObject jsonEventFailure = new JSONObject();

        try {
            jsonEventFailure.put("message", eventFailure.message);
            jsonEventFailure.put("timestamp", eventFailure.timestamp);
            jsonEventFailure.put("event_token", eventFailure.eventToken);
            jsonEventFailure.put("adid", eventFailure.adid);
            jsonEventFailure.put("will_retry", String.valueOf(eventFailure.willRetry));

            if (eventFailure.jsonResponse != null) {
                jsonEventFailure.put("json_response", eventFailure.jsonResponse.toString());
            } else {
                jsonEventFailure.put("json_response", "");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        eventFailureCallback(jsonEventFailure.toString());
    }

    @Override
    public boolean launchReceivedDeeplink(Uri deeplink) {
        deferredDeeplinkCallback(deeplink.toString());
        return this.shouldDeferredDeeplinkBeOpened;
    }

    private boolean isStringValid(String string) {
        if (string != null && !string.equals("")) {
            return true;
        } else {
            return false;
        }
    }
}
