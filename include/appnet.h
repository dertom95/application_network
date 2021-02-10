/*  =========================================================================
    appnet - class description

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef APPNET_H_INCLUDED
#define APPNET_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/appnet.api" to make changes.
//  @interface
//  This is a stable class, and may not change except for emergencies. It
//  is provided in stable builds.
//  Appnet
APPLICATION_NETWORK_EXPORT appnet_t *
    appnet_new (const char *node_name);

//  Destroy the appnet.
APPLICATION_NETWORK_EXPORT void
    appnet_destroy (appnet_t **self_p);

//  Is this appnet an application?
APPLICATION_NETWORK_EXPORT bool
    appnet_is_application (appnet_t *self);

//  Set the current appnet to be an application
APPLICATION_NETWORK_EXPORT appnet_application_t *
    appnet_set_application (appnet_t *self);

//  Get the application-object
APPLICATION_NETWORK_EXPORT appnet_application_t *
    appnet_get_application (appnet_t *self);

//  Is this appnet a client?
APPLICATION_NETWORK_EXPORT bool
    appnet_is_client (appnet_t *self);

//  Set the current appnet to be a client
APPLICATION_NETWORK_EXPORT appnet_client_t *
    appnet_set_client (appnet_t *self);

//  Get the client-object
APPLICATION_NETWORK_EXPORT appnet_client_t *
    appnet_get_client (appnet_t *self);

//
APPLICATION_NETWORK_EXPORT appnet_msg_t *
    appnet_receive_event (appnet_t *self);

//  Start the node
APPLICATION_NETWORK_EXPORT void
    appnet_start (appnet_t *self);

//  Stop the node
APPLICATION_NETWORK_EXPORT void
    appnet_stop (appnet_t *self);

//  Get underlying zyre-node
APPLICATION_NETWORK_EXPORT zyre_t *
    appnet_get_zyre_node (appnet_t *self);

//  Self test of this class.
APPLICATION_NETWORK_EXPORT void
    appnet_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
