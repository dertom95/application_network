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

#include <stdio.h>
#include <signal.h>


static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

void on_app_enter(appnet_application_t* app,void* userdata)
{
    appnet_t* self = (appnet_t*)userdata;

    appnet_application_print(app);


}

void on_app_exit(appnet_application_t* app,void* userdata)
{
    const char* app_name = appnet_application_get_name(app);
    const char* app_uuid = appnet_application_get_peer_id(app);
    printf("Application EXIT:%s[%s]\n",app_name,app_uuid);
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
    appnet_set_on_app_enter(node,on_app_enter,node);
    appnet_set_on_app_exit(node,on_app_exit,node);
    
    // set this node to be an application
    appnet_client_t* client = appnet_set_client(node);
    appnet_client_set_name(client,"forty-client");
    appnet_start(node);

    //zyre_t* zyre = appnet_get_zyre_node(node);
    sleep(1);
    s_catch_signals ();
    //int count = 5;
    while (true){

        if (s_interrupted){
            break;
        }

        uint8_t rc = appnet_receive_event(node);
        // if (rc == APPNET_TYPE_TIMEOUT){
        //     sleep(0.f);
        // }

        zlist_t* app_keys = appnet_get_remote_application_names(node);
        int app_amount = zlist_size(app_keys);
        
        if (app_amount == 0) continue;

        int pick_app = rand() % app_amount;
        char* app_name = zlist_first(app_keys);
        while(pick_app-->0){
            app_name = zlist_next(app_keys);
        }
//        appnet_application_t* app = (appnet_app)
        assert(app_name);
        appnet_application_t* app = appnet_get_remote_application(node,app_name);
        
        int random_value = rand() % 50;
        if (random_value < 25){
            appnet_application_remote_trigger_action(app,"kick_settlers","1,2,3");
        } else {
            appnet_application_remote_trigger_action(app,"restart","0");
        }

        sleep(1);
    }

    // printf("applications:\n");
    // zlist_t* app_keys = appnet_get_remote_application_names(node);
    // for (void* ptr=zlist_first(app_keys);ptr!=NULL;ptr=zlist_next(app_keys)){
    //     printf("\t%s\n",(char*)ptr);
    // }

    appnet_stop(node);
    appnet_destroy(&node);
    //  @end
    printf ("OK\n");

    return 0;
}
