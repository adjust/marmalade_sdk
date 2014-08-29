/*
java implementation of the AdjustMarmalade extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import com.ideaworks3d.marmalade.SuspendResumeListener;
import com.ideaworks3d.marmalade.SuspendResumeEvent;
import com.adjust.sdk.Adjust;
import com.adjust.sdk.AdjustFactory;
import com.adjust.sdk.Logger;
import java.util.Map;

class AdjustMarmalade
{
    public void AppDidLaunch(String appToken, String environment, String sdkPrefix, String logLevel, boolean eventBuffering)
    {
        LoaderActivity activity = LoaderAPI.getActivity();
        Adjust.appDidLaunch(activity,
                            appToken,
                            environment,
                            logLevel,
                            eventBuffering);
        Adjust.setSdkPrefix(sdkPrefix);
        Adjust.onResume(activity);

        LoaderAPI.addSuspendResumeListener(new SuspendResumeListener() {

            @Override
            public void onSuspendResumeEvent(SuspendResumeEvent paramSuspendResumeEvent) {
                if (paramSuspendResumeEvent.eventType == 
                    SuspendResumeEvent.EventType.SUSPEND) {
                    Adjust.onPause();
                }

                if (paramSuspendResumeEvent.eventType ==
                    SuspendResumeEvent.EventType.RESUME) {
                    Adjust.onResume(LoaderAPI.getActivity());
                }
            }
        });
    }
    public void TrackEvent(String eventToken, Map<String, String> parameters)
    {
        Adjust.trackEvent(eventToken, parameters);
    }
    public void TrackRevenue(double cents, String eventToken, Map<String, String> parameters)
    {
        Adjust.trackRevenue(cents, eventToken, parameters);     
    }
    public void setEnabled(boolean enabled)
    {
        Adjust.setEnabled(enabled);
    }
    public boolean isEnabled()
    {
        return Adjust.isEnabled();
    }
}
