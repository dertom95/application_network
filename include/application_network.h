/*  =========================================================================
    application_network - A network for sharing data and actions in a network

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of application-network, an open-source framework for connecting application and viewers p2p

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef APPLICATION_NETWORK_H_H_INCLUDED
#define APPLICATION_NETWORK_H_H_INCLUDED

//  Include the project library file
#include "application_network_library.h"

//  Add your own public definitions here, if you need them
#define STRFREE(str) if (str){free(str);str=NULL;} 

#define STRCPY(DEST,SRC) \
    STRFREE(DEST) \
    {int size      = strlen(SRC)+1;\
    DEST    = malloc(size);\
    memcpy(DEST,SRC,size);}


#endif
