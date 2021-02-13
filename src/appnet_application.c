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
#include <zclock.h>

//  Structure of our class

struct _appnet_application_t {
    zhash_t* views;
    zhash_t* actions;
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
    self->actions = zhash_new();
    self->views = zhash_new();
    self->remote = false;
    self->parent = parent;

    zyre_t* zyre_node = appnet_get_zyre_node(self->parent);
    const char* peer_id = zyre_uuid(zyre_node);
    STRCPY(self->peer_id,peer_id);


    return self;
}

static void
s_delete_view_ctx (void *argument)
{
    appnet_view_context_t *view_ctx = (appnet_view_context_t *) argument;
    appnet_view_context_destroy (&view_ctx);
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
            self->views = zhash_new();
            for (int i=0;i<size;i++){
                cJSON* json_view = cJSON_GetArrayItem(json_views,i);
                char* view_name = cJSON_GetStringValue(json_view);
                appnet_view_context_t* view_ctx = appnet_view_context_new(view_name);
                zhash_insert(self->views,view_name,view_ctx);
                zhash_freefn(self->views,view_name,s_delete_view_ctx);
            }
        }
    }

    const cJSON* json_actions = cJSON_GetObjectItemCaseSensitive(json,"actions");
    if (cJSON_IsArray(json_actions)){
        int size = cJSON_GetArraySize(json_actions);
        if (size>0){
            self->actions = zhash_new();
            zhash_autofree(self->actions);
            for (int i=0;i<size;i++){
                cJSON* json_action = cJSON_GetArrayItem(json_actions,i);
                char* action_name = cJSON_GetStringValue(json_action);
                zhash_insert(self->actions,action_name,action_name);
                //zlist_append(self->actions,(void*)action_name);
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

        zhash_destroy(&self->views);
        zhash_destroy(&self->actions);

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

//  checks if this viewname exists in this application
bool
    appnet_application_has_view (appnet_application_t *self, const char *view_name)
{
    assert(self);
    assert(view_name);
    void* result = zhash_lookup(self->views,view_name);
    return result != NULL;
}

//  checks if this action exists in this application
bool
    appnet_application_has_action (appnet_application_t *self, const char *action_name)
{
    assert(self);
    assert(action_name);
    void* result = zhash_lookup(self->actions,action_name);
    return result != NULL;
}

// get application name
const char *
    appnet_application_get_name (appnet_application_t *self)
{
    assert(self);
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
    appnet_application_add_view (appnet_application_t *self, const char *viewname, int interval)
{
    assert(self);
    assert(self->views);

    appnet_view_context_t* view_ctx = appnet_view_context_new(viewname);
    appnet_view_context_set_interval(view_ctx,interval);
    int rc = zhash_insert(self->views,viewname,view_ctx);
    zhash_freefn(self->views,viewname,s_delete_view_ctx);

    zyre_t* zyre = appnet_get_zyre_node(self->parent);
    zyre_join(zyre,viewname);

    return rc == 0;
}

// add multiple views to applications
void
    appnet_application_add_views (appnet_application_t *self, int interval,uint8_t view_amount, const char *view, ...)
{
    assert(self);

    va_list args;

    va_start (args, view);
    while(view_amount--){
        appnet_application_add_view(self,view,interval);
        view = va_arg (args, const char*);
    }
    va_end(args);
}

//  get zlist of all view-keys
zlist_t *
    appnet_application_get_view_keys (appnet_application_t *self)
{
    assert(self);
    assert(self->views);
    return zhash_keys(self->views);
}

//  return the view-hashtable
zhash_t *
    appnet_application_get_view_hashtable (appnet_application_t *self)
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
    int rc = zhash_insert(self->actions,(char*)action,(char*)action);
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
    return zhash_keys(self->actions);
}

//  add this user(peer-id) to be subscriber on the specified view
void
    appnet_application_add_subscriber (appnet_application_t *self, const char *viewname, const char *peer_id)
{
    assert(self);
    assert(viewname);
    assert(peer_id);

    appnet_view_context_t* view_ctx = zhash_lookup(self->views,viewname);
    assert(view_ctx);
    appnet_view_context_add_subscriber(view_ctx,peer_id);
}

//  remove this user(peer-id) from subscriber-list of the specified view
void
    appnet_application_remove_subscriber (appnet_application_t *self, const char *viewname, const char *peer_id)
{
    assert(self);
    assert(viewname);
    assert(peer_id);

    appnet_view_context_t* view_ctx = zhash_lookup(self->views,viewname);
    assert(view_ctx);
    appnet_view_context_remove_subscriber(view_ctx,peer_id);
}


//  get application meta data as string-json
char *
    appnet_application_to_metadata_json_string (appnet_application_t *self)
{
    assert(self);
    cJSON* json_meta_header = cJSON_CreateObject();
    cJSON_AddStringToObject(json_meta_header,"name",self->name);

    cJSON* json_views = cJSON_CreateArray();
    zlist_t* view_keys = zhash_keys(self->views);
    for(void* ptr=zlist_first(view_keys);ptr!=NULL;){
        cJSON* json_view = cJSON_CreateString((char*)ptr);            
        cJSON_AddItemToArray(json_views,json_view);
        ptr=zlist_next(view_keys);
    }
    cJSON_AddItemToObject(json_meta_header,"views",json_views);
    
    // zyre_set_header(self->zyre_node,"appnet_application_views",buf);
    cJSON* json_actions = cJSON_CreateArray();
    zlist_t* action_names = zhash_keys(self->actions);
    for(void* ptr=zlist_first(action_names);ptr!=NULL;){
        cJSON* json_action = cJSON_CreateString((char*)ptr);
        cJSON_AddItemToArray(json_actions,json_action);
        ptr=zlist_next(action_names);
    }
    cJSON_AddItemToObject(json_meta_header,"actions",json_actions);
    char* json_string = cJSON_PrintUnformatted(json_meta_header);
    cJSON_Delete(json_meta_header);
    return json_string;
}

void appnet_application_print(appnet_application_t* app){
    const char* appname = appnet_application_get_name(app);
    const char* peer_id = appnet_application_get_peer_id(app);
    zlist_t* views    = appnet_application_get_view_keys(app);
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

// --------------------------------------------------------------------------------------
// --                                     remote calls                                 --
// --------------------------------------------------------------------------------------

void whisper_to_app(appnet_application_t* self,zmsg_t* msg)
{
    zyre_t* zyre = appnet_get_zyre_node(self->parent);
    zyre_whisper(zyre,self->peer_id,&msg);    
}

void join_group(appnet_application_t* self,const char* groupname){
    zyre_t* zyre = appnet_get_zyre_node(self->parent);
    zyre_join(zyre,groupname);
}

void leave_group(appnet_application_t* self,const char* groupname){
    zyre_t* zyre = appnet_get_zyre_node(self->parent);
    zyre_leave(zyre,groupname);
}


//  remote: subscribe for this application's view
void
    appnet_application_remote_subscribe_view (appnet_application_t *self, const char *view_name)
{
    assert(self);
    assert(view_name);
    if (!appnet_application_has_view(self,view_name)){
        fprintf(stderr,"unknown view[%s] to subscribe to! ignoring!\n",view_name);
        return;
    }
    zmsg_t* msg = appnet_msg_create_generic_string_list_message(APPNET_MSG_SUBSCRIBE_VIEW,1,view_name);
    whisper_to_app(self,msg);
    join_group(self,view_name);
}

//  subscribe to multiple views on this application
void
    appnet_application_remote_subscribe_views (appnet_application_t *self, uint8_t view_amount, const char *view, ...)
{
    va_list args;
    va_start(args,view);
    for (int i=0;i<view_amount;i++){
        appnet_application_remote_subscribe_view(self,view);
        view = va_arg(args,const char*);
    }
    va_end(args);
}

//  remote: unsubscribe from specified view
void
    appnet_application_remote_unsubscribe_view (appnet_application_t *self, const char *view_name)
{   
   if (!appnet_application_has_view(self,view_name)){
        fprintf(stderr,"unknown view[%s] to unsubscribe from! ignoring!\n",view_name);
        return;
    }
    zmsg_t* msg = appnet_msg_create_generic_string_list_message(APPNET_MSG_UNSUBSCRIBE_VIEW,1,view_name);
    whisper_to_app(self,msg);
    leave_group(self,view_name);
}

//  remote: unsubscribe from all views of this application
void
    appnet_application_remote_unsubscribe_all (appnet_application_t *self)
{
    
}

//  remote: trigger action (string)
void
    appnet_application_remote_trigger_action (appnet_application_t *self, const char *action_name, const char *args)
{
    assert(self);
    assert(action_name);

    if (!appnet_application_has_action(self,action_name)){
        fprintf(stderr,"unknown action[%s] to trigger! ignoring!\n",action_name);
        return;        
    }
    
    zmsg_t* msg = appnet_msg_create_trigger_action(action_name,args);
    whisper_to_app(self,msg);
}

//  remote: trigger action (data-buffer-argument)
void
    appnet_application_remote_trigger_action_data (appnet_application_t *self, const char *action_name, void *data, size_t size)
{
    if (!appnet_application_has_action(self,action_name)){
        fprintf(stderr,"unknown action[%s] to trigger! ignoring!\n",action_name);
        return;        
    }
    zmsg_t* msg = appnet_msg_create_trigger_action_data(action_name,data,size);
    whisper_to_app(self,msg);
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
