#include "s3e.h"
#include "IwNUI.h"
#include "IwDebug.h"
#include "AdjustMarmalade.h"

using namespace IwNUI;

static void trace_attribution_data(adjust_attribution_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Attribution changed!"));
    IwTrace(ADJUSTMARMALADE, (response->tracker_token));
    IwTrace(ADJUSTMARMALADE, (response->tracker_name));
    IwTrace(ADJUSTMARMALADE, (response->network));
    IwTrace(ADJUSTMARMALADE, (response->campaign));
    IwTrace(ADJUSTMARMALADE, (response->ad_group));
    IwTrace(ADJUSTMARMALADE, (response->creative));
    IwTrace(ADJUSTMARMALADE, (response->click_label));
}

void initializeSDK() 
{
    // Initialize adjust SDK
    adjust_config* config = new adjust_config("{YourAppToken}", "sandbox");
    config->set_log_level("verbose");

    adjust_SetAttributionCallback(trace_attribution_data);

    adjust_Start(config);

    delete config;
}

bool OnTrackSimpleEventClick(void* data, CButton* button)
{
    adjust_event* event = new adjust_event("{YourEventToken}");

    adjust_TrackEvent(event);

    delete event;

    return true;
}

bool onTrackRevenueEventClick(void* data, CButton* button)
{
    adjust_event* event = new adjust_event("{YourEventToken}");
    event->set_revenue(0.01, "EUR");

    adjust_TrackEvent(event);

    delete event;

    return true;
}

bool OnTrackCallbackEventClick(void* data, CButton* button)
{
    adjust_event* event = new adjust_event("{YourEventToken}");
    event->add_callback_parameter("a", "b");
    event->add_callback_parameter("foo", "bar");
    event->add_callback_parameter("a", "c");

    adjust_TrackEvent(event);

    delete event;

    return true;
}

bool OnTrackPartnerEventClick(void* data, CButton* button)
{
    adjust_event* event = new adjust_event("{YourEventToken}");
    event->add_partner_parameter("x", "y");
    event->add_partner_parameter("key", "value");
    event->add_partner_parameter("x", "z");

    adjust_TrackEvent(event);

    delete event;

    return true;
}

bool OnEnableOfflineModeClick(void* data, CButton* button)
{
    adjust_SetOfflineMode(true);

    return true;
}

bool OnDisableOfflineModeClick(void* data, CButton* button)
{
    adjust_SetOfflineMode(false);

    return true;
}

bool OnEnableSDKClick(void* data, CButton* button)
{
    adjust_SetEnabled(true);

    return true;
}

bool OnDisableSDKClick(void* data, CButton* button)
{
    adjust_SetEnabled(false);

    return true;
}

bool OnIsSDKEnabledClick(void* data, CButton* button)
{
    bool is_enabled;

    adjust_IsEnabled(is_enabled);

    if (is_enabled == true) {
        button->SetAttribute("caption", "Is Enabled? Yes!");
    } else {
        button->SetAttribute("caption", "Is Enabled? No!");
    }

    return true;
}

