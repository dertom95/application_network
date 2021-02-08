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


struct _appnet_t
{
    char *node_name;
    zyre_t *zyre_node;

    appnet_application_t *application_data;
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

//  Set the current appnet to be an application
void appnet_set_application_type (appnet_t *self)
{
    assert( self );
    assert( !self->application_data);

    self->application_data = appnet_application_new();
}

//  Is this appnet an application?
bool
    appnet_is_application_type (appnet_t *self)
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

//  Start the node
void
    appnet_start (appnet_t *self)
{
    assert(self);
    assert(self->zyre_node);

    if (appnet_is_application_type(self)){
        zyre_set_header(self->zyre_node,"appnet_is_application", "true");

        appnet_application_t* app = appnet_get_application(self);
        zlist_t* views = appnet_application_get_view_list(app);
        char buf[500]={0};
        for(void* ptr=zlist_first(views);ptr!=NULL;){
            strcat(buf,(char*)ptr);
            ptr=zlist_next(views);
            if(ptr) strcat(buf,",");
        }
        zyre_set_header(self->zyre_node,"appnet_application_views",buf);

        zlist_t* actions = appnet_application_get_action_list(app);
        char buf2[500]={0};
        for(void* ptr=zlist_first(actions);ptr!=NULL;){
            strcat(buf2,(char*)ptr);
            ptr=zlist_next(actions);
            if(ptr) strcat(buf2,",");
        }
        zyre_set_header(self->zyre_node,"appnet_application_actions",buf2);        

    } else {
        zyre_set_header(self->zyre_node,"appnet_is_application", "false");
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

//  debug: print zyre-event
void
    appnet_recive_event (appnet_t *self)
{
    assert(self);
    assert(self->zyre_node);
    zyre_event_t* evt = zyre_event_new(self->zyre_node);
    zyre_event_print(evt);
    zyre_event_destroy(&evt);
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

void appnet_test (bool verbose)
{
    printf (" * appnet: ");


    //  @selftest
    //  Simple create/destroy test
    appnet_t *self = appnet_new ("node");

    appnet_set_application_type(self);

    appnet_application_t* appdata = appnet_get_application(self);
    appnet_application_add_view(appdata,"entt.comps");
    appnet_application_add_view(appdata,"entt.comps.transform");
    appnet_application_add_view(appdata,"entt.comps.renderable");

    appnet_application_add_action(appdata,"global.cheat.add_resource");
    appnet_application_add_action(appdata,"global.game.stop");

    appnet_start(self);

    int count = 10;

    while (count--) {
        zyre_event_t *event = zyre_event_new (self->zyre_node);
        printf ("events-left:%d", count);
        zyre_event_print (event);
        zyre_event_destroy (&event);
    }

    assert (self);
    appnet_destroy (&self);
    //  @end
    printf ("OK\n");
}
