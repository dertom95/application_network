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

void on_app_enter(appnet_application_t* app,void* userdata)
{
    appnet_t* self = (appnet_t*)userdata;

    appnet_application_print(app);

}

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
    appnet_t* node = appnet_new ("node-client");
    appnet_set_timeout(node,10.0f);
    appnet_set_on_application_enter(node,on_app_enter,node);
    
    // set this node to be an application
    appnet_client_t* client = appnet_set_client(node);
    appnet_client_set_name(client,"forty-client");
    appnet_start(node);

    //zyre_t* zyre = appnet_get_zyre_node(node);
    sleep(1);
    int count = 5;
    while (count--){
        appnet_msg_t* msg = appnet_receive_event(node);
        uint8_t msg_type = appnet_msg_get_type(msg);
    }

    printf("applications:\n");
    zlist_t* app_keys = appnet_get_remote_application_names(node);
    for (void* ptr=zlist_first(app_keys);ptr!=NULL;ptr=zlist_next(app_keys)){
        printf("\t%s\n",(char*)ptr);
    }

    appnet_stop(node);
    appnet_destroy(&node);
    //  @end
    printf ("OK\n");

    return 0;
}
