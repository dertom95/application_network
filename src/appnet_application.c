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

        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
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