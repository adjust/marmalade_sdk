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
import android.util.Log;
import com.adjust.sdk.*;
import org.json.JSONObject;
import org.json.JSONException;

import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import com.ideaworks3d.marmalade.SuspendResumeListener;
import com.ideaworks3d.marmalade.SuspendResumeEvent;

public class AdjustMarmalade implements OnAttributionChangedListener {
    public native void attributionCallback(String attributionString);

    public void adjust_Start(String appToken, String environment, String logLevel, String sdkPrefix,
        boolean isEventBufferingEnabled, String processName, String defaultTracker, boolean isMacMd5TrackingEnabled,
        boolean isAttributionCallbackSet) {

        if (isStringValid(appToken) && isStringValid(environment)) {
            AdjustConfig adjustConfig = new AdjustConfig(LoaderAPI.getActivity(), appToken, environment);

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

            if (isAttributionCallbackSet) {
                adjustConfig.setOnAttributionChangedListener(this);    
            }

            adjustConfig.setEventBufferingEnabled(isEventBufferingEnabled);

            Adjust.onCreate(adjustConfig);
            Adjust.onResume();
        }
    }

    public void adjust_TrackEvent(String eventToken, String currency, String transactionId, String receipt, 
        double revenue, Map<String, String> callbackParams, Map<String, String> partnerParams, boolean isReceiptSet) {
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

    public void adjust_OnPause() {
        Adjust.onPause();
    }

    public void adjust_OnResume() {
        Adjust.onResume();
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

    private boolean isStringValid(String string) {
        if (string != null && !string.equals("")) {
            return true;
        } else {
            return false;
        }
    }
}
