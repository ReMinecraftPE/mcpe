#include "RakAssert.h"

#ifdef _XBOX

// RakNet hack
#ifdef _WIN32
#define WIN32
#endif


/* Contains portions of headers from Windows 7.1 SDK */


/* ws2def.h */
#define AF_INET6        23              // Internetwork Version 6


/* WinNls.h */
#define WC_COMPOSITECHECK         0x00000200  // convert composite to precomposed


/* Begin WinDef.h */
#define FAR                 far

/*
 * Structures returned by network data base library, taken from the
 * BSD file netdb.h.  All addresses are supplied in host order, and
 * returned in network order (suitable for use in system calls).
 */

struct  hostent {
        char    FAR * h_name;           /* official name of host */
        char    FAR * FAR * h_aliases;  /* alias list */
        short   h_addrtype;             /* host address type */
        short   h_length;               /* length of address */
        char    FAR * FAR * h_addr_list; /* list of addresses */
#define h_addr  h_addr_list[0]          /* address, for backward compat */
};
/* End WinDef.h */


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

struct hostent FAR * PASCAL FAR gethostbyname(__in_z const char FAR * name)
{
	RakAssert("Not yet supported" && 0);
	return NULL;
}

int PASCAL FAR gethostname (
                            __out_bcount_part(namelen, return) char FAR * name,
                            __in int namelen)
{
	RakAssert("Not yet supported" && 0);
	return NULL;
}

char FAR * PASCAL FAR inet_ntoa (__in struct in_addr in)
{
	RakAssert("Not yet supported" && 0);
	return NULL;
}

/* End WinSock.h */


/* Begin ws2ipdef.h */

#define IP_HDRINCL                 2 // Header is included with data.

/* End ws2ipdef.h */

















































#endif // __XBOX360_INCLUDES_H