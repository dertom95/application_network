/*  =========================================================================
    appnet_client - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    appnet_client -
@discuss
@end
*/

#include "application_network_classes.h"

//  Structure of our class

struct _appnet_client_t {
    char* name;
    char* peer_id;
    bool remote;
    appnet_t* parent;
};


//  --------------------------------------------------------------------------
//  Create a new appnet_client

appnet_client_t *
appnet_client_new (appnet_t* parent)
{
    appnet_client_t *self = (appnet_client_t *) zmalloc (sizeof (appnet_client_t));
    assert (self);
    //  Initialize class properties here
    self->remote = false;
    self->parent = parent;

    zyre_t* zyre_node = appnet_get_zyre_node(self->parent);
    const char* peer_id = zyre_uuid(zyre_node);
    STRCPY(self->peer_id,peer_id);    

    return self;
}

//  Create a new appnet_client.
appnet_client_t *
    appnet_client_new_from_zyre (zyre_event_t *evt)
{
    appnet_client_t *self = (appnet_client_t *) zmalloc (sizeof (appnet_client_t));
    assert (self);
    self->remote = true;

    const char* app_meta = zyre_event_header(evt,APPNET_HEADER_CLIENT);
    cJSON* json = cJSON_Parse(app_meta);
    cJSON* name = cJSON_GetObjectItemCaseSensitive(json,"name");   
    if (cJSON_IsString(name)){
        STRCPY(self->name,name->valuestring);
    }
    STRCPY(self->peer_id,zyre_event_peer_uuid(evt));

    cJSON_free(json);      
    return self;
}    


//  get client name
const char *
    appnet_client_get_peer_id (appnet_client_t *self)
{
    assert(self);
    return self->peer_id;
}


//  --------------------------------------------------------------------------
//  Destroy the appnet_client

void
appnet_client_destroy (appnet_client_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        appnet_client_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free(self->name);
        free (self);
        *self_p = NULL;
    }
}

//  return parent appnet-node
appnet_t *
    appnet_client_parent (appnet_client_t *self)
{
    assert(self);
    return self->parent;
}

/// get client name
const char *
    appnet_client_get_name (appnet_client_t *self)
{
    return self->name;
}

//  set client name
void
    appnet_client_set_name (appnet_client_t *self, const char *client_name)
{
    self->name = malloc(sizeof(char)*strlen(client_name)+1);
    strcpy(self->name,client_name);    
}

//
char *
    appnet_client_to_metadata_json_string (appnet_client_t *self)
{
    //  get application meta data as string-json
    assert(self);
    cJSON* json_meta_header = cJSON_CreateObject();
    cJSON_AddStringToObject(json_meta_header,"name",self->name);
    char* out_json = cJSON_PrintUnformatted(json_meta_header);
    cJSON_Delete(json_meta_header);
    return out_json;
}

//  print the client info
void
    appnet_client_print (appnet_client_t *self)
{
    printf("client:[%s|%s] remote:%s\n",self->name,self->peer_id,self->remote?"true":"false");
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
appnet_client_test (bool verbose)
{
    printf (" * appnet_client: ");

    //  @selftest
    //  Simple create/destroy test
    // appnet_client_t *self = appnet_client_new ();
    // assert (self);
    // appnet_client_destroy (&self);
    //  @end
    printf ("OK\n");
}
