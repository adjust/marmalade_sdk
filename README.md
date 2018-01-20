## Summary

This is the Marmalade SDK of Adjust™. You can read more about Adjust™ at [adjust.com].

## Table of contents

* [Example app](#example-app)
* [Basic integration](#basic-integration)
   * [Get the SDK](#sdk-get)
   * [Add the SDK to your project](#sdk-add)
   * [Integrate the SDK into your app](#sdk-integrate)
   * [Adjust logging](#adjust-logging)
   * [Adjust project settings](#adjust-project-settings)
      * [Android permissions](#android-permissions)
      * [Google Play Services](#android-gps)
      * [Proguard settings](#android-proguard)
      * [Android install referrer](#android-broadcast-receiver)
      * [iOS frameworks](#ios-frameworks)
* [Additional features](#additional-features)
   * [Event tracking](#event-tracking)
      * [Revenue tracking](#revenue-tracking)
      * [Revenue deduplication](#revenue-deduplication)
      * [In-App Purchase verification](#iap-verification)
      * [Callback parameters](#callback-parameters)
      * [Partner parameters](#partner-parameters)
    * [Session parameters](#session-parameters)
      * [Session callback parameters](#session-callback-parameters)
      * [Session partner parameters](#session-partner-parameters)
      * [Delay start](#delay-start)
    * [Attribution callback](#attribution-callback)
    * [Session and event callbacks](#session-event-callbacks)
    * [Disable tracking](#disable-tracking)
    * [Offline mode](#offline-mode)
    * [Event buffering](#event-buffering)
    * [Background tracking](#background-tracking)
    * [Device IDs](#device-ids)
      * [iOS advertising identifier](#di-idfa)
      * [Google Play Services advertising identifier](#di-gps-adid)
      * [Adjust device identifier](#di-adid)
    * [User attribution](#user-attribution)
    * [Push token](#push-token)
    * [Pre-installed trackers](#pre-installed-trackers)
    * [Deep linking](#deeplinking)
        * [Standard deep linking scenario](#deeplinking-standard)
        * [Deferred deep linking scenario](#deeplinking-deferred)
        * [Deep linking setup for Android](#deeplinking-android)
        * [Deep linking setup for iOS](#deeplinking-ios)
        * [Reattribution via deep links](#deeplinking-reattribution)
* [License](#license)

## <a id="example-app"></a>Example app

There is an example app inside the [`example` directory][example-app]. You can use the example app to see how the Adjust SDK can be integrated.

## <a id="basic-integration">Basic integration

These are the minimal steps required to integrate the Adjust SDK into your Marmalade project.

**Note**: SDK 4.11.1 for Marmalade is built with **Marmalade 8.6.0** and we advise you to use this Marmalade version or higher. Especially if you want to rebuild our Marmalade extension on your own.

### <a id="sdk-get">Get the SDK

Download the latest version from our [releases page][releases]. Extract the zip file in a folder of your choice.

### <a id="sdk-add">Add the SDK to your project

Add `AdjustMarmalade` as a subproject of your own.  Edit the `.mkb` file of your project and add the following line:

```
subproject path_to_folder/adjust/AdjustMarmalade
```

### <a id="sdk-integrate">Integrate the SDK into your app

Add the `AdjustMarmalade.h` header to your project where the `main` function is located. In the `main` function, initialize `adjust_config` structure and call the function `adjust_Start`.

```cpp
#include "AdjustMarmalade.h"

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);

    adjust_Start(config);

    // ...
}
```

Replace `{YourAppToken}` with your app token. You can this find in your [dashboard].

Depending on whether you build your app for testing or for production, you must set `environment` with one of these values:

```cpp
const char* environment = "sandbox";
const char* environment = "production";
```

**Important:** This value should be set to `sandbox` if and only if you or someone else is testing your app. Make sure to set the environment to `production` just before you publish the app. Set it back to `sandbox` when you start developing and testing it again.

We use this environment to distinguish between real traffic and test traffic from test devices. It is very important that you keep this value meaningful at all times! This is especially important if you are tracking revenue.

### <a id="sdk-logging">Adjust logging

You can increase or decrease the amount of logs you see in tests by calling `set_log_level` on your `adjust_config` instance with one of the following parameters:

```cpp
config->set_log_level("verbose");   // enable all logging
config->set_log_level("debug");     // enable more logging
config->set_log_level("info");      // the default
config->set_log_level("warn");      // disable info logging
config->set_log_level("error");     // disable warnings as well
config->set_log_level("assert");    // disable errors as well
config->set_log_level("suppress");  // disable all log output
```

### <a id="adjust-project-settings">Adjust project settings

Once the Adjust SDK has been added to your app, certain tweeks are being performed so that the Adjust SDK can work properly. Below you can find a description of every additional thing that the Adjust SDK performs after you've added it to your app.

### <a id="android-permissions">Android permissions

The Adjust SDK adds two permissions to your Android manifest file: `INTERNET` and `ACCESS_WIFI_STATE`. You can see in the `AdjustMarmalade.mkf` file of the Adjust SDK that the `android-extra-manifest` property is being set with content from the `adjust_permissions.xml` file, which is part of the Adjust SDK:

```xml
<uses-permission android:name="android.permission.INTERNET"/>
<!-- ACCESS_WIFI_STATE is not needed if your app is using Google Play Services library -->
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE"/>
```

The `INTERNET` permission might be needed by our SDK at any point in time. The `ACCESS_WIFI_STATE` permission is needed by the Adjust SDK if your app is not targeting the Google Play Store and doesn't use Google Play Services. If you are targeting the Google Play Store and you are using Google Play Services, the Adjust SDK doesn't need this permission and, if you don't need it anywhere else in your app, you can remove it.

### <a id="android-gps">Google Play Services

Since 1 August 2014, all apps in the Google Play Store must use the [Google Advertising ID][google_ad_id] to uniquely identify devices. To allow the Adjust SDK to use the Google Advertising ID, you must integrate the [Google Play Services][google_play_services].

In order to add Google Play Services to your Marmalade app, you should edit your app's `.mkb` file and add  `s3eGooglePlayServices` in the `subprojects` list. In addition to this, you should add the following line to `deployment` list of your `.mkb` file:

```
android-extra-strings='(gps_app_id,your.app.package)'
```

After this, Google Play Services will be successfully integrated in your Marmalade app.

### <a id="android-proguard">Proguard settings

If you are using Proguard, add these lines to your Proguard file:

```
-keep public class com.adjust.sdk.** { *; }
-keep class com.google.android.gms.common.ConnectionResult {
    int SUCCESS;
}
-keep class com.google.android.gms.ads.identifier.AdvertisingIdClient {
    com.google.android.gms.ads.identifier.AdvertisingIdClient$Info getAdvertisingIdInfo(android.content.Context);
}
-keep class com.google.android.gms.ads.identifier.AdvertisingIdClient$Info {
    java.lang.String getId();
    boolean isLimitAdTrackingEnabled();
}
-keep class dalvik.system.VMRuntime {
    java.lang.String getRuntime();
}
-keep class android.os.Build {
    java.lang.String[] SUPPORTED_ABIS;
    java.lang.String CPU_ABI;
}
-keep class android.content.res.Configuration {
    android.os.LocaledList getLocales();
    java.util.Locale locale;
}
-keep class android.os.LocaledList {
    java.util.Locale get(int);
}
```

### <a id="android-broadcast-receiver">Android install referrer

The Adjust install referrer broadcast receiver is added to your app by default. For more information, you can check our native [Android SDK README][broadcast-receiver]. You can see in the `AdjustMarmalade.mkf` file that the `android-extra-application-manifest` property is being set with content from the `adjust_broadcast_receiver.xml` file:

```xml
<receiver android:name="com.adjust.sdk.AdjustReferrerReceiver" 
          android:exported="true" >
    <intent-filter>
        <action android:name="com.android.vending.INSTALL_REFERRER" />
    </intent-filter>
</receiver>
```

Please bear in mind that, if you are using your own broadcast receiver which handles the INSTALL_REFERRER intent, you don't need to add the Adjust broadcast receiver to your manifest file. You can remove it, but inside your own receiver add the call to the Adjust broadcast receiver as described in our [Android guide][broadcast-receiver-custom].

### <a id="ios-frameworks">iOS frameworks

The Adjust SDK plugin adds three iOS frameworks to your generated Xcode project:

* `iAd.framework` - in case you are running iAd campaigns.
* `AdSupport.framework` - for reading iOS Advertising Id (IDFA).
* `AdjustSdk.framework` - our native iOS SDK framework.

Settings for this can also be found in `AdjustMarmalade.mkf` file of the Adjust SDK:

```
iphone-link-opts="-weak_framework AdSupport -weak_framework iAd"
iphone-link-opts="-F$CWD/sdk/iOS -framework AdjustSdk -ObjC"
```

If you are not running any iAd campaigns, you can feel free to remove the `iAd.framework` dependency.

## <a id="additional-features">Additional features

You can take advantage of the following features once the Adjust SDK is integrated into your project.

### <a id="event-tracking">Event tracking

With Adjust, you can track every event that you want. Suppose you want to track every tap on a button. Simply create a new event token in your [dashboard]. Let's say that event token is `abc123`. You can add the following line in your button’s click handler method to track the click:

```cpp
adjust_event* event = new adjust_event("abc123");
adjust_TrackEvent(event);
```

### <a id="revenue-tracking">Revenue tracking

If your users can generate revenue by tapping on advertisements or making In-App Purchases, then you can track those revenues with events. Let's say a tap is worth €0.01. You could track the revenue event like this:

```cpp
adjust_event* event = new adjust_event("abc123");
event->set_revenue(0.01, "EUR");

adjust_TrackEvent(event);
```

### <a id="revenue-deduplication"></a>Revenue deduplication

You can also add an optional transaction ID to avoid tracking duplicate revenues. The last ten transaction IDs are remembered, and revenue events with duplicate transaction IDs are skipped. This is especially useful for in-app purchase tracking. You can see an example below.

If you want to track in-app purchases, please make sure to call the `adjust_TrackEvent` only if the transaction is finished
and item is purchased. That way you can avoid tracking revenue that is not actually being generated.

```cpp
adjust_event* event = new adjust_event("abc123");

event->set_revenue(0.01, "EUR");
event->set_transaction_id("transaction_id");

adjust_TrackEvent(event);
```

**Note**: Transaction ID is the iOS term, unique identifier for successfully finished Android In-App-Purchases is named **Order ID**.

### <a id="iap-verification">In-App Purchase verification

In-App Purchase Verification can be done with Marmalade purchase SDK which is currently being developed and will soon be publicly available. For more information, please contact support@adjust.com.

### <a id="callback-parameters">Callback parameters

You can also register a callback URL for that event in your [dashboard][dashboard] and we will send a GET request to that URL whenever the event gets tracked. In that case you can also put some key-value pairs in an object and pass it to the `adjust_TrackEvent` method. We will then append these named parameters to your callback URL.

For example, suppose you have registered the URL `http://www.adjust.com/callback` for your event with event token `abc123` and execute the following lines:

```cpp
adjust_event* event = new adjust_event("abc123");

event->add_callback_parameter("key", "value");
event->add_callback_parameter("foo", "bar");

adjust_TrackEvent(event);
```

In this case we would track the event and send a request to:

```
http://www.adjust.com/callback?key=value&foo=bar
```

It should be mentioned that we support a variety of placeholders like `{idfa}` for iOS or `{gps_adid}` for Android that can be used as parameter values.  In the resulting callback the `{idfa}` placeholder would be replaced with the ID for Advertisers of the current device for iOS and the `{gps_adid}` would be replaced with the Google Advertising ID of the current device for Android. Also note that we don't store any of your custom parameters, but only append them to your callbacks. If you haven't registered a callback for an event, these parameters won't even be read.

You can read more about using URL callbacks, including a full list of available values, in our [callbacks guide][callbacks-guide].

### <a id="partner-parameters">Partner parameters

Similarly to the callback parameters mentioned above, you can also add parameters that Adjust will transmit to the network partners of your choice. You can activate these networks in your Adjust dashboard.

For partner parameters to be added, you would need to call the `add_partner_parameter` method on your `adjust_event` instance.

```cpp
adjust_event* event = new adjust_event("abc123");

event->add_partner_parameter("key", "value");
event->add_partner_parameter("foo", "bar");

adjust_TrackEvent(event);
```

You can read more about special partners and networks in our [guide to special partners][special-partners].

### <a id="session-parameters">Session parameters

Some parameters are saved to be sent in every event and session of the Adjust SDK. Once you have added any of these parameters, you don't need to add them every time, since they will be saved locally. If you add the same parameter twice, there will be no effect.

These session parameters can be called before the Adjust SDK is launched to make sure they are sent even on install. If you need to send them with an install, but can only obtain the needed values after launch, it's possible to [delay](#delay-start) the first launch of the Adjust SDK to allow this behaviour.

### <a id="session-callback-parameters"> Session callback parameters

The same callback parameters that are registered for [events](#callback-parameters) can be also saved to be sent in every event or session of the Adjust SDK.

The session callback parameters have a similar interface of the event callback parameters. Instead of adding the key and it's value to an event, it's added through a call to method `adjust_AddSessionCallbackParameter`:

```cpp
adjust_AddSessionCallbackParameter("foo", "bar");
```

The session callback parameters will be merged with the callback parameters added to an event. The callback parameters added to an event have precedence over the session callback parameters. Meaning that, when adding a callback parameter to an event with the same key to one added from the session, the value that prevails is the callback parameter added to the event.

It's possible to remove a specific session callback parameter by passing the desiring key to the method `adjust_RemoveSessionCallbackParameter`.

```cpp
adjust_RemoveSessionCallbackParameter("foo");
```

If you wish to remove all key and values from the session callback parameters, you can reset it with the method `adjust_ResetSessionCallbackParameters`.

```cpp
adjust_ResetSessionCallbackParameters();
```

### <a id="session-partner-parameters">Session partner parameters

In the same way that there is [session callback parameters](#session-callback-parameters) that are sent for every event or session of the Adjust SDK, there is also session partner parameters.

These will be transmitted to network partners, for the integrations that have been activated in your Adjust [dashboard].

The session partner parameters have a similar interface to the event partner parameters. Instead of adding the key and its value to an event, it's added through a call to method `adjust_AddSessionPartnerParameter`:

```cpp
adjust_AddSessionPartnerParameter("foo", "bar");
```

The session partner parameters will be merged with the partner parameters added to an event. The partner parameters added to an event have precedence over the session partner parameters. Meaning that, when adding a partner parameter to an event with the same key to one added from the session, the value that prevails is the partner parameter added to the event.

It's possible to remove a specific session partner parameter by passing the desiring key to the method `adjust_RemoveSessionPartnerParameter`.

```cpp
adjust_RemoveSessionPartnerParameter("foo");
```

If you wish to remove all keys and values from the session partner parameters, you can reset it with the method `adjust_ResetSessionPartnerParameters`.

```cpp
adjust_ResetSessionPartnerParameters();
```

### <a id="delay-start">Delay start

Delaying the start of the Adjust SDK allows your app some time to obtain session parameters, such as unique identifiers, to be sent on install.

Set the initial delay time in seconds with the `set_delay_start` field of the `adjust_config` instance:

```cpp
config->set_delay_start(5.5);
```

In this case this will make the Adjust SDK not send the initial install session and any event created for 5.5 seconds. After this time is expired or if you call `adjust_SendFirstPackages()` in the meanwhile, every session parameter will be added to the delayed install session and events and the Adjust SDK will resume as usual.

**The maximum delay start time of the Adjust SDK is 10 seconds**.

### <a id="attribution-callback">Attribution callback

Adjust can also send you a callback upon change of attribution. Due to the different sources considered for attribution, this information cannot be provided synchronously. Follow these steps to implement the optional callback in your application:

1. Create void method which receives parameter of type `adjust_attribution_data*`.

2. After creating instance of `adjust_config`, call its `set_attribution_callback` method
with the previously created method as parameter.

The callback function will get called when the SDK receives final attribution data. 
Within the callback function you have access to the `attribution` parameter. 
Here is a quick summary of its properties:

- `char* tracker_token` the tracker token of the current attribution.
- `char* tracker_name` the tracker name of the current attribution.
- `char* network` the network grouping level of the current attribution.
- `char* campaign` the campaign grouping level of the current attribution.
- `char* ad_group` the ad group grouping level of the current attribution.
- `char* creative` the creative grouping level of the current attribution.
- `char* click_label` the click label of the current attribution.
- `char* adid` the Adjust device identifier.

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_attribution_data(adjust_attribution_data* attribution) {
    // Printing all attribution properties.
    IwTrace(ADJUSTMARMALADE, ("Attribution changed!"));
    IwTrace(ADJUSTMARMALADE, (attribution->tracker_token));
    IwTrace(ADJUSTMARMALADE, (attribution->tracker_name));
    IwTrace(ADJUSTMARMALADE, (attribution->network));
    IwTrace(ADJUSTMARMALADE, (attribution->campaign));
    IwTrace(ADJUSTMARMALADE, (attribution->ad_group));
    IwTrace(ADJUSTMARMALADE, (attribution->creative));
    IwTrace(ADJUSTMARMALADE, (attribution->click_label));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_attribution_callback(trace_attribution_data);
    
    adjust_Start(config);

    // ...
}
```

Please make sure to consider [applicable attribution data policies.][attribution-data]

### <a id="session-event-callbacks">Session and event callbacks

You can register a callback to be notified of successful and failed tracked events and/or sessions.

Follow the same steps to implement the following callback function for successfully tracked events:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_event_success_data(adjust_event_success_data* response) {
    IwTrace(ADJUSTMARMALADE, ("Event successfully tracked!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->event_token));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_event_success_callback(trace_event_success_data);
    
    adjust_Start(config);

    // ...
}
```

The following callback function for failed tracked events:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_event_failure_data(adjust_event_failure_data* response) {
    IwTrace(ADJUSTMARMALADE, ("Event tracking failed!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->event_token));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->will_retry));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_event_failure_callback(trace_event_failure_data);
    
    adjust_Start(config);

    // ...
}
```

For successfully tracked sessions:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_session_success_data(adjust_session_success_data* response) {
    IwTrace(ADJUSTMARMALADE, ("Session successfully tracked!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_session_success_callback(trace_session_success_data);
    
    adjust_Start(config);

    // ...
}
```

And for failed tracked sessions:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_session_failure_data(adjust_session_failure_data* response) {
    IwTrace(ADJUSTMARMALADE, ("Session tracking failed!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->will_retry));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_session_failure_callback(trace_session_failure_data);
    
    adjust_Start(config);

    // ...
}
```

The callback functions will be called after the SDK tries to send a package to the server. Within the callback you have access to a response data object specifically for the callback. Here is a quick summary of the session response data properties:

- `const char* message` the message from the server or the error logged by the SDK.
- `const char* timestamp` timestamp from the server.
- `const char* adid` a unique device identifier provided by Adjust.
- `const char* jsonResponse` the JSON object with the response from the server.

Both event response data objects contain:

- `const char* eventToken` the event token, if the package tracked was an event.

And both event and session failed objects also contain:

- `const char* willRetry` indicates there will be an attempt to resend the package at a later time.

### <a id="disable-tracking">Disable tracking

You can disable the Adjust SDK from tracking by invoking the method `adjust_SetEnabled` with the enabled parameter as `false`. This setting is **remembered between sessions**, but it can only be activated after the first session.

```cpp
adjust_SetEnabled(false);
```

You can verify if the Adjust SDK is currently active with the method `adjust_IsEnabled`. It is always possible to activate the Adjust SDK by invoking `adjust_SetEnabled` with the parameter set to `true`.

### <a id="offline-mode">Offline mode

You can put the Adjust SDK in offline mode to suspend transmission to our servers while retaining tracked data to be sent later. When in offline mode, all information is saved in a file, so be careful not to trigger too many events while in offline mode.

You can activate offline mode by calling `adjust_SetOfflineMode` with the parameter `true`.

```cpp
adjust_SetOfflineMode(true);
```

Conversely, you can deactivate offline mode by calling `adjust_SetOfflineMode` with `false`. When the Adjust SDK is put back in online mode, all saved information is send to our servers with the correct time information.

Unlike disabling tracking, **this setting is not remembered** between sessions. This means that the SDK is in online mode whenever it is started, even if the app was terminated in offline mode.

### <a id="event-buffering">Event buffering

If your app makes heavy use of event tracking, you might want to delay some HTTP requests in order to send them in one batch every minute. You can enable event buffering with your `adjust_config` instance by calling `set_event_buffering_enabled` method:

```cpp
// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_event_buffering_enabled(true);
    
    adjust_Start(config);

    // ...
}
```

If nothing set, event buffering is **disabled by default**.

### <a id="background-tracking">Background tracking

The default behaviour of the Adjust SDK is to **pause sending HTTP requests while the app is in the background**. You can change this in your `adjust_config` instance by calling `set_is_sending_in_background_enabled` method:

```cpp
// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_is_sending_in_background_enabled(true);
    
    adjust_Start(config);

    // ...
}
```

If nothing is set, sending in background is **disabled by default**.

### <a id="device-ids">Device IDs

Certain services (such as Google Analytics) require you to coordinate Device and Client IDs in order to prevent duplicate reporting.

### <a id="di-idfa">iOS advertising identifier

You can access the IDFA value of an iOS device by setting the appropriate callback method on the `adjust_config` object and invoking the `adjust_GetIdfa` method:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_idfa_data(const char* response) {
    IwTrace(ADJUSTMARMALADE, ("IDFA received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_idfa_callback(trace_idfa_data);
    
    adjust_Start(config);

    // ...
    
    adjust_GetIdfa();
    
    // ...
}
```

### <a id="di-gps-adid">Google Play Services advertising identifier

The Adjust SDK provides you with the possibility to read the Google advertising identifier of the Android device on which your app is running. In order to do this, set the callback method on the `adjust_config` object which receives the `const char*` parameter. After setting this, if you invoke the `adjust_GetGoogleAdId` method, you will receive the Google advertising identifier value in your callback method:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_google_ad_id_data(const char* response) {
    IwTrace(ADJUSTMARMALADE, ("Google Advertising Identifier received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

// ...

int main() {
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_google_ad_id_callback(trace_google_ad_id_data);
    
    adjust_Start(config);

    // ...
    
    adjust_GetGoogleAdId();
    
    // ...
}
```

### <a id="di-adid"></a>Adjust device identifier

For every device with your app installed on it, the Adjust backend generates a unique **Adjust device identifier** (**adid**). In order to obtain this identifier, call the `adjust_GetAdid` method:

```cpp
char* adid;

adjust_GetAdid(&adid);

IwTrace(ADJUSTMARMALADE, (adid));
```

**Note**: Information about the **adid** is only available after an app installation has been tracked by the Adjust backend. From that moment on, the Adjust SDK has information about the device **adid** and you can access it with this method. So, **it is not possible** to access the **adid** value before the SDK has been initialised and installation of your app has been successfully tracked.

### <a id="user-attribution"></a>User attribution

As described in the [attribution callback section](#attribution-callback), this callback is triggered, providing you with information about a new attribution whenever it changes. If you want to access information about a user's current attribution whenever you need it, you can make a call to the `adjust_GetAttribution` method:

```cpp
adjust_attribution_data* attribution = new adjust_attribution_data();
adjust_GetAttribution(attribution);

IwTrace(ADJUSTMARMALADE, (attribution->tracker_token));
IwTrace(ADJUSTMARMALADE, (attribution->tracker_name));
IwTrace(ADJUSTMARMALADE, (attribution->network));
IwTrace(ADJUSTMARMALADE, (attribution->campaign));
IwTrace(ADJUSTMARMALADE, (attribution->ad_group));
IwTrace(ADJUSTMARMALADE, (attribution->creative));
IwTrace(ADJUSTMARMALADE, (attribution->click_label));
IwTrace(ADJUSTMARMALADE, (attribution->adid));
```

**Note**: Information about current attribution is only available after an app installation has been tracked by the Adjust backend and the attribution callback has been triggered. From that moment on, the Adjust SDK has information about a user's attribution and you can access it with this method. So, **it is not possible** to access a user's attribution value before the SDK has been initialised and an attribution callback has been triggered.

### <a id="push-token">Push token

To send us the push notifications token, then add the following call to Adjust **whenever you get your token in 
the app or when it gets updated**:

```cpp
adjust_SetDeviceToken("YourPushNotificationToken");
```

### <a id="pre-installed-trackers">Pre-installed trackers

If you want to use the Adjust SDK to recognize users that found your app pre-installed on their device, follow these steps.

1. Create a new tracker in your [dashboard].
2. Open your app delegate and add set the default tracker of your `adjust_config` instance:

    ```cpp
    adjust_config* config = new adjust_config(app_token, environment);

    config->set_default_tracker("{TrackerToken}");
    
    adjust_Start(config);
    ```

  Replace `{TrackerToken}` with the tracker token you created in step 2. Please note that the dashboard displays a tracker 
  URL (including `http://app.adjust.com/`). In your source code, you should specify only the six-character token and not the
  entire URL.

3. Build and run your app. You should see a line like the following in the app's log output:

    ```
    Default tracker: 'abc123'
    ```

### <a id="deeplinking">Deep linking

If you are using the Adjust tracker URL with an option to deep link into your app from the URL, there is the possibility to 
get info about the deep link URL and its content. Hitting the URL can happen when the user has your app already installed 
(standard deep linking scenario) or if they don't have the app on their device (deferred deep linking scenario).

### <a id="deeplinking-standard">Standard deep linking scenario

Standard deep linking scenario is a platform specific feature and in order to support it, you need to add some additional 
settings to your app.

In order to get info about the URL content in a standard deep linking scenario, you should set a callback method on the 
`adjust_config` object which will receive one `const char*` parameter where the content of the URL will be delivered. You 
should set this method on the config object by calling the method `set_deeplink_callback`:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_deeplink_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Deeplink received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

// ...

int main()
{
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_deeplink_callback(trace_deeplink_data);
    
    adjust_Start(config);

    // ...
}
```

### <a id="deeplinking-deferred">Deferred deep linking scenario

While deferred deep linking is not supported out of the box on Android and iOS, our Adjust SDK makes it possible.
 
In order to get info about the URL content in a deferred deep linking scenario, you should set a callback method on the 
`adjust_config` object which will receive one `const char*` parameter where the content of the URL will be delivered. You 
should set this method on the config object by calling the method `set_deferred_deeplink_callback`:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_deferred_deeplink_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Deferred deeplink received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

// ...

int main()
{
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_deferred_deeplink_callback(trace_deferred_deeplink_data);
    
    adjust_Start(config);

    // ...
}
```

In deferred deep linking scenario, there is one additional setting which can be set on the `adjust_config` object. Once the 
Adjust SDK gets the deferred deep link info, we are offering you the possibility to choose whether our SDK should open this 
URL or not. You can choose to set this option by calling the `set_should_deferred_deeplink_be_opened` method on the config 
object:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_deferred_deeplink_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Deferred deeplink received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

// ...

int main()
{
    const char* app_token = "{YourAppToken}";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);
    config->set_should_deferred_deeplink_be_opened(false);
    config->set_deferred_deeplink_callback(trace_deferred_deeplink_data);
    
    adjust_Start(config);

    // ...
}
```

If nothing is set, **the Adjust SDK will always try to launch the URL by default**.

To enable your apps to support deep linking, you should set up schemes for each supported platform.

### <a id="deeplinking-android">Deep linking setup for Android

To set a scheme name for your Android app, you should add the following `<intent-filter>` to the activity you want to launch
after deep linking (usually to your default Marmalade Android activity):

```xml
<intent-filter>
    <action android:name="android.intent.action.VIEW" />
    <category android:name="android.intent.category.DEFAULT" />
    <category android:name="android.intent.category.BROWSABLE" />
    <data android:scheme="schemeName" />
</intent-filter>
```

You should replace `schemeName` with your desired scheme name for Android app.

After adding this intent filter to our example app, activity definition in `AndroidManifest.xml` looks like this:

```xml
<activity android:name=".${CLASSNAME}"
          android:label="@string/app_name"
          android:configChanges="locale|keyboardHidden|orientation|screenSize"
          android:launchMode="singleTask"
          ${EXTRA_ACTIVITY_ATTRIBS}>
    <intent-filter>
        <action android:name="android.intent.action.MAIN"/>
        <category android:name="android.intent.category.LAUNCHER"/>
    </intent-filter>
    <intent-filter>
        <action android:name="android.intent.action.VIEW" />
        <category android:name="android.intent.category.DEFAULT" />
        <category android:name="android.intent.category.BROWSABLE" />
        <data android:scheme="adjustExample" />
    </intent-filter>
</activity>
```

### <a id="deeplinking-ios">Deep linking setup for iOS

In iOS, you need to set up the custom URL scheme name, but unlike Android, all that needs to be edited is your app's `.mkb` 
file. You need to add the following lines to the `deployment` part of your app's `.mkb` file:

```
iphone-bundle-url-name = com.your.bundle
iphone-bundle-url-schemes = schemeName
```

You should replace `com.your.bundle` with your app's bundle ID and `schemeName` with your desired scheme name for iOS app.

**Important**: By using this approach for deep linking support in iOS, you will support deep linking handling for devices 
which have **iOS 8 and lower**. Starting from **iOS 9**, Apple has introduced universal links for which, at this moment, 
there's no built in support inside the Marmalade platform. To support this, you would need to edit the natively 
generated iOS project in Xcode (if possible) and add support to handle universal links from there. If you are interested in 
finding out how to do that on the native side, please consult our [native iOS universal links guide][universal-links-guide].

### <a id="deeplinking-reattribution">Reattribution via deep links

Adjust enables you to run re-engagement campaigns by using deep links. For more information on this, please 
check our [official docs][reattribution-with-deeplinks]. 

The Adjust SDK supports this feature out of the box and no additional setup is needed in your app's code.

[dashboard]:   http://adjust.com
[adjust.com]:  http://adjust.com

[releases]:               https://github.com/adjust/marmalade_sdk/releases
[example-app]:            https://github.com/adjust/marmalade_sdk/tree/master/example
[google_ad_id]:           https://developer.android.com/google/play-services/id.html
[callbacks-guide]:        https://docs.adjust.com/en/callbacks
[custom-receiver]:        https://github.com/adjust/android_sdk/blob/master/doc/referrer.md
[special-partners]:       https://docs.adjust.com/en/special-partners
[attribution-data]:       https://github.com/adjust/sdks/blob/master/doc/attribution-data.md
[broadcast-receiver]:     https://github.com/adjust/android_sdk#sdk-broadcast-receiver

[google_play_services]:         http://developer.android.com/google/play-services/setup.html
[universal-links-guide]:        https://github.com/adjust/ios_sdk/#deeplinking-setup-new
[broadcast-receiver-custom]:    https://github.com/adjust/android_sdk/blob/master/doc/english/referrer.md
[reattribution-with-deeplinks]: https://docs.adjust.com/en/deeplinking/#manually-appending-attribution-data-to-a-deep-link

## <a id="license">License

The Adjust SDK is licensed under the MIT License.

Copyright (c) 2012-2017 Adjust GmbH,
http://www.adjust.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
