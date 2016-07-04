## Migrate your adjust SDK for Marmalade to 4.7.0 from 3.4.0

### SDK initialization

We have changed how you configure and start the adjust SDK. All initial setup is now done 
with a new instance of the `adjust_config` structure. The following steps should now be taken 
to configure the adjust SDK:

1. Create an instance of an `adjust_config` config structure with the app token and environment.
2. Optionally, you can now call methods of the `adjust_config` structure to specify available options.
3. Launch the SDK by invoking `adjust_Start` with the config structure.

Here is an example of how the setup might look before and after the migration:

##### Before

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

##### After

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

### Event tracking

We also introduced proper event structure that is set up before event is tracked. 
Again, an example of how it might look like before and after:

##### Before

```cpp
adjust_param_type *params = new adjust_param_type();
params->push_back(std::make_pair("foo", "bar"));
params->push_back(std::make_pair("key", "value"));

adjust_TrackEvent("abc123", params);
```

##### After

```cpp
adjust_event* event = new adjust_event("abc123");

event->add_callback_parameter("foo", "bar");
event->add_callback_parameter("key", "value");

adjust_TrackEvent(event);
```

### Revenue tracking

Revenues are now handled like normal events. You just set a revenue and a currency 
to track revenues. Note that it is no longer possible to track revenues without associated 
event tokens. You might need to create an additional event token in your dashboard.

*Please note* - the revenue format has been changed from a cent float to a whole 
currency-unit float. Current revenue tracking must be adjusted to whole currency units 
(i.e., divided by 100) in order to remain consistent.

##### Before

```cpp
adjust_TrackRevenue(1.0, "abc123", NULL);
```

##### After

```cpp
adjust_event* event = new adjust_event("abc123");
event->set_revenue(0.01, "EUR");

adjust_TrackEvent(event);
```
