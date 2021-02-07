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

#include<time.h>
#include<stdlib.h>

#include <zyre.h>


//  Structure of our class

struct _appnet_t {
    char* node_name;
    zyre_t* zyre_node;
};


//  --------------------------------------------------------------------------
//  Create a new appnet

appnet_t *
appnet_new (const char *node_name)
{
    appnet_t *self = (appnet_t *) zmalloc (sizeof (appnet_t));
    assert (self);
    
    srand(time(NULL));
    int r = rand();

    char buf[50];
    snprintf(buf,50,"%s-%d",node_name,r);

    self->node_name = malloc(sizeof(char)*strlen(buf)+1);
    strcpy(self->node_name,buf);

    self->zyre_node = zyre_new(self->node_name);
    zyre_set_header(self->zyre_node,"is_application","true");
    zyre_set_header(self->zyre_node,"is_viewer","false");
    zyre_start(self->zyre_node); // connect to the zyre-network

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the appnet

void
appnet_destroy (appnet_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        appnet_t *self = *self_p;

        zyre_destroy( &self->zyre_node );
        free(self->node_name); self->node_name=NULL;

        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
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
appnet_test (bool verbose)
{


    printf (" * appnet: ");

    //  @selftest
    //  Simple create/destroy test
    appnet_t *self = appnet_new ("node");

    int count = 10;

    while (count--) {
        zyre_event_t* event = zyre_event_new( self->zyre_node );
        printf("events-left:%d",count);
        zyre_event_print(event);
        zyre_event_destroy(&event);
    }

    assert (self);
    appnet_destroy (&self);
    //  @end
    printf ("OK\n");
}
