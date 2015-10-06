## Summary

This is the Marmalade SDK of adjust™. You can read more about adjust™ at
adjust.com.

## Basic Installation

These are the minimal steps required to integrate the adjust SDK into your
Marmalade project.

### 1. Get the SDK

Download the latest version from our [releases page][releases]. Extract the
zip file in a folder of your choice.

### 2. Add the SDK to your project

Add `AdjustMarmalade` as a subproject of your own.  Edit the `.mkb` file of your project and add the following line:

````
subproject path_to_folder/adjust/AdjustMarmalade
```

### 3. Integrate with your app

Add the `AdjustMarmalade.h` header to your project where the `main` function is located. In the `main` function, call the function `adjust_AppDidLaunch`.

```cpp

#include "AdjustMarmalade.h"

int main()
{
  const char* appToken = "{YourAppToken}";
  const char* environment = "sandbox";
  const char* logLevel = "info";
  bool eventBuffering = false;

  adjust_AppDidLaunch(appToken, environment, logLevel, eventBuffering);

  // ...
}
```

Replace `{YourAppToken}` with your App Token. You can find in your [dashboard].

You can increase or decrease the amount of logs you see by changing the value
of `Log Level` to one of the following:

- `verbose` - enable all logging
- `debug` - enable more logging
- `info` - the default
- `warn` - disable info logging
- `error` - disable warnings as well
- `assert` - disable errors as well

Depending on whether or not you build your app for testing or for production
you must change the `environment` variable with one of these values:

```
"sandbox"
"production"
```

**Important:** This value should be set to `sandbox` if and only if you or
someone else is testing your app. Make sure to set the environment to
`production` just before you publish the app. Set it back to `sandbox` when you
start testing it again.

We use this environment to distinguish between real traffic and artificial
traffic from test devices. It is very important that you keep this value
meaningful at all times! Especially if you are tracking revenue.

If your app makes heavy use of event tracking, you might want to delay some
HTTP requests in order to send them in one batch every minute. You can enable
event buffering by changing the `eventBuffering` to `true`.

## Additional features

Once you integrated the adjust SDK into your project, you can take advantage
of the following features.

### 4. Add tracking of custom events.

You can tell adjust about every event you want. Suppose you want to track
every tap on a button. You would have to create a new Event Token in your
[dashboard]. Let's say that Event Token is `abc123`. In your button's
click handler function you could then add the following line to track the click:

```cpp
adjust_TrackEvent("abc123", NULL);
```

You can also register a callback URL for that event in your [dashboard] and we
will send a GET request to that URL whenever the event gets tracked. In that
case you can also put some key-value-pairs in an object and pass it to the
`adjust_TrackEvent` function. We will then append these named parameters to your
callback URL.

The key-value-pairs object is a vector of c-string pairs defined as followed and included in the header `AdjustMarmalde.h`

```cpp
#include <vector>
typedef std::vector<std::pair<const char*, const char*> > adjust_param_type;
```

For example, suppose you have registered the URL
`http://www.adjust.com/callback` for your event with Event Token `abc123` and
execute the following lines:

```cpp
adjust_param_type *params = new adjust_param_type();
params->push_back(std::make_pair("foo", "bar"));
params->push_back(std::make_pair("key", "value"));

adjust_TrackEvent("abc123", params);
```

In that case we would track the event and send a request to:

```
http://www.adjust.com/callback?key=value&foo=bar
```

It should be mentioned that we support a variety of placeholders like `{idfa}`
for iOS or `{android_id}` for Android that can be used as parameter values.  In
the resulting callback the `{idfa}` placeholder would be replaced with the ID
for Advertisers of the current device for iOS and the `{android_id}` would be
replaced with the AndroidID of the current device for Android. Also note that
we don't store any of your custom parameters, but only append them to your
callbacks.  If you haven't registered a callback for an event, these parameters
won't even be read.

### 6. Add tracking of revenue

If your users can generate revenue by clicking on advertisements or making
in-app purchases you can track those revenues. If, for example, a click is
worth one cent, you could make the following call to track that revenue:

```cpp
adjust_TrackRevenue(1.0, NULL, NULL);
```

The parameter is supposed to be in cents and will get rounded to one decimal
point. If you want to differentiate between different kinds of revenue you can
get different Event Tokens for each kind. Again, you need to create those Event
Tokens in your [dashboard]. In that case you would make a call like this:

