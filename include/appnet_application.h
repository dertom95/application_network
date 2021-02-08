/*  =========================================================================
    appnet_application - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef APPNET_APPLICATION_H_INCLUDED
#define APPNET_APPLICATION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/appnet_application.api" to make changes.
//  @interface
//  This is a stable class, and may not change except for emergencies. It
//  is provided in stable builds.
//  Appnet-Application
APPLICATION_NETWORK_EXPORT appnet_application_t *
    appnet_application_new (void);

//  Destroy the appnet_application.
APPLICATION_NETWORK_EXPORT void
    appnet_application_destroy (appnet_application_t **self_p);

//  add application view ( appnet needs to be set as application-type )
APPLICATION_NETWORK_EXPORT bool
    appnet_application_add_view (appnet_application_t *self, const char *view);

//  get zlist of all views
APPLICATION_NETWORK_EXPORT zlist_t *
    appnet_application_get_view_list (appnet_application_t *self);

//  add application action ( appnet needs to be set as application-type )
APPLICATION_NETWORK_EXPORT bool
    appnet_application_add_action (appnet_application_t *self, const char *action);

//  get zlist of all actions
APPLICATION_NETWORK_EXPORT zlist_t *
    appnet_application_get_action_list (appnet_application_t *self);

//  Self test of this class.
APPLICATION_NETWORK_EXPORT void
    appnet_application_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif