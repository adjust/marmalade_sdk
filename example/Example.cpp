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
    IwTrace(ADJUSTMARMALADE, (response->adid));
}

static void trace_session_success_data(adjust_session_success_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Session successfully tracked!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

static void trace_session_failure_data(adjust_session_failure_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Session tracking failed!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->will_retry));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

static void trace_event_success_data(adjust_event_success_data* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Event successfully tracked!"));
    IwTrace(ADJUSTMARMALADE, (response->message));
    IwTrace(ADJUSTMARMALADE, (response->timestamp));
    IwTrace(ADJUSTMARMALADE, (response->event_token));
    IwTrace(ADJUSTMARMALADE, (response->adid));
    IwTrace(ADJUSTMARMALADE, (response->json_response));
}

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

static void trace_deeplink_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Deeplink received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

static void trace_deferred_deeplink_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Deferred deeplink received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

static void trace_google_ad_id_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("Google Advertising Identifier received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

static void trace_idfa_data(const char* response) 
{
    IwTrace(ADJUSTMARMALADE, ("IDFA received!"));
    IwTrace(ADJUSTMARMALADE, (response));
}

void initializeSDK() 
{
    // Initialize adjust SDK
    const char* app_token = "2fm9gkqubvpc";
    const char* environment = "sandbox";
    const char* log_level = "verbose";
    const char* user_agent = "dummy_agent";

    adjust_config* config = new adjust_config(app_token, environment);
    
    config->set_log_level(log_level);
    config->set_is_sending_in_background_enabled(true);
    config->set_should_deferred_deeplink_be_opened(true);
    config->set_delay_start(3.6);
    config->set_user_agent(user_agent);

    config->set_attribution_callback(trace_attribution_data);
    config->set_session_success_callback(trace_session_success_data);
    config->set_session_failure_callback(trace_session_failure_data);
    config->set_event_success_callback(trace_event_success_data);
    config->set_event_failure_callback(trace_event_failure_data);
    config->set_deeplink_callback(trace_deeplink_data);
    config->set_deferred_deeplink_callback(trace_deferred_deeplink_data);
    
    config->set_google_ad_id_callback(trace_google_ad_id_data);
    config->set_idfa_callback(trace_idfa_data);

    // Add session callback parameters.
    adjust_AddSessionCallbackParameter("scp_foo", "scp_bar");
    adjust_AddSessionCallbackParameter("scp_key", "scp_value");

    // Remove session callback parameters.
    adjust_RemoveSessionCallbackParameter("scp_foo");

    // Add session partner parameters.
    adjust_AddSessionPartnerParameter("spp_a", "spp_b");
    adjust_AddSessionPartnerParameter("spp_x", "spp_y");

    // Remove session partner parameters.
    adjust_RemoveSessionPartnerParameter("spp_a");

    adjust_ResetSessionCallbackParameters();
    adjust_ResetSessionPartnerParameters();

    adjust_Start(config);

    adjust_SetDeviceToken("mega_giga_dummy_token");

    // adjust_SendFirstPackages();
}

bool OnTrackSimpleEventClick(void* data, CButton* button)
{
    adjust_GetGoogleAdId();

    adjust_event* event = new adjust_event("g3mfiw");

    adjust_TrackEvent(event);

    return true;
}

bool onTrackRevenueEventClick(void* data, CButton* button)
{
    adjust_GetIdfa();

    adjust_event* event = new adjust_event("a4fd35");
    event->set_revenue(0.01, "EUR");
    event->set_transaction_id("transaction_id");

    adjust_TrackEvent(event);

    return true;
}

bool OnTrackCallbackEventClick(void* data, CButton* button)
{
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

    adjust_event* event = new adjust_event("34vgg9");
    event->add_callback_parameter("a", "b");
    event->add_callback_parameter("foo", "bar");
    event->add_callback_parameter("a", "c");

    adjust_TrackEvent(event);

    return true;
}

bool OnTrackPartnerEventClick(void* data, CButton* button)
{
    char* adid;

    adjust_GetAdid(&adid);

    IwTrace(ADJUSTMARMALADE, (adid));

    adjust_event* event = new adjust_event("w788qs");
    event->add_partner_parameter("x", "y");
    event->add_partner_parameter("key", "value");
    event->add_partner_parameter("x", "z");

    adjust_TrackEvent(event);

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
