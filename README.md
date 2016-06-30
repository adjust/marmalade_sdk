## Summary

This is the Marmalade SDK of adjust™. You can read more about adjust™ at adjust.com.

## Example app

There is an example app inside the [`Example` directory][example-app]. You can use the example app to see how the 
adjust SDK can be integrated.

## Basic Installation

These are the minimal steps required to integrate the adjust SDK into your Marmalade project.

**Note**: SDK 4.7.0 for Marmalade is built with **Marmalade 8.3.0p3** and we advise you to use that Marmalade version 
or higher especially if you want to rebuild our Marmalade extension on your own.

### 1. Get the SDK

Download the latest version from our [releases page][releases]. Extract the zip file in a folder of your choice.

### 2. Add the SDK to your project

Add `AdjustMarmalade` as a subproject of your own.  Edit the `.mkb` file of your project and add the following line:

````
subproject path_to_folder/adjust/AdjustMarmalade
```

### 3. Integrate with your app

Add the `AdjustMarmalade.h` header to your project where the `main` function is located. In the `main` function, 
initialize `adjust_config` structure and call the function `adjust_Start`.

```cpp
#include "AdjustMarmalade.h"

int main()
{
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

**Important:** This value should be set to `sandbox` if and only if you or someone else is testing your app. Make sure 
to set the environment to `production` just before you publish the app. Set it back to `sandbox` when you start developing
and testing it again.

We use this environment to distinguish between real traffic and test traffic from test devices. It is very important that
you keep this value meaningful at all times! This is especially important if you are tracking revenue.

#### Adjust Logging

You can increase or decrease the amount of logs you see in tests by calling `set_log_level` on your `adjust_config` 
instance with one of the following parameters:

```cpp
config->set_log_level("verbose"); // enable all logging
config->set_log_level("debug");   // enable more logging
config->set_log_level("info");    // the default
config->set_log_level("warn");    // disable info logging
config->set_log_level("error");   // disable warnings as well
config->set_log_level("assert");  // disable errors as well
```

### 4. Adjust Android manifest

In order to use your Marmalade app for Android with our SDK, certain changes are needed in `AndroidManifest.xml` file of 
your app.

Automatically, the adjust SDK will add it's own broadcast receiver for the Android `INSTALL_REFERRER` intent and permissions needed for the SDK.

Please, have following in mind:

- In case that you are using your own custom broadcast receiver for the `INSTALL_REFERRER` intent, please follow our 
[guide][custom-receiver] in order to add needed calls to the adjust SDK. In this case, you can go to `AdjustMarmalade.mkf`
file and remove line which automatically adds our custom broadcast receiver to your app's manifest file:

    ```xml
    android-extra-application-manifest="adjust_broadcast_receiver.xml"
    ```
    
- By default, the adjust SDK extension is going to add the `INTERNET` and `ACCESS_WIFI_STATE` permissions to your app's
manifest file. `ACCESS_WIFI_STATE` permission is only needed if you *are not using Google Play Services* in your app. If
this is the case, feel free to remove this permission unless you need it for something else.

### 5. Add Google Play Services

Since 1 August 2014, all apps in the Google Play Store must use the [Google Advertising ID][google_ad_id] to uniquely 
identify devices. To allow the adjust SDK to use the Google Advertising ID, you must integrate the 
[Google Play Services][google_play_services].

In order to add Google Play Services to your Marmalade app, you should edit your app's `.mkb` file and add 
`s3eGooglePlayServices` in `subprojects` list. In addition to this, you should add following line to `deployment` list of 
your `.mkb` file:

```
android-extra-strings='(gps_app_id,your.app.package)'
```

After this, Google Play Services will be successfully integrated in your Marmalade app.

## Additional features

You can take advantage of the following features once the adjust SDK is integrated into your project.

### 6. Add tracking of custom events

You can tell adjust about every event you want. Suppose you want to track every tap on a button. Simply create a new event
token in your [dashboard]. Let's say that event token is `abc123`. You can add the following line in your button’s click
handler method to track the click:

```cpp
adjust_event* event = new adjust_event("abc123");
adjust_TrackEvent(event);
```

### 7. Add revenue tracking

If your users can generate revenue by tapping on advertisements or making In-App Purchases, then you can track those
revenues with events. Let's say a tap is worth €0.01. You could track the revenue event like this:

```cpp
adjust_event* event = new adjust_event("abc123");
event->set_revenue(0.01, "EUR");

adjust_TrackEvent(event);
```

#### iOS

##### <a id="deduplication"></a>Revenue deduplication

You can also add an optional transaction ID to avoid tracking duplicate revenues. The last ten transaction IDs are
remembered, and revenue events with duplicate transaction IDs are skipped. This is especially useful for In-App Purchase
tracking. You can see an example below.

If you want to track in-app purchases, please make sure to call the `adjust_TrackEvent` only if the transaction is finished
and item is purchased. That way you can avoid tracking revenue that is not actually being generated.

```cpp
adjust_event* event = new adjust_event("abc123");

event->set_revenue(0.01, "EUR");
event->set_transaction_id("transaction_id");

adjust_TrackEvent(event);
```

##### Receipt verification

If you want to check the validity of In-App Purchases made in your app by using the adjust Server Side Receipt Verification
mechanism, stay tuned because the adjust purchase SDK for Marmalade will be released soon.

### 8. Add callback parameters

You can also register a callback URL for that event in your [dashboard][dashboard] and we will send a GET request to that
URL whenever the event gets tracked. In that case you can also put some key-value-pairs in an object and pass it to the 
`adjust_TrackEvent` method. We will then append these named parameters to your callback URL.

For example, suppose you have registered the URL `http://www.adjust.com/callback` for your event with event token `abc123`
and execute the following lines:

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

It should be mentioned that we support a variety of placeholders like `{idfa}` for iOS or `{android_id}` for Android that
can be used as parameter values.  In the resulting callback the `{idfa}` placeholder would be replaced with the ID for
Advertisers of the current device for iOS and the `{android_id}` would be replaced with the AndroidID of the current device
for Android. Also note that we don't store any of your custom parameters, but only append them to your callbacks. If you 
haven't registered a callback for an event, these parameters won't even be read.

### 9. Partner parameters

You can also add parameters for integrations that have been activated in your adjust dashboard that are transmittable to
network partners.

This works similarly to the callback parameters mentioned above, but can be added by calling the `add_partner_parameter`
method on your `adjust_event` instance.

```cpp
adjust_event* event = new adjust_event("abc123");

event->add_partner_parameter("key", "value");
event->add_partner_parameter("foo", "bar");

adjust_TrackEvent(event);
```

You can read more about special partners and these integrations in our [guide to special partners.][special-partners]

### 10. Receive attribution change callback

You can register a callback to be notified of tracker attribution changes. Due to the different sources considered for
attribution, this information cannot be provided synchronously. Follow these steps to implement the optional callback in
your application:

1. Create void method which receives parameter of type `adjust_attribution_data*`.

2. After creating instance of `adjust_config`, call its `set_attribution_callback` method
with the previously created method as parameter.

The callback function will get called when the SDK receives final attribution data. 
Within the callback function you have access to the `attribution` parameter. 
Here is a quick summary of its properties:

- `char* tracker_token` the tracker token of the current install.
- `char* tracker_name` the tracker name of the current install.
- `char* network` the network grouping level of the current install.
- `char* campaign` the campaign grouping level of the current install.
- `char* ad_group` the ad group grouping level of the current install.
- `char* creative` the creative grouping level of the current install.
- `char* click_label` the click label of the current install.

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_attribution_data(adjust_attribution_data* attribution) 
{
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

int main()
{
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

### 11. Implement callbacks for tracked events and sessions

You can register a callback to be notified of successful and failed tracked events and/or sessions.

Follow the same steps to implement the following callback function for successfully tracked events:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_event_success_data(adjust_event_success_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Event successfully tracked!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->event_token));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main()
{
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

static void trace_event_failure_data(adjust_event_failure_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Event tracking failed!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->event_token));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->will_retry));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main()
{
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

static void trace_session_success_data(adjust_session_success_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Session successfully tracked!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main()
{
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

static void trace_session_failure_data(adjust_session_failure_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Session tracking failed!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->will_retry));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

// ...

int main()
{
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

The callback functions will be called after the SDK tries to send a package to the server. Within the callback you have
access to a response data object specifically for the callback. Here is a quick summary of the session response data
properties:

- `const char* message` the message from the server or the error logged by the SDK.
- `const char* timestamp` timestamp from the server.
- `const char* adid` a unique device identifier provided by adjust.
- `const char* jsonResponse` the JSON object with the response from the server.

Both event response data objects contain:

- `const char* eventToken` the event token, if the package tracked was an event.

And both event and session failed objects also contain:

- `const char* willRetry` indicates there will be an attempt to resend the package at a later time.

### 12. Disable tracking

You can disable the adjust SDK from tracking by invoking the method `adjust_SetEnabled` with the enabled parameter as
`false`. This setting is **remembered between sessions**, but it can only be activated after the first session.

```cpp
adjust_SetEnabled(false);
```

You can verify if the adjust SDK is currently active with the method `adjust_IsEnabled`. It is always possible to activate
the adjust SDK by invoking `adjust_SetEnabled` with the parameter set to `true`.

### 13. Offline mode

You can put the adjust SDK in offline mode to suspend transmission to our servers, while retaining tracked data to be sent
later. While in offline mode, all information is saved in a file, so be careful not to trigger too many events while in
offline mode.

You can activate offline mode by calling `adjust_SetOfflineMode` with the parameter `true`.

```cpp
adjust_SetOfflineMode(true);
```

Conversely, you can deactivate offline mode by calling `adjust_SetOfflineMode` with `false`. When the adjust SDK is put back
in online mode, all saved information is send to our servers with the correct time information.

Unlike disabling tracking, this setting is *not remembered* bettween sessions. This means that the SDK is in online mode
whenever it is started, even if the app was terminated in offline mode.

### 14. Device IDs

Certain services (such as Google Analytics) require you to coordinate Device and Client IDs in order to prevent duplicate
reporting. 

#### Android

The adjust SDK provides you with possibility to read Google Advertising Identifier of the Android device which is running
your app. In order to do that, you can set callback method on `adjust_config` object which receives `const char*` parameter.
After setting this, if you invoke the method `adjust_GetGoogleAdId`, you will get the Google Advertising Identifier value in
your callback method:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_google_ad_id_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Google Advertising Identifier received!"));
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
    config->set_google_ad_id_callback(trace_google_ad_id_data);
    
    adjust_Start(config);

    // ...
    
    adjust_GetGoogleAdId();
    
    // ...
}
```

#### iOS

Similarly like for Google Advertising Identifier on Android device, you can access to IDFA value on iOS device by setting 
the appropriate callback method on `adjust_config` object and by invoking the `adjust_GetIdfa` method:

```cpp
#include "AdjustMarmalade.h"

//...

static void trace_idfa_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("IDFA received!"));
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
    config->set_idfa_callback(trace_idfa_data);
    
    adjust_Start(config);

    // ...
    
    adjust_GetIdfa();
    
    // ...
}
```

### 15. Deeplinking



[adjust.com]: http://adjust.com
[dashboard]: http://adjust.com
[example-app]: https://github.com/adjust/marmalade_sdk/tree/master/Example
[releases]: https://github.com/adjust/marmalade_sdk/releases
[custom-receiver]: https://github.com/adjust/android_sdk/blob/master/doc/referrer.md
[attribution-data]: https://github.com/adjust/sdks/blob/master/doc/attribution-data.md
[google_play_services]: http://developer.android.com/google/play-services/setup.html
[google_ad_id]: https://developer.android.com/google/play-services/id.html
[xcode-logs]: https://raw.githubusercontent.com/adjust/sdks/master/Resources/marmalade/v4/xcode-logs.png
[android-pkg-name]: https://raw.githubusercontent.com/adjust/sdks/master/Resources/marmalade/v4/android-pkg-name.png
[special-partners]: https://docs.adjust.com/en/special-partners

## License

The adjust SDK is licensed under the MIT License.

Copyright (c) 2012-2016 adjust GmbH,
http://www.adjust.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
