/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#define RAKNET_VERSION "4.081"
#define RAKNET_VERSION_NUMBER 4.081
#define RAKNET_VERSION_NUMBER_INT 4081

#define RAKNET_DATE "5/28/2014"

// What compatible protocol version RakNet is using. When this value changes, it indicates this version of RakNet cannot connection to an older version.
// ID_INCOMPATIBLE_PROTOCOL_VERSION will be returned on connection attempt in this case

// ----- The version that RakNet actually implements.
//#define RAKNET_PROTOCOL_VERSION 6
// ----- The version that we fake in order to get actual MCPE clients to connect.
#define RAKNET_PROTOCOL_VERSION 4

// @TODO: Look for a version of RakNet that supports protocol version 4.
// While changing this directly to version 4 will allow interop with official MCPE clients,
// it's not a good solution long-term.
