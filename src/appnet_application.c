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
    bool remote;

    appnet_t* parent;
};


//  --------------------------------------------------------------------------
//  Create a new appnet_application

appnet_application_t *
appnet_application_new (appnet_t* parent)
{
    appnet_application_t *self = (appnet_application_t *) zmalloc (sizeof (appnet_application_t));
    assert (self);
    //  Initialize class properties here
    self->actions = zlist_new();
    self->views = zlist_new();
    self->remote = false;
    self->parent = parent;

    zyre_t* zyre_node = appnet_get_zyre_node(self->parent);
    const char* peer_id = zyre_uuid(zyre_node);
    STRCPY(self->peer_id,peer_id);

    return self;
}

appnet_application_t *
    appnet_application_new_from_zyre (zyre_event_t *evt, appnet_t *parent)
{
    assert (parent);
    assert (evt);

    appnet_application_t *self = (appnet_application_t *) zmalloc (sizeof (appnet_application_t));
    assert (self);
    
    self->remote = true;
    self->parent = parent;

    const char* app_meta = zyre_event_header(evt,APPNET_HEADER_APPLICATION);
    cJSON* json = cJSON_Parse(app_meta);
    cJSON* name = cJSON_GetObjectItemCaseSensitive(json,"name");
    //  Initialize class properties here

    if (cJSON_IsString(name) && name->valuestring){
        appnet_application_set_name(self,name->valuestring);
        self->name = malloc (sizeof (char) * strlen (name->valuestring) + 1);
        strcpy (self->name, name->valuestring);
    }

    const cJSON* json_views = cJSON_GetObjectItemCaseSensitive(json,"views");
    if (cJSON_IsArray(json_views)){
        int size = cJSON_GetArraySize(json_views);
        if (size>0){
            self->views = zlist_new();
            zlist_autofree(self->views);
            for (int i=0;i<size;i++){
                cJSON* json_view = cJSON_GetArrayItem(json_views,i);
                char* view_name = cJSON_GetStringValue(json_view);
                zlist_append(self->views,(void*)view_name);
            }
        }
    }

    const cJSON* json_actions = cJSON_GetObjectItemCaseSensitive(json,"actions");
    if (cJSON_IsArray(json_actions)){
        int size = cJSON_GetArraySize(json_actions);
        if (size>0){
            self->actions = zlist_new();
            zlist_autofree(self->actions);
            for (int i=0;i<size;i++){
                cJSON* json_action = cJSON_GetArrayItem(json_actions,i);
                char* action_name = cJSON_GetStringValue(json_action);
                zlist_append(self->actions,(void*)action_name);
            }
        }
    }

    cJSON_Delete(json);



    const char* peer_id = zyre_event_peer_uuid(evt);
    if (peer_id){
        STRCPY(self->peer_id, peer_id);
    }

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

//  return parent appnet-node
appnet_t *
    appnet_application_parent (appnet_application_t *self)
{
    assert(self);
    return self->parent;
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
    STRCPY(self->name,application_name);
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

// add multiple views to applications
void
    appnet_application_add_views (appnet_application_t *self, uint8_t view_amount, const char *view, ...)
{
    assert(self);

    va_list args;

    va_start (args, view);
    while(view_amount--){
        appnet_application_add_view(self,view);
        view = va_arg (args, const char*);
    }
    va_end(args);
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

//  add multiple views to applications
void
    appnet_application_add_actions (appnet_application_t *self, uint8_t action_amount, const char *action, ...)
{
    assert(self);

    va_list args;

    va_start (args, action);
    while(action_amount--){
        appnet_application_add_action(self,action);
        action = va_arg (args, const char*);
    }
    va_end(args);    
}    


//  get zlist of all actions
zlist_t *
    appnet_application_get_action_list (appnet_application_t *self)
{
    assert(self);
    assert(self->actions);
    return self->actions;
}

//  get application meta data as string-json
char *
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
    char* json_string = cJSON_PrintUnformatted(json_meta_header);
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

//  remote: trigger action (string)
void
    appnet_application_remote_trigger_action (appnet_application_t *self, const char *action_name, const char *args)
{
    zmsg_t* msg = appnet_msg_create_trigger_action(action_name,args);
    
    zyre_t* zyre = appnet_get_zyre_node(self->parent);
    zyre_whisper(zyre,self->peer_id,&msg);
}

//  remote: trigger action (data-buffer-argument)
void
    appnet_application_remote_trigger_action_data (appnet_application_t *self, const char *action_name, void *data, size_t size)
{
    zmsg_t* msg = appnet_msg_create_trigger_action_data(action_name,data,size);
    
    zyre_t* zyre = appnet_get_zyre_node(self->parent);
    zyre_whisper(zyre,self->peer_id,&msg);
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

void
appnet_application_test (bool verbose)
{
    printf (" * appnet_application: ");

    //  @selftest
    //  Simple create/destroy test
    // appnet_application_t *self = appnet_application_new ();
    // assert (self);
    // appnet_application_destroy (&self);
    //  @end
    printf ("OK\n");
}
