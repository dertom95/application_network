/*  =========================================================================
    application_network_classes - private header file

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef APPLICATION_NETWORK_CLASSES_H_INCLUDED
#define APPLICATION_NETWORK_CLASSES_H_INCLUDED

//  Platform definitions, must come first
#include "platform.h"

//  Asserts check the invariants of methods. If they're not
//  fulfilled the program should fail fast. Therefore enforce them!
#ifdef NDEBUG
  #undef NDEBUG
  #include <assert.h>
  #define NDEBUG
#else
  #include <assert.h>
#endif

//  External API
#include "../include/application_network.h"

//  Private external dependencies

//  Opaque class structures to allow forward references

//  Extra headers

//  Internal API


//  *** To avoid double-definitions, only define if building without draft ***
#ifndef APPLICATION_NETWORK_BUILD_DRAFT_API

//  *** Draft global constants, defined for internal use only ***
#define APPNET_PROTO_CALLER_APPLICATION  "caller_app"  //  zframe: caller is an application
#define APPNET_PROTO_CALLER_CLIENT  "caller_client"  //  zframe: caller is a client
#define APPNET_PROTO_DATA_STRING  "string_data"  //
#define APPNET_PROTO_DATA_BUFFER  "buffer_data"  //
#define APPNET_MSG_TRIGGER_ACTION  "trigger_action"  //
#define APPNET_MSG_SUBSCRIBE_VIEW  "subscribe_view"  //
#define APPNET_MSG_UNSUBSCRIBE_VIEW  "unsubscribe_view"  //
#define APPNET_MSG_VIEWDATA  "incoming_viewdata"  //

//  Self test for private classes
APPLICATION_NETWORK_PRIVATE void
    application_network_private_selftest (bool verbose, const char *subtest);

#endif // APPLICATION_NETWORK_BUILD_DRAFT_API

#endif
