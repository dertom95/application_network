/*  =========================================================================
    test_appnet_client - description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    test_appnet_client -
@discuss
@end
*/

#include "application_network_classes.h"

int main (int argc, char *argv [])
{
    bool verbose = false;
    int argn;
    for (argn = 1; argn < argc; argn++) {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-h")) {
            puts ("test_appnet_client [options] ...");
            puts ("  --verbose / -v         verbose test output");
            puts ("  --help / -h            this information");
            return 0;
        }
        else
        if (streq (argv [argn], "--verbose")
        ||  streq (argv [argn], "-v"))
            verbose = true;
        else {
            printf ("Unknown option: %s\n", argv [argn]);
            return 1;
        }
    }
    //  Insert main code here
    if (verbose)
        zsys_info ("test_appnet_client - ");

    //  @selftest
    //  Simple create/destroy test
    appnet_t* node = appnet_new ("node");

    // set this node to be an application
    appnet_client_t* client = appnet_set_client(node);
    appnet_start(node);

    zyre_t* zyre = appnet_get_zyre_node(node);
    sleep(1);
    zlist_t* peers = zyre_peers(zyre);
    for (void* peer=zlist_first(peers);peer!=NULL;peer=zlist_next(peers)){
        zuuid_t* peer = (zuuid_t*)peer;
        const char* peer_id = zuuid_str(peer);
        const char* data = zyre_peer_header_value(zyre,peer_id,APPNET_HEADER_IS_APPLICATION);
        int a=0;
        free((void*)peer_id);
        free((void*)peer);
        free((void*)data);
    }

    int count = 5;
    while (count--){
        appnet_receive_event(node);
    }

    appnet_stop(node);
    appnet_destroy(&node);
    //  @end
    printf ("OK\n");

    return 0;
}