```cpp
adjust_TrackRevenue(1.0, "abc123", NULL);
```

Again, you can register a callback and provide a dictionary of named
parameters, just like it worked with normal events.

```cpp
adjust_param_type *params = new adjust_param_type();
params->push_back(std::make_pair("foo", "bar"));
params->push_back(std::make_pair("key", "value"));

adjust_TrackRevenue(1.0, "abc123", params);
```

### 7. Receive delegate callbacks

Every time your app tries to track a session, an event or some revenue, you can
be notified about the success of that operation and receive additional
information about the current install. Follow these steps to implement a
delegate to this event.

Please make sure to consider [applicable attribution data policies.][attribution-data]

1. Create a function with the signature of `adjust_response_data_delegate`. The following definitions are included in the header `AdjustMarmalade.h`:

  ```cpp
  struct adjust_response_data {
    char* activityKind;
    bool success;
    bool willRetry;
    char* error;
    char* trackerToken;
    char* trackerName;
    char* network;
    char* campaign;
    char* adgroup;
    char* creative;
  }; 

  typedef void (*adjust_response_data_delegate)(adjust_response_data*);

  ```

2. After calling `adjust_AppDidLaunch`, call the `adjust_SetResponseDelegate` function
with the previously created function pointer.

The delegate function will get called every time any activity was tracked or
failed to track. Within the delegate function you have access to the
`adjust_response_data*` parameter. Take notice that all the memory allocated by the `adjust_response_data` object is freed at the end of the delegate function. If you wish to use any of the c-strings make sure to copy them. Here is a quick summary of its attributes:

- `char* activityKind` indicates what kind of activity was tracked. It has
one of these values:

	```
	session
	event
	revenue
	reattribution
	```

- `bool success` indicates whether or not the tracking attempt was
  successful.
- `bool willRetry` is true when the request failed, but will be retried.
- `char* error` an error message when the activity failed to track or
  the response could not be parsed. Is `NULL` otherwise.
- `char* trackerToken` the tracker token of the current install. Is `NULL` if
  request failed or response could not be parsed.
- `char* trackerName` the tracker name of the current install. Is `NULL` if
  request failed or response could not be parsed.
- `char* network` the network grouping level of the current install. Is `NULL` if
  request failed, unavailable or response could not be parsed.
- `char* campaign` the campaign grouping level of the current install. Is `NULL` if
  request failed, unavailable or response could not be parsed.
- `char* adgroup` the ad group grouping level of the current install. Is `NULL` if
  request failed, unavailable or response could not be parsed.
- `char* creative` the creative grouping level of the current install. Is `NULL` if
  request failed, unavailable or response could not be parsed.

```cpp
#include "AdjustMarmalade.h"

void response_delegate(adjust_response_data* response) 
{ 
  if (response == NULL) {
    IwTrace(DEFAULT, ("response null"));
    return;
  }
  
  IwTrace(DEFAULT, ("response not null"));
  
  if (response->activityKind != NULL) {
    IwTrace(DEFAULT, ("activityKind"));
    IwTrace(DEFAULT, (response->activityKind));
  }
  IwTrace(DEFAULT, ("success"));
  IwTrace(DEFAULT, (response->success ? "true" : "false"));
  
  // ...
}

int main()
{
  // ...
  
  adjust_AppDidLaunch(appToken, environment, logLevel, eventBuffering);
  
  adjust_SetResponseDelegate(response_delegate);
  
  // ...
}

```

### 8. Disable tracking

You can disable the adjust SDK from tracking by invoking the function `adjust_SetEnabled`
with the enabled parameter as `false`. This setting is remembered between sessions, but it can only
be activated after the first session.

```cpp
adjust_SetEnabled(false);
```

You can verify if the adjust SDK is currently active with the function `adjust_IsEnabled(bool& isEnabled_out)`. It is always possible to activate the adjust SDK by invoking `adjust_SetEnabled` with the enabled parameter as `true`.

[adjust.com]: http://adjust.com
[dashboard]: http://adjust.com
[releases]: https://github.com/adjust/marmalade_sdk/releases
[attribution-data]: https://github.com/adjust/sdks/blob/master/doc/attribution-data.md

## License

The adjust SDK is licensed under the MIT License.

Copyright (c) 2012-2014 adjust GmbH,
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
