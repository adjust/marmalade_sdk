## Summary

This is the Marmalade SDK of adjust™. You can read more about adjust™ at
adjust.com.

## Example app

There is an example app inside the [`HelloAdjust` directory][example-app]. 
You can try an example app to see how the adjust SDK can be integrated.

## Basic Installation

These are the minimal steps required to integrate the adjust SDK into your
Marmalade project.

### 1. Get the SDK

Download the latest version from our [releases page][releases]. Extract the
zip file in a folder of your choice.

### 2. Add the SDK to your project

Add `AdjustMarmalade` as a subproject of your own.  Edit the `.mkb` file of your project 
and add the following line:

````
subproject path_to_folder/adjust/AdjustMarmalade
```

### 3. Integrate with your app

Add the `AdjustMarmalade.h` header to your project where the `main` function is located. 
In the `main` function, initialize `adjust_config` structure and call the function `adjust_Start`.

```cpp
#include "AdjustMarmalade.h"

int main()
{
    const char* app_token = "YourAppToken";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);

    adjust_Start(config);

    // ...
}
```

Replace `YourAppToken` with your app token. You can find in your [dashboard].

Depending on whether you build your app for testing or for production, you must 
set `environment` with one of these values:

```cpp
const char* environment = "sandbox";
const char* environment = "production";
```

**Important:** This value should be set to `sandbox` if and only if you or someone 
else is testing your app. Make sure to set the environment to `production` just before 
you publish the app. Set it back to `sandbox` when you start developing and testing it again.

We use this environment to distinguish between real traffic and test traffic from 
test devices. It is very important that you keep this value meaningful at all times! 
This is especially important if you are tracking revenue.

#### Adjust Logging

You can increase or decrease the amount of logs you see in tests by calling 
`set_log_level` on your `adjust_config` instance with one of the following parameters:

```cpp
config->set_log_level("verbose"); // enable all logging
config->set_log_level("debug");   // enable more logging
config->set_log_level("info");    // the default
config->set_log_level("warn");    // disable info logging
config->set_log_level("error");   // disable warnings as well
config->set_log_level("assert");  // disable errors as well
```

### 4. Adjust Android manifest

In order to use your Marmalade app for Android with our SDK, you must edit the Android 
manifest file.

You can find the needed [permissions][android-permissions] and how to add 
[broadcast receiver][brodcast-android] in our Android guide.

### 5. Add Google Play Services

Since 1st August 2014, all apps in the Google Play Store must use the [Google Advertising ID][google_ad_id] 
to uniquely identify devices. To allow the  adjust SDK to use the Google Advertising ID, 
you must integrate the [Google Play Services][google_play_services].

In order to add Google Play Services to your Marmalade app, you should edit your app's
`.mkb` file and add `s3eGooglePlayServices` in `subprojects` list. In addition to this,
you should add following line to `deployment` list of your `.mkb` file:

```
android-extra-strings='(gps_app_id,your.app.package)'
```

After this, Google Play Services will be successfully integrated in your Marmalade app.

## Debugging on device

### Android device

Use ```logcat``` tool that comes with Android SDK:

```
<path-to-android-sdk>/platform-tools/adb logcat -s "Adjust"
```

### iOS device

Check the Console at XCode's Device Organizer to access Adjust logs:

![][xcode-logs]

## Additional features

You can take advantage of the following features once the adjust SDK is integrated 
into your project.

### 6. Add tracking of custom events

You can tell adjust about every event you want. Suppose you want to track every tap 
on a button. Simply create a new event token in your [dashboard]. Let's say that event 
token is `abc123`. You can add the following line in your button’s click handler method 
to track the click:

```cpp
adjust_event* event = new adjust_event("abc123");
adjust_TrackEvent(event);
```

### 7. Add tracking of revenue

If your users can generate revenue by tapping on advertisements or making in-app 
purchases, then you can track those revenues with events. Lets say a tap is worth 
€0.01. You could track the revenue event like this:

```cpp
adjust_event* event = new adjust_event("abc123");
event->set_revenue(0.01, "EUR");

adjust_TrackEvent(event);
```

#### iOS

##### <a id="deduplication"></a> Revenue deduplication

You can also add an optional transaction ID to avoid tracking duplicate revenues. 
The last ten transaction IDs are remembered, and revenue events with duplicate 
transaction IDs are skipped. This is especially useful for in-app purchase tracking. 
See an example below.

If you want to track in-app purchases, please make sure to call `adjust_TrackEvent` 
only if the transaction is finished and item is purchased. That way you can avoid 
tracking revenue that is not actually being generated.

```cpp
adjust_event* event = new adjust_event("abc123");

event->set_revenue(0.01, "EUR");
event->set_transaction_id("transaction_id");

adjust_TrackEvent(event);
```

##### Receipt verification

If you track in-app purchases, you can also attach the receipt to the tracked event. 
Our servers will verify that receipt with Apple and discard the event if verification 
fails. To make this work, you will also need to send us the transaction ID of the purchase. 
The transaction ID will additionally be used for SDK side deduplication as explained [above](#deduplication):

```cpp
adjust_event* event = new adjust_event("abc123");

event->set_revenue(0.01, "EUR");
event->set_receipt("receipt", "transaction_id");

adjust_TrackEvent(event);
```

### 8. Add callback parameters

You can also register a callback URL for that event in your [dashboard] and we will 
send a GET request to that URL whenever the event gets tracked. In that case you can 
also put some key-value-pairs in an object and pass it to the `adjust_TrackEvent` method. 
We will then append these named parameters to your callback URL.

For example, suppose you have registered the URL `http://www.adjust.com/callback` for 
your event with event token `abc123` and execute the following lines:

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

