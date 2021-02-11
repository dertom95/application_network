/*  =========================================================================
    appnet_application - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    appnet_application -
@discuss
@end
*/

#include "application_network_classes.h"

#include <zlist.h>

//  Structure of our class

struct _appnet_application_t {
    zlist_t* views;
    zlist_t* actions;
    char* name;
    char* peer_id;
};


//  --------------------------------------------------------------------------
//  Create a new appnet_application

appnet_application_t *
appnet_application_new (void)
{
    appnet_application_t *self = (appnet_application_t *) zmalloc (sizeof (appnet_application_t));
    assert (self);
    //  Initialize class properties here
    self->actions = zlist_new();
    self->views = zlist_new();

    return self;
}

appnet_application_t *
    appnet_application_new_from_msg (appnet_msg_t *msg)
{
    appnet_application_t *self = (appnet_application_t *) zmalloc (sizeof (appnet_application_t));
    assert (self);
    //  Initialize class properties here
    const char* msg_name    = appnet_msg_get_name(msg);
    const char* msg_peer_id = appnet_msg_get_peer_id(msg);
    zlist_t* msg_views   = appnet_msg_get_views(msg);
    zlist_t* msg_actions = appnet_msg_get_actions(msg);

    // int size      = strlen(msg_name)+1;
    // self->name    = malloc(size);
    // memcpy(self->name,msg_name,size);

    STRCPY(self->name,msg_name);
    STRCPY(self->peer_id,msg_peer_id);

    self->actions = zlist_dup(msg_actions);
    self->views   = zlist_dup(msg_views);

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the appnet_application


void
appnet_application_destroy (appnet_application_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        appnet_application_t *self = *self_p;

        zlist_destroy(&self->views);
        zlist_destroy(&self->actions);

        STRFREE(self->name);
        STRFREE(self->peer_id);

        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

// get application name
const char *
    appnet_application_get_name (appnet_application_t *self)
{
    return self->name;
}

//  set application name
void
    appnet_application_set_name (appnet_application_t *self, const char *application_name)
{
    self->name = malloc(sizeof(char)*strlen(application_name)+1);
    strcpy(self->name,application_name);
}

//  get zyre peer id
const char *
    appnet_application_get_peer_id (appnet_application_t *self)
{
    return self->peer_id;
}

//  set zyre peer id
void
    appnet_application_set_peer_id (appnet_application_t *self, const char *peer_id)
{
    STRCPY(self->peer_id,peer_id);
}

//  add application view ( appnet needs to be set as application-type )
bool
    appnet_application_add_view (appnet_application_t *self, const char *viewname)
{
    assert(self);
    assert(self->views);
    int rc = zlist_append(self->views,(void*)viewname);
    return rc == 0;
}
//  get zlist of all views
zlist_t *
    appnet_application_get_view_list (appnet_application_t *self)
{
    assert(self);
    assert(self->views);
    return self->views;
}


//  add application action ( appnet needs to be set as application-type )
bool
    appnet_application_add_action (appnet_application_t *self, const char *action)
{
    assert(self);
    assert(self->actions);
    int rc = zlist_append(self->actions,(void*)action);
    return rc == 0;
}

//  get zlist of all actions
zlist_t *
    appnet_application_get_action_list (appnet_application_t *self)
{
    assert(self);
    assert(self->actions);
    return self->actions;
}

//  get application meta data a string-json
const char *
    appnet_application_to_metadata_json_string (appnet_application_t *self)
{
    assert(self);
    cJSON* json_meta_header = cJSON_CreateObject();
    cJSON_AddStringToObject(json_meta_header,"name",self->name);

    cJSON* json_views = cJSON_CreateArray();
    for(void* ptr=zlist_first(self->views);ptr!=NULL;){
        cJSON* json_view = cJSON_CreateString((char*)ptr);            
        cJSON_AddItemToArray(json_views,json_view);
        ptr=zlist_next(self->views);
    }
    cJSON_AddItemToObject(json_meta_header,"views",json_views);
    
    // zyre_set_header(self->zyre_node,"appnet_application_views",buf);
    cJSON* json_actions = cJSON_CreateArray();
    for(void* ptr=zlist_first(self->actions);ptr!=NULL;){
        cJSON* json_action = cJSON_CreateString((char*)ptr);
        cJSON_AddItemToArray(json_actions,json_action);
        ptr=zlist_next(self->actions);
    }
    cJSON_AddItemToObject(json_meta_header,"actions",json_actions);
    const char* json_string = cJSON_PrintUnformatted(json_meta_header);
    cJSON_Delete(json_meta_header);
    return json_string;
}

void appnet_application_print(appnet_application_t* app){
    const char* appname = appnet_application_get_name(app);
    const char* peer_id = appnet_application_get_peer_id(app);
    zlist_t* views    = appnet_application_get_view_list(app);
    zlist_t* actions = appnet_application_get_action_list(app);

    printf("application:%s[%s]\n",appname,peer_id);
    printf("\nviews:\n");
    if (zlist_size(views)){
        for (void* ptr=zlist_first(views);ptr!=NULL;ptr=zlist_next(views)){
            printf("\tview:%s\n",(char*)ptr);
        }
    } else {
        printf("\tnone\n");
    }

    printf("\nActions:\n");

    if (zlist_size(actions)){
        for (void* ptr=zlist_first(actions);ptr!=NULL;ptr=zlist_next(actions)){
            printf("\tactions:%s\n",(char*)ptr);
        }
    } else {
        printf("\tnone\n");
    }    
}

// remote calls 

//  remote: subscribe for this application's view
void
    appnet_application_remote_subscribe_view (appnet_application_t *self, const char *view_name)
{}

//  remote: unsubscribe from specified view
void
    appnet_application_remote_unsubscribe_view (appnet_application_t *self, const char *view_name)
{}

//  remote: unsubscribe from all views of this application
void
    appnet_application_remote_unsubscribe_all (appnet_application_t *self)
{}

//  remote: trigger action
void
    appnet_application_remote_trigger_action (appnet_application_t *self, const char *action_name, const char *json)
{}


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

void
appnet_application_test (bool verbose)
{
    printf (" * appnet_application: ");

    //  @selftest
    //  Simple create/destroy test
    appnet_application_t *self = appnet_application_new ();
    assert (self);
    appnet_application_destroy (&self);
    //  @end
    printf ("OK\n");
}
