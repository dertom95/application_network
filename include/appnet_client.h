/*  =========================================================================
    appnet_client - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef APPNET_CLIENT_H_INCLUDED
#define APPNET_CLIENT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/appnet_client.api" to make changes.
//  @interface
//  This is a stable class, and may not change except for emergencies. It
//  is provided in stable builds.
//  Appnet-Client
APPLICATION_NETWORK_EXPORT appnet_client_t *
    appnet_client_new (appnet_t *parent);

//  Create a new appnet_client.
APPLICATION_NETWORK_EXPORT appnet_client_t *
    appnet_client_new_from_zyre (zyre_event_t *zyre_evt);

//  Destroy the appnet_client.
APPLICATION_NETWORK_EXPORT void
    appnet_client_destroy (appnet_client_t **self_p);

//
//  Caller owns return value and must destroy it when done.
APPLICATION_NETWORK_EXPORT char *
    appnet_client_to_metadata_json_string (appnet_client_t *self);

//  get client name
APPLICATION_NETWORK_EXPORT const char *
    appnet_client_get_name (appnet_client_t *self);

//  set client name
APPLICATION_NETWORK_EXPORT void
    appnet_client_set_name (appnet_client_t *self, const char *name);

//  get client name
APPLICATION_NETWORK_EXPORT const char *
    appnet_client_get_peer_id (appnet_client_t *self);

//  print the client info
APPLICATION_NETWORK_EXPORT void
    appnet_client_print (appnet_client_t *self);

//  return parent appnet-node
APPLICATION_NETWORK_EXPORT appnet_t *
    appnet_client_parent (appnet_client_t *self);

//  Self test of this class.
APPLICATION_NETWORK_EXPORT void
    appnet_client_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
