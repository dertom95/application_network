/*  =========================================================================
    appnet_msg - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    appnet_msg -
@discuss
@end
*/

#include "application_network_classes.h"

//  Structure of our class

struct _appnet_msg_t {
    byte type;
    char* name;
    zlist_t* views;
    zlist_t* actions;
};


//  --------------------------------------------------------------------------
//  Create a new appnet_msg

appnet_msg_t *
    appnet_msg_new (zyre_event_t *zyre_event)
{
    appnet_msg_t *self = (appnet_msg_t *) zmalloc (sizeof (appnet_msg_t));
    assert (self);
    //  Initialize class properties here
    if (zyre_event){
        appnet_msg_parse_zyre_event(self,zyre_event);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the appnet_msg

void
appnet_msg_destroy (appnet_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        appnet_msg_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}
//  Parse zyre parse zyre event
void
    appnet_msg_parse_zyre_event (appnet_msg_t *self, zyre_event_t *evt)
{
    const char* evt_type = zyre_event_type(evt);

    if (streq(evt_type,"ENTER")){
        const char* is_application = zyre_event_header(evt,APPNET_HEADER_IS_APPLICATION);
        if (is_application){

            self->type = APPNET_TYPE_APPLICATION;
            const char* app_meta = zyre_event_header(evt,APPNET_HEADER_APPLICATION);
            cJSON* json = cJSON_Parse(app_meta);
            cJSON* name = cJSON_GetObjectItemCaseSensitive(json,"name");
            
            if (cJSON_IsString(name) && name->valuestring){
                self->name = malloc (sizeof (char) * strlen (name->valuestring) + 1);
                strcpy (self->name, name->valuestring);
            }

            const cJSON* json_views = cJSON_GetObjectItemCaseSensitive(json,"views");
            if (cJSON_IsArray(json_views)){
                int size = cJSON_GetArraySize(json_views);
                if (size>0){
                    self->views = zlist_new();
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
                    for (int i=0;i<size;i++){
                        cJSON* json_action = cJSON_GetArrayItem(json_actions,i);
                        char* action_name = cJSON_GetStringValue(json_actions);
                        zlist_append(self->actions,(void*)action_name);
                    }
                }
            }

            cJSON_Delete(json);
        }


        const char* is_client = zyre_event_header(evt,APPNET_HEADER_IS_CLIENT);
        if (is_client){
            self->type = APPNET_TYPE_APPLICATION;
            const char* app_meta = zyre_event_header(evt,APPNET_HEADER_APPLICATION);
            cJSON* json = cJSON_Parse(app_meta);
            cJSON* name = cJSON_GetObjectItemCaseSensitive(json,"name");            
        } 
    }

    //free((void*)evt_type);
    //zyre_event_print(evt);
}

//  Get Message-Type
uint8_t
    appnet_msg_get_type (appnet_msg_t *self)
{
    assert(self);
    return self->type;
}

//  Get Name
 const char *
    appnet_msg_get_name (appnet_msg_t *self)
{
    assert(self);
    return self->name;
}

//  Get views as zlist
 zlist_t *
    appnet_msg_get_views (appnet_msg_t *self)
{
    assert(self);
    return self->views;
}

//  Get actions as zlist
 zlist_t *
    appnet_msg_get_actions (appnet_msg_t *self)
{
    assert(self);
    return self->actions;
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
appnet_msg_test (bool verbose)
{
    printf (" * appnet_msg: ");

    //  @selftest
    //  Simple create/destroy test
    appnet_msg_t *self = appnet_msg_new (NULL);
    assert (self);
    appnet_msg_destroy (&self);
    //  @end
    printf ("OK\n");
}