void initializeUI() {
    CAppPtr app = CreateApp();
    CWindowPtr window = CreateWindow();

    app->AddWindow(window);

    CViewPtr view = CreateView("canvas");

    CButtonPtr btnTrackSimpleEvent = CreateButton(CAttributes()
                                                  .Set("name",    "btnTrackSimpleEvent")
                                                  .Set("caption", "Track Simple Event")
                                                  .Set("x1",      "50%")
                                                  .Set("y1",      "5%")
                                                  .Set("width",   "50%")
                                                  .Set("height",  "7%")
                                                  .Set("alignW",  "centre"));

    CButtonPtr btnTrackRevenueEvent = CreateButton(CAttributes()
                                                   .Set("name",    "btnTrackRevenueEvent")
                                                   .Set("caption", "Track Revenue Event")
                                                   .Set("x1",      "50%")
                                                   .Set("y1",      "15%")
                                                   .Set("width",   "50%")
                                                   .Set("height",  "7%")
                                                   .Set("alignW",  "centre"));

    CButtonPtr btnTrackCallbackEvent = CreateButton(CAttributes()
                                                    .Set("name",    "btnTrackCallbackEvent")
                                                    .Set("caption", "Track Callback Event")
                                                    .Set("x1",      "50%")
                                                    .Set("y1",      "25%")
                                                    .Set("width",   "50%")
                                                    .Set("height",  "7%")
                                                    .Set("alignW",  "centre"));

    CButtonPtr btnTrackPartnerEvent = CreateButton(CAttributes()
                                                   .Set("name",    "btnTrackPartnerEvent")
                                                   .Set("caption", "Track Partner Event")
                                                   .Set("x1",      "50%")
                                                   .Set("y1",      "35%")
                                                   .Set("width",   "50%")
                                                   .Set("height",  "7%")
                                                   .Set("alignW",  "centre"));

    CButtonPtr btnEnableOfflineMode = CreateButton(CAttributes()
                                                   .Set("name",    "btnEnableOfflineMode")
                                                   .Set("caption", "Enable Offline Mode")
                                                   .Set("x1",      "50%")
                                                   .Set("y1",      "45%")
                                                   .Set("width",   "50%")
                                                   .Set("height",  "7%")
                                                   .Set("alignW",  "centre"));

    CButtonPtr btnDisableOfflineMode = CreateButton(CAttributes()
                                                    .Set("name",    "btnDisableOfflineMode")
                                                    .Set("caption", "Disable Offline Mode")
                                                    .Set("x1",      "50%")
                                                    .Set("y1",      "55%")
                                                    .Set("width",   "50%")
                                                    .Set("height",  "7%")
                                                    .Set("alignW",  "centre"));

    CButtonPtr btnEnableSDK = CreateButton(CAttributes()
                                           .Set("name",    "btnEnableSDK")
                                           .Set("caption", "Enable SDK")
                                           .Set("x1",      "50%")
                                           .Set("y1",      "65%")
                                           .Set("width",   "50%")
                                           .Set("height",  "7%")
                                           .Set("alignW",  "centre"));

    CButtonPtr btnDisableSDK = CreateButton(CAttributes()
                                            .Set("name",    "btnDisableSDK")
                                            .Set("caption", "Disable SDK")
                                            .Set("x1",      "50%")
                                            .Set("y1",      "75%")
                                            .Set("width",   "50%")
                                            .Set("height",  "7%")
                                            .Set("alignW",  "centre"));

    CButtonPtr btnIsSDKEnabled = CreateButton(CAttributes()
                                              .Set("name",    "btnIsSDKEnabled")
                                              .Set("caption", "Is SDK Enabled?")
                                              .Set("x1",      "50%")
                                              .Set("y1",      "85%")
                                              .Set("width",   "50%")
                                              .Set("height",  "7%")
                                              .Set("alignW",  "centre"));

    btnTrackSimpleEvent->SetEventHandler("click", (void*)NULL, &OnTrackSimpleEventClick);
    btnTrackRevenueEvent->SetEventHandler("click", (void*)NULL, &onTrackRevenueEventClick);
    btnTrackCallbackEvent->SetEventHandler("click", (void*)NULL, &OnTrackCallbackEventClick);
    btnTrackPartnerEvent->SetEventHandler("click", (void*)NULL, &OnTrackPartnerEventClick);
    btnEnableOfflineMode->SetEventHandler("click", (void*)NULL, &OnEnableOfflineModeClick);
    btnDisableOfflineMode->SetEventHandler("click", (void*)NULL, &OnDisableOfflineModeClick);
    btnEnableSDK->SetEventHandler("click", (void*)NULL, &OnEnableSDKClick);
    btnDisableSDK->SetEventHandler("click", (void*)NULL, &OnDisableSDKClick);
    btnIsSDKEnabled->SetEventHandler("click", (void*)NULL, &OnIsSDKEnabledClick);

    view->AddChild(btnTrackSimpleEvent);
    view->AddChild(btnTrackRevenueEvent);
    view->AddChild(btnTrackCallbackEvent);
    view->AddChild(btnTrackPartnerEvent);
    view->AddChild(btnEnableOfflineMode);
    view->AddChild(btnDisableOfflineMode);
    view->AddChild(btnEnableSDK);
    view->AddChild(btnDisableSDK);
    view->AddChild(btnIsSDKEnabled);

    window->SetChild(view);

    // Initialize adjust SDK
    initializeSDK();

    app->ShowWindow(window);
    app->Run();
}

// Main entry point for the application
int main()
{
    // Initialize UI
    initializeUI();
    
    return 0;
}
