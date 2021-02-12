/*  =========================================================================
    test_appnet_all - description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    test_appnet_all -
@discuss
@end
*/

#include "application_network_classes.h"

//
typedef bool (two_node_logic) (appnet_t*, appnet_t*);


// --------------------------------------------
// --------------------------------------------
// --           CALLBACKS                    --
// --------------------------------------------
// --------------------------------------------

void on_action_triggered (const char *action_name,
                          const char *args,
                          uint8_t caller_type,
                          void *called_by,
                          void *userdata)
{
    char* sign = appnet_node_signature(userdata);
    printf("%s: triggered action:%s args:%s\n"
                                              ,sign
                                              ,action_name
                                              ,args);
    free(sign);
}

void on_app_enter(appnet_application_t* app,void* userdata)
{
    assert(app);
    char* signature = appnet_node_signature(userdata);

    appnet_t* self = (appnet_t*)userdata;
    printf("%s: app entered:\n",signature);
    appnet_application_print(app);
    free(signature);
}

void on_app_exit(appnet_application_t* app,void* userdata)
{
    assert(app);
    char* signature = appnet_node_signature(userdata);

    const char* app_name = appnet_application_get_name(app);
    const char* app_uuid = appnet_application_get_peer_id(app);
    printf("%s: app %s[%s] EXIT\n",signature,app_name,app_uuid);
    free(signature);
}

void on_client_enter(appnet_client_t* client,void* userdata)
{
    assert(client);

    char* signature = appnet_node_signature(userdata);

    printf("%s: client %s[%s] ENTERED\n"
                ,signature
                ,appnet_client_get_name(client)
                ,appnet_client_get_peer_id(client));

    free(signature);
}

void on_client_exit(appnet_client_t* client,void* userdata)
{
    assert(client);
    char* signature = appnet_node_signature(userdata);    

    printf("%s: client exit:%s[%s]\n"
            ,signature
            ,appnet_client_get_name(client)
            ,appnet_client_get_peer_id(client));

    free(signature);
}


appnet_t* create_client_node(const char* name){
    appnet_t* client_node = appnet_new(name);
    appnet_client_t* client = appnet_set_client(client_node);
    appnet_client_set_name(client,name);
    appnet_set_timeout(client_node,10.f);

    return client_node;
}

appnet_t* create_application_node(const char* name){
    appnet_t* application_node = appnet_new(name);
    appnet_application_t* app = appnet_set_application(application_node);
    
    appnet_application_set_name(app,name);
    appnet_set_timeout(application_node,10.f);

    return application_node;
}

void add_default_callbacks(appnet_t* node){
    appnet_set_on_app_enter(node,on_app_enter,node);
    appnet_set_on_app_exit(node,on_app_exit,node);
    appnet_set_on_client_enter(node,on_client_enter,node);
    appnet_set_on_client_exit(node,on_client_exit,node);
    appnet_set_on_action_triggered(node,on_action_triggered,node);
}

bool test_trigger(appnet_t* app,appnet_t* client)
{
    zlist_t* app_names = appnet_get_remote_application_names(client);
    appnet_application_t* remote_app = appnet_get_remote_application(client,zlist_first(app_names));
    appnet_application_remote_trigger_action(remote_app,"doit","argument");
    int rc = appnet_receive_event(app);
    assert(rc == APPNET_TYPE_TRIGGER_ACTION);

    return true;
}

int test_enter_exit(two_node_logic* inner_logic) {
    // create client-node
    appnet_t* client_node_1 = create_client_node("client_1");
    add_default_callbacks(client_node_1);

    // create application-node
    appnet_t* app_node_1 = create_application_node("app_1");
    appnet_application_t* app_1 = appnet_get_application(app_node_1);
    appnet_application_add_views(app_1,4,"view1","view2","view.two","globals.resources");
    appnet_application_add_actions(app_1,3,"start","restart","stop");
    add_default_callbacks(app_node_1);

    appnet_start(client_node_1);
    appnet_start(app_node_1);

    sleep(2);

    int rc = appnet_receive_event(client_node_1); // appnode enter
    assert(rc==APPNET_TYPE_APPLICATION_ENTER);

    rc = appnet_receive_event(app_node_1); // client entered
    assert(rc==APPNET_TYPE_CLIENT_ENTER);


    if (inner_logic){
        inner_logic(app_node_1,client_node_1);
    }

    appnet_stop(app_node_1);

    rc = appnet_receive_event(client_node_1); // appnode exited
    assert(rc==APPNET_TYPE_APPLICATION_EXIT);

    appnet_stop(client_node_1);

    appnet_destroy(&app_node_1);
    appnet_destroy(&client_node_1);
}

int main (int argc, char *argv [])
{
    bool verbose = false;
    int argn;
    for (argn = 1; argn < argc; argn++) {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-h")) {
            puts ("test_appnet_all [options] ...");
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
        zsys_info ("test_appnet_all - ");


    test_enter_exit(test_trigger);

    return 0;
}


