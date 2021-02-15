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
    appnet_application_new (appnet_t *parent);

//  Create a new appnet_application.
APPLICATION_NETWORK_EXPORT appnet_application_t *
    appnet_application_new_from_zyre (zyre_event_t *evt, appnet_t *parent);

//  Destroy the appnet_application.
APPLICATION_NETWORK_EXPORT void
    appnet_application_destroy (appnet_application_t **self_p);

//  checks if this action exists in this application
APPLICATION_NETWORK_EXPORT bool
    appnet_application_has_action (appnet_application_t *self, const char *action_name);

//  get application name
APPLICATION_NETWORK_EXPORT const char *
    appnet_application_get_name (appnet_application_t *self);

//  set application name
APPLICATION_NETWORK_EXPORT void
    appnet_application_set_name (appnet_application_t *self, const char *application_name);

//  get zyre peer id
APPLICATION_NETWORK_EXPORT const char *
    appnet_application_get_peer_id (appnet_application_t *self);

//  set zyre peer id
APPLICATION_NETWORK_EXPORT void
    appnet_application_set_peer_id (appnet_application_t *self, const char *peer_id);

//  return parent appnet-node
APPLICATION_NETWORK_EXPORT appnet_t *
    appnet_application_parent (appnet_application_t *self);

//  print application data
APPLICATION_NETWORK_EXPORT void
    appnet_application_print (appnet_application_t *self);

//  add application view ( appnet needs to be set as application-type )
APPLICATION_NETWORK_EXPORT bool
    appnet_application_add_view (appnet_application_t *self, const char *view, int interval);

//  add multiple views to applications
APPLICATION_NETWORK_EXPORT void
    appnet_application_add_views (appnet_application_t *self, int interval, uint8_t view_amount, const char *view, ...);

//  checks if this viewname exists in this application
APPLICATION_NETWORK_EXPORT bool
    appnet_application_has_view (appnet_application_t *self, const char *view_name);

//  return full zyre-groupname [peer-id].[viewname]
//  Caller owns return value and must destroy it when done.
APPLICATION_NETWORK_EXPORT char *
    appnet_application_zyre_group_name (appnet_application_t *self, const char *view_name);

//  add this user(peer-id) to be subscriber on the specified view
APPLICATION_NETWORK_EXPORT void
    appnet_application_add_subscriber (appnet_application_t *self, const char *viewname, const char *peer_id);

//  remove this user(peer-id) from subscriber-list of the specified view
APPLICATION_NETWORK_EXPORT void
    appnet_application_remove_subscriber (appnet_application_t *self, const char *viewname, const char *peer_id);

//  remove subscriber from all views
APPLICATION_NETWORK_EXPORT void
    appnet_application_remove_subscriber_from_views (appnet_application_t *self, const char *peer_id);

//  get zlist of all views-keys
APPLICATION_NETWORK_EXPORT zlist_t *
    appnet_application_get_view_keys (appnet_application_t *self);

//  return the view-hashtable
APPLICATION_NETWORK_EXPORT zhash_t *
    appnet_application_get_view_hashtable (appnet_application_t *self);

//  get zlist of all actions
APPLICATION_NETWORK_EXPORT zlist_t *
    appnet_application_get_action_list (appnet_application_t *self);

//  add application action ( appnet needs to be set as application-type )
APPLICATION_NETWORK_EXPORT bool
    appnet_application_add_action (appnet_application_t *self, const char *action);

//  add multiple views to applications
APPLICATION_NETWORK_EXPORT void
    appnet_application_add_actions (appnet_application_t *self, uint8_t action_amount, const char *action, ...);

//  remote: subscribe for this application's view
APPLICATION_NETWORK_EXPORT void
    appnet_application_remote_subscribe_view (appnet_application_t *self, const char *view_name);

//  subscribe to multiple views on this application
APPLICATION_NETWORK_EXPORT void
    appnet_application_remote_subscribe_views (appnet_application_t *self, uint8_t view_amount, const char *views, ...);

//  remote: unsubscribe from specified view
APPLICATION_NETWORK_EXPORT void
    appnet_application_remote_unsubscribe_view (appnet_application_t *self, const char *view_name);

//  remote: unsubscribe from all views of this application
APPLICATION_NETWORK_EXPORT void
    appnet_application_remote_unsubscribe_all (appnet_application_t *self);

//  remote: trigger action
APPLICATION_NETWORK_EXPORT void
    appnet_application_remote_trigger_action (appnet_application_t *self, const char *action_name, const char *args);

//  remote: trigger action
APPLICATION_NETWORK_EXPORT void
    appnet_application_remote_trigger_action_data (appnet_application_t *self, const char *action_name, void *data, size_t size);

//  get application meta data a string-json
//  Caller owns return value and must destroy it when done.
APPLICATION_NETWORK_EXPORT char *
    appnet_application_to_metadata_json_string (appnet_application_t *self);

//  Self test of this class.
APPLICATION_NETWORK_EXPORT void
    appnet_application_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
