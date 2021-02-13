/*  =========================================================================
    appnet_view_context - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    appnet_view_context -
@discuss
@end
*/

#include "application_network_classes.h"



//  Structure of our class

struct _appnet_view_context_t {
    char* name;
    uint64_t next_trigger;
    int interval_in_ms;
    zhash_t* subscribers;
    void* userdata;
    zmsg_t* msg;
};


//  --------------------------------------------------------------------------
//  Create a new appnet_view_context

appnet_view_context_t *
appnet_view_context_new (const char* viewname)
{
    appnet_view_context_t *self = (appnet_view_context_t *) zmalloc (sizeof (appnet_view_context_t));
    assert (self);
    //  Initialize class properties here
    STRCPY(self->name,viewname);
    self->subscribers = zhash_new();
    self->interval_in_ms = 2500;
    self->msg=NULL;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the appnet_view_context

void
appnet_view_context_destroy (appnet_view_context_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        appnet_view_context_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free(self->name);
        zhash_destroy(&self->subscribers);

        free (self);
        *self_p = NULL;
    }
}

//  Get name of this view
const char *
    appnet_view_context_viewname (appnet_view_context_t *self)
{
    assert(self);
    return self->name;    
}

//  Get time this view is going to triggered (in ms)
uint64_t
    appnet_view_context_next_triggertime (appnet_view_context_t *self)
{
    assert(self);
    return self->next_trigger;
}

//  Set interval for this view (in milliseconds)
void
    appnet_view_context_set_interval (appnet_view_context_t *self, int interval_in_ms)
{
    assert(self);
    self->interval_in_ms = interval_in_ms;
}

//  Get userdata
void *
    appnet_view_context_userdata (appnet_view_context_t *self)
{
    assert(self);
    return self->userdata;
}

//  set userdata
void
    appnet_view_context_set_userdata (appnet_view_context_t *self, void *userdata)
{
    assert(self);
    self->userdata = userdata;
}

//  Trigger this view
void
    appnet_view_context_prepare_next_interval (appnet_view_context_t *self)
{
    self->next_trigger = zclock_mono() + self->interval_in_ms;
}

//  set this user(peer-id) to be subscriber
void
    appnet_view_context_add_subscriber (appnet_view_context_t *self, const char *peer_id)
{
    assert(self);
    zhash_insert(self->subscribers,peer_id,NULL);
}

//  remove this user(peer-id) from subscriber-list
void
    appnet_view_context_remove_subscriber (appnet_view_context_t *self, const char *peer_id)
{
    assert(self);
    zhash_delete(self->subscribers,peer_id);
}

//  Get the hashtable of all subscrbiers on this view
zhash_t *
    appnet_view_context_get_subscriber_map (appnet_view_context_t *self)
{
    assert(self);
    return self->subscribers;
}

//  Get amount of subscribers
int
    appnet_view_context_get_amount_subscribers (appnet_view_context_t *self)
{
    assert(self);
    return zhash_size(self->subscribers);
}

//  Set serialized view-data
void
    appnet_view_context_set_data (appnet_view_context_t *self, void *data, size_t size)
{
    assert(self);
    //assert(data);
    assert(self->msg==NULL);

    if (data != NULL){
        self->msg = zmsg_new();
        zmsg_addmem(self->msg,data,size);
    }
}

//  Pack data in zmsg, or NULL if no data was set
zmsg_t *
    appnet_view_context_get_zmsg (appnet_view_context_t *self)
{
    zmsg_t* ptr = self->msg;
    self->msg = NULL;
    return ptr;
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
appnet_view_context_test (bool verbose)
{
    printf (" * appnet_view_context: ");

    //  @selftest
    //  Simple create/destroy test
    appnet_view_context_t *self = appnet_view_context_new ("view1");
    assert (self);
    appnet_view_context_destroy (&self);
    //  @end
    printf ("OK\n");
}
