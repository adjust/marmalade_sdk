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

class AdjustMarmalade
{
    public int AppDidLaunch(String appToken, String environment, String sdkPrefix, String logLevel, boolean eventBuffering)
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

        return 0;
    }
    public int TrackEvent(String eventToken, int params)
    {
        if (params == null)
            LoaderAPI.trace("params null");
        else
            LoaderAPI.trace("params class name: " + params.getClass().getName());

        Adjust.trackEvent(eventToken);
        return 0;
    }
    public int TrackRevenue(double cents, String eventToken)
    {
        if(eventToken != null && !eventToken.isEmpty()) {
            Adjust.trackRevenue(cents, eventToken);
        } else {
            Adjust.trackRevenue(cents);
        }

        return 0;
    }
    public int setEnabled(boolean enabled)
    {
        Adjust.setEnabled(enabled);
        return 0;
    }
    public int isEnabled()
    {
        return 0;
    }
}