It should be mentioned that we support a variety of placeholders like `{idfa}` for iOS or 
`{android_id}` for Android that can be used as parameter values.  In the resulting callback 
the `{idfa}` placeholder would be replaced with the ID for Advertisers of the current device 
for iOS and the `{android_id}` would be replaced with the AndroidID of the current device for 
Android. Also note that we don't store any of your custom parameters, but only append them to
your callbacks. If you haven't registered a callback for an event, these parameters won't even 
be read.

### 9. Partner parameters

You can also add parameters for integrations that have been activated in your adjust dashboard 
that are transmittable to network partners.

This works similarly to the callback parameters mentioned above, but can be added by calling the 
`add_partner_parameter` method on your `adjust_event` instance.

```cpp
adjust_event* event = new adjust_event("abc123");

event->add_partner_parameter("key", "value");
event->add_partner_parameter("foo", "bar");

adjust_TrackEvent(event);
```

You can read more about special partners and these integrations in our 
[guide to special partners.][special-partners]

### 10. Receive attribution change callback

You can register a callback to be notified of tracker attribution changes. Due to the 
different sources considered for attribution, this information can not by provided 
synchronously. Follow these steps to implement the optional callback in your application:

1. Create void method which receives parameter of type `adjust_attribution_data*`.

2. After creating instance of `adjust_config`, call the `adjust_SetAttributionCallback`
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
    const char* app_token = "YourAppToken";
    const char* environment = "sandbox";
    const char* log_level = "verbose";

    adjust_config* config = new adjust_config(app_token, environment);
    config->set_log_level(log_level);

    adjust_SetAttributionCallback(trace_attribution_data);
    adjust_Start(config);

    // ...
}
```

Please make sure to consider [applicable attribution data policies.][attribution-data]

### 11. Set up deep link reattributions

You can set up the adjust SDK to handle deep links that are used to open your app. 
We will only read certain, adjust-specific parameters. This is essential if you are 
planning to run retargeting or re-engagement campaigns with deep links. The only thing 
you need to do is to properly set your app schema name for iOS and Android platform. By 
using this scheme name later for deep linking, our SDK will handle deep linking automatically 
without need to set anything in your app source code.

#### iOS

In order to set scheme name for your iOS app, you should add the following lines to `deployment`
list of your app's `.mkb` file:

```
iphone-bundle-url-name = your.app.package
iphone-bundle-url-schemes = desired-scheme-name
```

#### Android

To set a scheme name for your Android app, you should add the following `<intent-filter>` to the 
activity you want to launch after deep linking:

```xml
<!-- ... -->
    <activity>
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.VIEW" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.BROWSABLE" />
                <data android:scheme="desired-scheme-name" />
        </intent-filter>
    </activity>
<!-- ... -->
```

For each activity that accepts deep links, find the `onCreate` or `onNewIntent` method and add 
the following call to adjust:

###### For activities with `standard` launch mode

```java
@Override
protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    Intent intent = getIntent();
    Uri data = intent.getData();
    Adjust.appWillOpenUrl(data);
    // ...
}
```

###### For activities with `singleTop` or `singleTask` launch mode

```java
@Override
protected void onNewIntent(Intent intent) {
    super.onNewIntent(intent);

    Uri data = intent.getData();
    Adjust.appWillOpenUrl(data);
    // ...
}
```

### 12. Event buffering

If your app makes heavy use of event tracking, you might want to delay some HTTP requests in 
order to send them in a single batch every minute. You can enable event buffering by calling 
the `config.set_event_buffering_enabled` method with parameter `true`.

```cpp
config.set_event_buffering_enabled(true);
```

### 13. Disable tracking

You can disable the adjust SDK from tracking by invoking the method `adjust_SetEnabled` with the 
enabled parameter as `false`. This setting is remembered between sessions, but it can only be 
activated after the first session.

```cpp
adjust_SetEnabled(false);
```

You can verify if the adjust SDK is currently active with the method `adjust_IsEnabled`. It is 
always  possible to activate the adjust SDK by invoking `adjust_SetEnabled` with the `enabled` 
parameter set to `true`.

### 14. Offline mode

You can put the adjust SDK in offline mode to suspend transmission to our servers, while still 
retaining tracked data to be sent later. While in offline mode, all information is saved in a 
file, so be careful not to trigger too many events while in offline mode.

You can activate offline mode by calling `adjust_SetOfflineMode` with the parameter `true`.

```actionscript
adjust_SetOfflineMode(true);
```

Conversely, you can deactivate offline mode by calling `adjust_SetOfflineMode` with `false`. 
When the adjust SDK is put back in online mode, all saved information is sent to our servers 
with the correct timstamps.

Unlike disabling tracking, this setting is *not remembered* between sessions. This means that 
the SDK is in online mode whenever it is started, even if the app was terminated in offline mode.

[adjust.com]: http://adjust.com
[dashboard]: http://adjust.com
[example-app]: https://github.com/adjust/marmalade_sdk/tree/master/HelloAdjust
[releases]: https://github.com/adjust/marmalade_sdk/releases
[android-permissions]: https://github.com/adjust/android_sdk#5-add-permissions
[brodcast-android]: https://github.com/adjust/android_sdk#6-add-broadcast-receiver
[attribution-data]: https://github.com/adjust/sdks/blob/master/doc/attribution-data.md
[google_play_services]: http://developer.android.com/google/play-services/setup.html
[google_ad_id]: https://developer.android.com/google/play-services/id.html
[xcode-logs]: https://raw.githubusercontent.com/adjust/sdks/master/Resources/marmalade/v4/xcode-logs.png
[special-partners]: https://docs.adjust.com/en/special-partners

## License

The adjust SDK is licensed under the MIT License.

Copyright (c) 2012-2015 adjust GmbH,
http://www.adjust.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
