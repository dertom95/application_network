/*  =========================================================================
    appnet - class description

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    appnet -
@discuss
@end
*/

#include "application_network_classes.h"

#include <time.h>
#include <stdlib.h>
#include <zyre.h>
#include <zlist.h>
#include <zhash.h>

typedef struct
{
    // peers
    zhash_t *applications;
    zhash_t *clients;
} peers_t;

struct _appnet_t
{
    char *node_name;
    zyre_t *zyre_node;

    // own data
    appnet_application_t *application_data;
    appnet_client_t *client_data;

    appnet_on_app_enter *on_app_enter;
    void *on_app_enter_userdata;

    appnet_on_client_enter *on_client_enter;
    void *on_client_enter_userdata;

    appnet_on_action_triggered *on_action_triggered;
    void *on_action_triggered_userdata;

    // peers
    peers_t peers;
};


//  --------------------------------------------------------------------------
//  Create a new appnet

appnet_t *appnet_new (const char *node_name)
{
    appnet_t *self = (appnet_t *) zmalloc (sizeof (appnet_t));
    assert (self);

    srand (time (NULL));
    int r = rand ();

    char buf[50];
    snprintf (buf, 50, "%s-%d", node_name, r);

    self->node_name = malloc (sizeof (char) * strlen (buf) + 1);
    strcpy (self->node_name, buf);

    self->zyre_node = zyre_new (self->node_name);

    self->peers.applications = zhash_new ();
    self->peers.clients = zhash_new ();

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the appnet

void appnet_destroy (appnet_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        appnet_t *self = *self_p;

        appnet_application_destroy (&self->application_data);
        zyre_destroy (&self->zyre_node);

        if (&self->application_data) {
            appnet_application_destroy (&self->application_data);
        }

        free (self->node_name);
        self->node_name = NULL;

        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

// --------------- callbacks ----------------------------------------------
#define IMPLEMENT_CALLBACK_SETTER(callbackname)                                \
    void appnet_set_##callbackname (                                           \
      appnet_t *self, appnet_##callbackname callback, void *userdata)          \
    {                                                                          \
        self->callbackname = callback;                                         \
        self->callbackname##_userdata = userdata;                              \
    }

IMPLEMENT_CALLBACK_SETTER (on_action_triggered);
IMPLEMENT_CALLBACK_SETTER (on_client_enter);
IMPLEMENT_CALLBACK_SETTER (on_app_enter);

// ---------------- class implementation -------------------------------------

//  Set timeout
void appnet_set_timeout (appnet_t *self, float timeout)
{
    assert (self);
    zsock_t *socket = zyre_socket (self->zyre_node);
    assert (socket);
    zsock_set_rcvtimeo (socket, timeout);
}

//  Set the current appnet to be an application
appnet_application_t *appnet_set_application (appnet_t *self)
{
    assert (self);
    assert (!self->application_data);
    assert (!self->client_data);

    self->application_data = appnet_application_new ();
    return self->application_data;
}

//  Is this appnet an application?
bool appnet_is_application (appnet_t *self)
{
    assert (self);

    return self->application_data != NULL;
}


//  Get the application-object
appnet_application_t *appnet_get_application (appnet_t *self)
{
    assert (self);
    assert (self->application_data);
    return self->application_data;
}

//  Is this appnet a client?
bool appnet_is_client (appnet_t *self)
{
    assert (self);
    return self->client_data != NULL;
}

//  Set the current appnet to be a client
appnet_client_t *appnet_set_client (appnet_t *self)
{
    assert (self);
    assert (!self->application_data);
    assert (!self->client_data);

    self->client_data = appnet_client_new ();
    return self->client_data;
}

//  Get the client-object
appnet_client_t *appnet_get_client (appnet_t *self)
{
    assert (self);
    assert (self->client_data);
    return self->client_data;
}


//  Start the node
void appnet_start (appnet_t *self)
{
    assert (self);
    assert (self->zyre_node);

    if (appnet_is_application (self)) {
        zyre_set_header (self->zyre_node, APPNET_HEADER_IS_APPLICATION, "true");
        appnet_application_t *app = appnet_get_application (self);
        const char *app_meta_data =
          appnet_application_to_metadata_json_string (app);
        zyre_set_header (self->zyre_node, APPNET_HEADER_APPLICATION,
                         app_meta_data);
        free ((void *) app_meta_data);
    } else {
        zyre_set_header (self->zyre_node, "", "false");
    }

    if (appnet_is_client (self)) {
        zyre_set_header (self->zyre_node, APPNET_HEADER_IS_CLIENT, "true");
        appnet_client_t *client = appnet_get_client (self);
        char *client_meta_data = appnet_client_to_metadata_json_string (client);
        zyre_set_header (self->zyre_node, APPNET_HEADER_CLIENT,
                         client_meta_data);
        free (client_meta_data);
    } else {
        zyre_set_header (self->zyre_node, APPNET_HEADER_CLIENT, "false");
    }


    zyre_start (self->zyre_node);
}

//  Stop the node
void appnet_stop (appnet_t *self)
{
    assert (self);
    assert (self->zyre_node);
    zyre_stop (self->zyre_node);
}

//  Get underlying zyre-node
zyre_t *appnet_get_zyre_node (appnet_t *self)
{
    assert (self);
    assert (self->zyre_node);
    return self->zyre_node;
}

uint8_t appnet_receive_event_enter (appnet_t *self, zyre_event_t *evt)
{
    assert (self);
    assert (evt);

    const char *is_application =
      zyre_event_header (evt, APPNET_HEADER_IS_APPLICATION);
    const char *is_client = zyre_event_header (evt, APPNET_HEADER_IS_CLIENT);

    // -- app-enter --
    if (is_application) {
        appnet_application_t *remote_app =
          appnet_application_new_from_zyre (evt, self);

        assert (remote_app);
        if (self->on_app_enter) {
            self->on_app_enter (remote_app, self->on_app_enter_userdata);
        }

        const char *app_peer_id = appnet_application_get_peer_id (remote_app);
        zhash_insert (self->peers.applications, app_peer_id, remote_app);
        return APPNET_TYPE_APPLICATION_ENTER;
    } else if (is_client) {
        appnet_client_t *remote_client = appnet_client_new_from_zyre (evt);
        assert (remote_client);

        if (self->on_client_enter) {
            self->on_client_enter (remote_client,
                                   self->on_client_enter_userdata);
        }
        const char *client_peer_id = appnet_client_get_peer_id (remote_client);
        zhash_insert (self->peers.clients, client_peer_id, remote_client);
        return APPNET_TYPE_CLIENT_ENTER;
    } else {
        fprintf (stderr, "unknown enter state! with zyre-event:\n");
        zyre_event_print (evt);
        return APPNET_TYPE_UNSUPPORTED;
    }
}

uint8_t appnet_receive_event_whisper (appnet_t *self, zyre_event_t *evt)
{
    assert (self);
    assert (evt);

    const char *client_uuid = zyre_event_peer_uuid (evt);
    assert (client_uuid);
    appnet_client_t *client = zhash_lookup (self->peers.clients, client_uuid);

    uint8_t caller_type = APPNET_CALLER_TYPE_CLIENT;

    if (!client) {
        // no client, look in app-list
        client = zhash_lookup (self->peers.applications, client_uuid);

        if (!client) {
            fprintf (
              stderr,
              "couldn't find client(zyre-node with uuid:%s) ignoring event\n",
              client_uuid);
            zyre_event_print (evt);
            return APPNET_TYPE_UNKNOWN_PEER;
        }
        caller_type = APPNET_CALLER_TYPE_APPLICATION;
    }

    zmsg_t *msg = zyre_event_msg (evt);
    char *incoming_str = zmsg_popstr (msg);
    cJSON *json = cJSON_Parse (incoming_str);

    const char *whisper_type =
      cJSON_GetObjectItem (json, APPNET_MSG_FIELD_TYPE)->valuestring;

    if (streq (whisper_type, APPNET_MSG_TYPE_TRIGGER_ACTION)) {
        if (self->on_action_triggered) {
            const char *action_name =
              cJSON_GetObjectItem (json, APPNET_MSG_FIELD_ACTION_NAME)
                ->valuestring;
            const char *action_args =
              cJSON_GetObjectItem (json, APPNET_MSG_FIELD_ACTION_ARGS)
                ->valuestring;

            self->on_action_triggered (action_name, action_args, client,
                                       caller_type,
                                       self->on_action_triggered_userdata);
        }
        return APPNET_TYPE_WISPHER_INCOMING;
    } else {
        fprintf (stderr, "unsupported WHISPER-Type:%s", whisper_type);
        zyre_event_print (evt);
        return APPNET_TYPE_UNSUPPORTED;
    }

    free (incoming_str);
    cJSON_Delete (json);
}

//  debug: print zyre-event
uint8_t _appnet_receive_event (appnet_t *self)
{
    assert (self);
    assert (self->zyre_node);
    zyre_event_t *evt = zyre_event_new (self->zyre_node);

    uint8_t return_code = APPNET_TYPE_TIMEOUT;

    if (!evt) {
        return APPNET_TYPE_TIMEOUT;
    }

    const char *evt_type = zyre_event_type (evt);

    if (streq (evt_type, "ENTER")) {
        return_code = appnet_receive_event_enter (self, evt); // process zyre_enter_event
    } else if (streq (evt_type, "WHISPER")) {
        return_code = appnet_receive_event_whisper (self, evt); // process zyre_whisper_event
    } else {
        return_code = APPNET_TYPE_UNSUPPORTED;
        //fprintf(stderr,"unsupported event-type:%s\n",evt_type);
        //zyre_event_print (evt);
    }

    zyre_event_destroy (&evt);
    return return_code;
}

uint8_t appnet_receive_event (appnet_t *self)
{
    uint8_t last_type = APPNET_TYPE_UNSUPPORTED;
    while (last_type != APPNET_TYPE_TIMEOUT){
        last_type = _appnet_receive_event(self);
        if (last_type != APPNET_TYPE_UNSUPPORTED){
            break;
        }
    }
    return last_type;
}

//  Receive all messages and call the corresponding callbacks
void
    appnet_receive_all_events (appnet_t *self)
{
    uint8_t last_type = APPNET_TYPE_UNSUPPORTED;
    while (last_type != APPNET_TYPE_TIMEOUT){
        last_type = appnet_receive_event(self);
        if (last_type == APPNET_TYPE_TIMEOUT){
            break;
        }
    }    
}

//  get application by name
appnet_application_t *
appnet_get_remote_application (appnet_t *self, const char *application_name)
{
    assert (self);
    appnet_application_t *remote_app = (appnet_application_t *) zhash_lookup (
      self->peers.applications, application_name);
}

//  Return all connection application names
zlist_t *appnet_get_remote_application_names (appnet_t *self)
{
    assert (self);
    return zhash_keys (self->peers.applications);
}

zhash_t *appnet_get_remote_applications (appnet_t *self)
{
    assert (self);
    return self->peers.applications;
}

//  custom: send string to application
void appnet_remote_send_string (appnet_t *self,
                                const char *peer_id,
                                const char *string_data)
{
    assert (self);
    assert (peer_id);
    assert (string_data);

    zmsg_t* msg = zmsg_new();
    zmsg_pushstr(msg,string_data);
    zyre_whisper (self->zyre_node, peer_id, &msg);
}
//  custom: send buffer(void* size) to application
void appnet_remote_send_buffer (appnet_t *self,
                                const char *peer_id,
                                void *data,
                                size_t size)
{
    assert (self);
    assert (peer_id);
    assert (data);

    zmsg_t *msg = zmsg_new ();
    zmsg_addmem (msg, data, size);
    zyre_whisper (self->zyre_node, peer_id, &msg);
}


//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

#include <stdio.h>
#include <signal.h>


static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

void on_application_enter (appnet_application_t *app, void *userdata)
{
    appnet_t *self = (appnet_t *) userdata;

    appnet_application_print (app);

    const char *peer_id = appnet_application_get_peer_id (app);
    zyre_t *zyre = appnet_get_zyre_node (self);


    // chose first action
    zlist_t *actions = appnet_application_get_action_list (app);
    const char *action_name = (const char *) zlist_first (actions);
    // trigger action
    appnet_application_remote_trigger_action (app, action_name, "{}");
}

void on_action_triggered (const char *action_name,
                          const char *args,
                          void *triggered_by,
                          uint8_t caller_type,
                          void *userdata)
{
    if (caller_type == APPNET_CALLER_TYPE_APPLICATION) {
        printf ("Client[%s|%s] triggered ACTION: %s\n"
                    ,appnet_application_get_name ((appnet_application_t*)triggered_by)
                    ,appnet_application_get_peer_id ((appnet_application_t*)triggered_by)
                    ,action_name);
    } else if (caller_type == APPNET_CALLER_TYPE_CLIENT) {
        printf ("Client[%s|%s] triggered ACTION: %s\n"
                ,appnet_client_get_name ((appnet_client_t*)triggered_by)
                ,appnet_client_get_peer_id ((appnet_client_t*)triggered_by)
                ,action_name);
    }
}

void appnet_test (bool verbose)
{
    printf (" * appnet: ");

    // application - to trigger actions and deliver views
    appnet_t *self = appnet_new ("node");
    appnet_set_timeout (self, 10.0f);

    appnet_set_on_app_enter (self, on_application_enter, self);
    appnet_set_on_action_triggered (self, on_action_triggered, self);

    // set this node to be an application
    appnet_application_t *appdata = appnet_set_application (self);

    // set meta
    assert (appnet_get_application (self) == appdata);

    srand (time (NULL));
    int r = rand ();

    char buf[50];
    snprintf (buf, 50, "application-%d", r);

    appnet_application_set_name (appdata, buf);

    appnet_application_add_view (appdata, "entt.comps");
    appnet_application_add_view (appdata, "entt.comps.transform");
    appnet_application_add_view (appdata, "entt.comps.renderable");

    appnet_application_add_action (appdata, "global.cheat.add_resource");
    appnet_application_add_action (appdata, "global.game.stop");

    appnet_start (self);

    //    int count = 10;
    s_catch_signals ();
    while (true) {
        int8_t rc = appnet_receive_event (self);

        if (s_interrupted) {
            break;
        }

        if (rc == 0) {
            // timeout
            sleep (0.1f);
            continue;
        }
    }

    assert (self);
    appnet_stop (self);
    appnet_destroy (&self);
    //  @end
    printf ("OK\n");
}
