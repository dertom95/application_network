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
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new appnet_msg

appnet_msg_t *
appnet_msg_new (void)
{
    appnet_msg_t *self = (appnet_msg_t *) zmalloc (sizeof (appnet_msg_t));
    assert (self);
    //  Initialize class properties here
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

zmsg_t *
    appnet_msg_create_trigger_action (const char *action_name, const char *args)
{
    zmsg_t* msg = zmsg_new();
    zmsg_pushstr(msg,args);
    zmsg_pushstr(msg,APPNET_PROTO_DATA_STRING);
    zmsg_pushstr(msg,action_name);
    zmsg_pushstr(msg,APPNET_MSG_TRIGGER_ACTION);
    return msg;
}

zmsg_t *
    appnet_msg_create_trigger_action_data (const char *action_name, void *data, size_t size)
{
    zmsg_t* msg = zmsg_new();
    zmsg_pushmem(msg,data,size);
    zmsg_pushstr(msg,APPNET_PROTO_DATA_BUFFER);
    zmsg_pushstr(msg,action_name);
    zmsg_pushstr(msg,APPNET_MSG_TRIGGER_ACTION);
    return msg;
}

//  Create trigger action msg (as json)
// char *
//     appnet_msg_create_trigger_action (const char *action_name,const char *args)
// {
//     assert(action_name);
//     cJSON* json = cJSON_CreateObject();
//     // type
//     cJSON_AddStringToObject(json,APPNET_MSG_FIELD_TYPE,APPNET_MSG_TYPE_TRIGGER_ACTION);
//     // action-name
//     cJSON_AddStringToObject(json,APPNET_MSG_FIELD_ACTION_NAME,action_name);
//     // action-name
//     cJSON_AddStringToObject(json,APPNET_MSG_FIELD_ACTION_ARGS,args);
//     char* json_string=NULL;
//     STRCPY(json_string,cJSON_PrintUnformatted(json));
//     cJSON_Delete(json);
//     return json_string;
// }

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
    appnet_msg_t *self = appnet_msg_new ();
    assert (self);
    appnet_msg_destroy (&self);
    //  @end
    printf ("OK\n");
}
