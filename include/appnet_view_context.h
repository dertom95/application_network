/*  =========================================================================
    appnet_view_context - class description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef APPNET_VIEW_CONTEXT_H_INCLUDED
#define APPNET_VIEW_CONTEXT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/appnet_view_context.api" to make changes.
//  @interface
//  This is a draft class, and may change without notice. It is disabled in
//  stable builds by default. If you use this in applications, please ask
//  for it to be pushed to stable state. Use --enable-drafts to enable.
#ifdef APPLICATION_NETWORK_BUILD_DRAFT_API
//  *** Draft method, for development use, may change without warning ***
//  Create a new appnet_view_context.
APPLICATION_NETWORK_EXPORT appnet_view_context_t *
    appnet_view_context_new (const char *viewname);

//  *** Draft method, for development use, may change without warning ***
//  Destroy the appnet_view_context.
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_destroy (appnet_view_context_t **self_p);

//  *** Draft method, for development use, may change without warning ***
//  Get name of this view
APPLICATION_NETWORK_EXPORT const char *
    appnet_view_context_viewname (appnet_view_context_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Get time this view is going to triggered (in ms)
APPLICATION_NETWORK_EXPORT uint64_t
    appnet_view_context_next_triggertime (appnet_view_context_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Set interval for this view (in milliseconds)
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_set_interval (appnet_view_context_t *self, int interval_in_ms);

//  *** Draft method, for development use, may change without warning ***
//  Get userdata
APPLICATION_NETWORK_EXPORT void *
    appnet_view_context_userdata (appnet_view_context_t *self);

//  *** Draft method, for development use, may change without warning ***
//  set userdata
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_set_userdata (appnet_view_context_t *self, void *userdata);

//  *** Draft method, for development use, may change without warning ***
//  set this user(peer-id) to be subscriber
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_add_subscriber (appnet_view_context_t *self, const char *peer_id);

//  *** Draft method, for development use, may change without warning ***
//  remove this user(peer-id) from subscriber-list
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_remove_subscriber (appnet_view_context_t *self, const char *peer_id);

//  *** Draft method, for development use, may change without warning ***
//  Get the hashtable of all subscrbiers on this view
APPLICATION_NETWORK_EXPORT zhash_t *
    appnet_view_context_get_subscriber_map (appnet_view_context_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Get amount of subscribers
APPLICATION_NETWORK_EXPORT int
    appnet_view_context_get_amount_subscribers (appnet_view_context_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Setup data for next interval
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_prepare_next_interval (appnet_view_context_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Self test of this class.
APPLICATION_NETWORK_EXPORT void
    appnet_view_context_test (bool verbose);

#endif // APPLICATION_NETWORK_BUILD_DRAFT_API
//  @end

#ifdef __cplusplus
}
#endif

#endif
