#ifndef RAKNET_XBOX360INCLUDES_H
#define RAKNET_XBOX360INCLUDES_H

#if defined(_XBOX)

#include "WindowsIncludes.h"

#include <winsockx.h>

#include "RakAssert.h"

// RakNet hack
#ifdef _WIN32
#define WIN32
#endif


/* Contains portions of headers from Windows 7.1 SDK */


/* ws2def.h */
#define AF_INET6        23              // Internetwork Version 6


/* WinNls.h */
#define WC_COMPOSITECHECK         0x00000200  // convert composite to precomposed


/* Begin netdb.h */
struct hostent {
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
                           terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
                           network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
};
/* End netdb.h */

/* Begin WinSock.h */

/*
 * Options for use with [gs]etsockopt at the IP level.
 */
#define IP_OPTIONS          1           /* set/get IP per-packet options    */
#define IP_MULTICAST_IF     2           /* set/get IP multicast interface   */
#define IP_MULTICAST_TTL    3           /* set/get IP multicast timetolive  */
#define IP_MULTICAST_LOOP   4           /* set/get IP multicast loopback    */
#define IP_ADD_MEMBERSHIP   5           /* add  an IP group membership      */
#define IP_DROP_MEMBERSHIP  6           /* drop an IP group membership      */
#define IP_TTL              7           /* set/get IP Time To Live          */
#define IP_TOS              8           /* set/get IP Type Of Service       */
#define IP_DONTFRAGMENT     9           /* set/get IP Don't Fragment flag   */


#define IP_DEFAULT_MULTICAST_TTL   1    /* normally limit m'casts to 1 hop  */
#define IP_DEFAULT_MULTICAST_LOOP  1    /* normally hear sends if a member  */
#define IP_MAX_MEMBERSHIPS         20   /* per socket; must fit in one mbuf */

struct hostent * gethostbyname(__in_z const char * name);
int gethostname (__out_bcount_part(namelen, return) char * name, __in int namelen);
char * inet_ntoa (__in struct in_addr in);

/* End WinSock.h */


/* Begin ws2ipdef.h */

#define IP_HDRINCL                 2 // Header is included with data.

/* End ws2ipdef.h */
















































#endif // defined(_XBOX)
#endif // RAKNET_XBOX360INCLUDES_H