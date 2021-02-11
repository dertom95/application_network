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

typedef struct {
    // peers
    zhash_t* applications;
    zhash_t* clients; 
} peers_t;

struct _appnet_t
{
    char *node_name;
    zyre_t *zyre_node;

    // own data
    appnet_application_t *application_data;
    appnet_client_t *client_data;

    appnet_on_app_enter_cb* on_app_enter;
    void* on_app_enter_userdata;

    appnet_on_client_enter_cb* on_client_enter;
    void* on_client_enter_userdata;

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

    self->peers.applications = zhash_new();
    self->peers.clients = zhash_new();

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

        if (&self->application_data){
            appnet_application_destroy(&self->application_data);
        }

        free (self->node_name);
        self->node_name = NULL;

        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  Set timeout
void
    appnet_set_timeout (appnet_t *self, float timeout)
{
    assert(self);
    zsock_t* socket = zyre_socket(self->zyre_node);
    assert(socket);
    zsock_set_rcvtimeo(socket,timeout);
}

//  Set the current appnet to be an application
appnet_application_t* appnet_set_application (appnet_t *self)
{
    assert( self );
    assert( !self->application_data);
    assert( !self->client_data);

    self->application_data = appnet_application_new();
    return self->application_data;
}

//  Is this appnet an application?
bool
    appnet_is_application (appnet_t *self)
{
    assert(self);

    return self->application_data!=NULL;    
}


//  Get the application-object
appnet_application_t*
    appnet_get_application (appnet_t *self)
{
    assert(self);
    assert(self->application_data);
    return self->application_data;
}

//  Is this appnet a client?
bool
    appnet_is_client (appnet_t *self)
{
    assert(self);
    return self->client_data!=NULL;
}

//  Set the current appnet to be a client
appnet_client_t*
    appnet_set_client (appnet_t *self)
{
    assert(self);
    assert(!self->application_data);
    assert(!self->client_data);

    self->client_data=appnet_client_new();
    return self->client_data;
}

//  Get the client-object
appnet_client_t *
    appnet_get_client (appnet_t *self)
{
    assert(self);
    assert(self->client_data);
    return self->client_data;
}


//  Start the node
void
    appnet_start (appnet_t *self)
{
    assert(self);
    assert(self->zyre_node);

    if (appnet_is_application(self)){
        zyre_set_header(self->zyre_node,APPNET_HEADER_IS_APPLICATION, "true");
        appnet_application_t* app = appnet_get_application(self);
        const char* app_meta_data = appnet_application_to_metadata_json_string(app);
        zyre_set_header(self->zyre_node,APPNET_HEADER_APPLICATION,app_meta_data);
        free((void*)app_meta_data);
    } else {
        zyre_set_header(self->zyre_node,"", "false");
    }

    if (appnet_is_client(self)){
        zyre_set_header(self->zyre_node,APPNET_HEADER_IS_CLIENT, "true");
        appnet_client_t* client = appnet_get_client(self);
        zyre_set_header(self->zyre_node,"client_header","i'm a client");
    } else {
        zyre_set_header(self->zyre_node,APPNET_HEADER_CLIENT, "false");
    }


    zyre_start(self->zyre_node);
}

//  Stop the node
void
    appnet_stop (appnet_t *self)
{
    assert(self);
    assert(self->zyre_node);
    zyre_stop(self->zyre_node);
}

//  Get underlying zyre-node
zyre_t *
    appnet_get_zyre_node (appnet_t *self)
{
    assert(self);
    assert(self->zyre_node);
    return self->zyre_node;
}

//  debug: print zyre-event
appnet_msg_t *
    appnet_receive_event(appnet_t *self)
{
    assert(self);
    assert(self->zyre_node);
    zyre_event_t* evt = zyre_event_new(self->zyre_node);
    appnet_msg_t* msg = appnet_msg_new(evt);

    uint8_t type = appnet_msg_get_type(msg);
    if (type == APPNET_TYPE_APPLICATION_ENTER){
        appnet_application_t* remote_app = appnet_application_new_from_msg(msg);
        if (self->on_app_enter){
            self->on_app_enter(remote_app,self->on_app_enter_userdata);
        }
        
        const char* appname = appnet_application_get_name(remote_app);
        zhash_insert(self->peers.applications,appname,remote_app); 
    }
    else {
        zyre_event_print(evt);
    }



    zyre_event_destroy(&evt);
    return msg;
}    

void
    appnet_set_on_application_enter (appnet_t *self, appnet_on_app_enter_cb callback, void* userdata)
{
    self->on_app_enter = callback; 
    self->on_app_enter_userdata = userdata;   
}

void
    appnet_set_on_client_enter (appnet_t *self, appnet_on_client_enter_cb callback, void *userdata)
{
    self->on_client_enter = callback;
    self->on_app_enter_userdata = userdata;    
}

//  get application by name
appnet_application_t *
    appnet_get_remote_application (appnet_t *self, const char *application_name)
{
    appnet_application_t* remote_app = (appnet_application_t*)zhash_lookup(self->peers.applications,application_name);
}

//  Return all connection application names
zlist_t *
    appnet_get_remote_application_names (appnet_t *self)
{
    return zhash_keys(self->peers.applications);
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

void on_application_enter(appnet_application_t* app,void* userdata)
{
    appnet_t* self = (appnet_t*)userdata;

    appnet_application_print(app);

    const char* peer_id = appnet_application_get_peer_id(app);
    zyre_t* zyre = appnet_get_zyre_node(self);
    
    zhash_t* ht = zhash_new();
    zhash_autofree(ht);
    zhash_insert(ht,"tom","trocha");
    zframe_t* frame = zhash_pack(ht);
    
    

    //zyre_whispers(zyre,peer_id,"tomthetomtomtom");
    zmsg_t* msg = zmsg_new();
    zmsg_addmem(msg,zframe_data(frame),zframe_size(frame));
    zyre_whisper(zyre,peer_id,&msg);
    
    zhash_destroy(&ht);
}

void appnet_test (bool verbose)
{
    printf (" * appnet: ");

    //  @selftest
    //  Simple create/destroy test
    appnet_t *self = appnet_new ("node");

    appnet_set_on_application_enter(self,on_application_enter,self);

    // set this node to be an application
    appnet_application_t* appdata = appnet_set_application(self);

    // set meta
    assert(appnet_get_application(self)==appdata);

    srand (time (NULL));
    int r = rand ();

    char buf[50];
    snprintf (buf, 50, "application-%d", r);

    appnet_application_set_name(appdata,buf);

    appnet_application_add_view(appdata,"entt.comps");
    appnet_application_add_view(appdata,"entt.comps.transform");
    appnet_application_add_view(appdata,"entt.comps.renderable");

    appnet_application_add_action(appdata,"global.cheat.add_resource");
    appnet_application_add_action(appdata,"global.game.stop");

    appnet_start(self);

    int count = 10;

    while (count--) {
        appnet_msg_t* msg = appnet_receive_event(self);
        
        const char* name = appnet_msg_get_name(msg);
        printf("type:%d name:%s\n",appnet_msg_get_type(msg),name);
    }

    assert (self);
    appnet_stop(self);
    appnet_destroy (&self);
    //  @end
    printf ("OK\n");
}
