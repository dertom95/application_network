/*  =========================================================================
    test_appnet_all - description

    Copyright (c) 2021 the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form
    
     is subject to the terms of the Mozilla Public
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

// application gets request to serialize data
void on_view_request(appnet_application_t* app,appnet_view_context_t* vctx)
{
    appnet_t* appnet = appnet_application_parent(app);
    char* sign = appnet_node_signature(appnet);

    zhash_t* ht = zhash_new();
    zhash_autofree(ht);
    char* viewname = (char*)appnet_view_context_viewname(vctx);
    zhash_insert(ht,"viewname",viewname);
    zhash_insert(ht,"random","stuff");

    zframe_t* frame = zhash_pack(ht);
    void* data  = zframe_data(frame);
    size_t size = zframe_size(frame);
    appnet_view_context_set_data(vctx,data,size);

    zhash_destroy(&ht);
    zframe_destroy(&frame);

    printf("%s: request view(%s) on application\n",sign,viewname);
    free(sign);
}

// client gets viewdata from application
void on_view_received(appnet_application_t* app,const char* viewname,void* data,size_t size,void* userdata)
{
    char* sign = appnet_node_signature(userdata);
    
    zframe_t* frame = zframe_new(data,size);
    zhash_t* ht = zhash_unpack(frame);
    zhash_autofree(ht);
    
    zlist_t* keys = zhash_keys(ht);
    const char* current_key =  zlist_first(keys);


    printf("%s: received viewdata:%s\n",sign,viewname);
    // printf("arguments hashtable-data:\n");

    // int key_size = zlist_size(keys)-1;
    // for(;key_size>=0;key_size--){
    //     const char* value = zhash_lookup(ht,current_key);
    //     printf("\t%s = %s\n",current_key,value);
    //     current_key = zlist_next(keys);
    // }
    zframe_destroy(&frame);
    free(sign);
}

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

void on_action_triggered_data (const char *action_name,
                          void* data,
                          size_t size,
                          uint8_t caller_type,
                          void *called_by,
                          void *userdata)
{
    char* sign = appnet_node_signature(userdata);
    
    zframe_t* frame = zframe_new(data,size);
    zhash_t* ht = zhash_unpack(frame);
    zhash_autofree(ht);
    
    zlist_t* keys = zhash_keys(ht);
    const char* current_key =  zlist_first(keys);

    printf("%s: triggered action:%s\n"
                                              ,sign
                                              ,action_name);
    printf("arguments hashtable-data:\n");

    int key_size = zlist_size(keys)-1;
    for(;key_size>=0;key_size--){
        const char* value = zhash_lookup(ht,current_key);
        printf("\t%s = %s\n",current_key,value);
        current_key = zlist_next(keys);
    }
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

// --------------------------------------------
// --------------------------------------------
// --             HELPERS                    --
// --------------------------------------------
// --------------------------------------------

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
    appnet_set_on_action_triggered_data(node,on_action_triggered_data,node);
    appnet_set_on_view_request(node,on_view_request);
    appnet_set_on_view_received(node,on_view_received,node);
}

// ------------
// nested logic
// ------------
bool test_trigger_string_argument(appnet_t* app,appnet_t* client)
{
    zlist_t* app_names = appnet_get_remote_application_names(client);
    appnet_application_t* remote_app = appnet_get_remote_application(client,zlist_first(app_names));
    appnet_application_remote_trigger_action(remote_app,"doit","argument");
    int rc = appnet_receive_event(app);
    assert(rc == APPNET_TYPE_TIMEOUT); // unknown trigger => not send at all

    appnet_application_remote_trigger_action(remote_app,"start","1");
    rc = appnet_receive_event(app);
    assert(rc == APPNET_TYPE_TRIGGER_ACTION); // unknown trigger => not send at all

    return true;
}

bool test_trigger_data_argument(appnet_t* app,appnet_t* client)
{
    zhash_t* ht = zhash_new();
    zhash_insert(ht,"fortuna","duesseldorf");
    zhash_insert(ht,"zyre","rocks");
    zhash_insert(ht,"hello","there!");
    zframe_t* frame = zhash_pack(ht);
    void* data  = zframe_data(frame);
    size_t size = zframe_size(frame);
    
    zlist_t* app_names = appnet_get_remote_application_names(client);
    appnet_application_t* remote_app = appnet_get_remote_application(client,zlist_first(app_names));
    appnet_application_remote_trigger_action_data(remote_app,"restart",data,size);
    int rc = appnet_receive_event(app);
    assert(rc == APPNET_TYPE_TRIGGER_ACTION);

    return true;
}

bool test_subscribe_2(appnet_t* app,appnet_t* client){
    // get first application
    zlist_t* app_names = appnet_get_remote_application_names(client);
    appnet_application_t* remote_app = appnet_get_remote_application(client,zlist_first(app_names));
    appnet_application_remote_subscribe_views(remote_app,2,"globals.resources","view1");
    sleep(1);

    int64_t start = zclock_mono();
    int64_t end = start + 30000;
    int64_t s1 = start + 10000;
    int64_t s2 = start + 20000;
    int64_t current = start;
    int state = 0;
    while (current < end){
        sleep(0.05f);
        current = zclock_mono();

        appnet_process_views(app);
        appnet_receive_all_events(app);

        if ( state==0 && current > s1 ){
            state = 1;
            printf("------------stop connection for client -----------------------\n");
            // add another view
            appnet_stop(client);
        }
        if ( state==1 && current > s2 ){
            state = 2;
            printf("------------start again client -----------------------\n");
            // add another view
            appnet_start(client);
            appnet_application_remote_subscribe_view(remote_app,"view2");
        }
        appnet_receive_all_events(client);
    }
}

bool test_subscribe_multiple(appnet_t* app,appnet_t* client)
{
    // get first application
    zlist_t* app_names = appnet_get_remote_application_names(client);
    appnet_application_t* remote_app = appnet_get_remote_application(client,zlist_first(app_names));

    // client sends view-request
//    appnet_application_remote_subscribe_view(remote_app,"globals.resources");
    appnet_application_remote_subscribe_views(remote_app,2,"globals.resources","view1");
    int rc = appnet_receive_event(app);
    assert(rc==APPNET_TYPE_SUBSCRIBE_VIEW);

    // add it another time to test check for already subscribed views
    appnet_application_remote_subscribe_view(remote_app,"globals.resources");

    int64_t start = zclock_mono();
    int64_t end = start + 40000;
    int64_t s1 = start + 10000;
    int64_t s2 = start + 20000;
    int64_t s3 = start + 30000;
    int64_t current = start;
    int state = 0;
    while (current < end){
        sleep(0.05f);
        current = zclock_mono();

        appnet_process_views(app);
        appnet_receive_all_events(app);

        if ( state==0 && current > s1 ){
            state = 1;
            printf("------------adding view2 -----------------------\n");
            // add another view
            appnet_application_remote_subscribe_view(remote_app,"view2");
        }
        else if (state==1 && current > s2) {
            state = 2;
            printf("------------unsubscrive all -----------------------\n");
            appnet_application_remote_unsubscribe_all(remote_app);
        }
        else if (state==2 && current > s3) {
            printf("------------adding view2 -----------------------\n");
            // add another view
            appnet_application_remote_subscribe_view(remote_app,"view.two");
            state=3;
        }
        appnet_receive_all_events(client);
    }

    appnet_application_remote_unsubscribe_view(remote_app,"view.two");
    rc = appnet_receive_event(app);
    assert(rc==APPNET_TYPE_UNSUBSCRIBE_VIEW);

    return true;
}

// ---------------------------
// default test with two nodes
// ---------------------------
int test_enter_exit(two_node_logic* inner_logic, bool two_apps) {
    // create client-node or appnode
    appnet_t* node_2 = two_apps
                                ? create_application_node("app_2")
                                : create_client_node("client_1");

    add_default_callbacks(node_2);

    // create application-node
    appnet_t* app_node_1 = create_application_node("app_1");
    appnet_application_t* app_1 = appnet_get_application(app_node_1);
    appnet_application_add_views(app_1,3000,4,"view1","view2","view.two","globals.resources");
    appnet_application_add_actions(app_1,3,"start","restart","stop");
    add_default_callbacks(app_node_1);

    appnet_start(node_2);
    appnet_start(app_node_1);

    sleep(2);

    int rc = appnet_receive_event(node_2); // appnode enter
    assert(rc==APPNET_TYPE_APPLICATION_ENTER);

    rc = appnet_receive_event(app_node_1); // client entered
    if (two_apps){
        assert(rc==APPNET_TYPE_APPLICATION_ENTER);
    } else {
        assert(rc==APPNET_TYPE_CLIENT_ENTER);
    }

    if (inner_logic){
        inner_logic(app_node_1,node_2);
    }

    appnet_stop(app_node_1);

    rc = appnet_receive_event(node_2); // appnode exited
    assert(rc==APPNET_TYPE_APPLICATION_EXIT);

    appnet_stop(node_2);

    appnet_destroy(&app_node_1);
    appnet_destroy(&node_2);
}

int main (int argc, char *argv [])
{
    setbuf(stdout,NULL);
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


//    test_enter_exit(NULL); // start / stop
    //test_enter_exit(test_trigger_string_argument); // start / trigger-string / stop
//    test_enter_exit(test_trigger_data_argument); // start / trigger-string / stop
//    test_enter_exit(test_subscribe_multiple,false);
 //   test_enter_exit(test_subscribe_multiple,true);

    test_enter_exit(test_subscribe_2,false);

    return 0;
}


