/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "EmptyHeader.h"
#include "../../compat/PlatformDefinitions.h"

#ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS

#ifndef RAKNETSOCKET2_WINDOWS_LINUX_CPP
#define RAKNETSOCKET2_WINDOWS_LINUX_CPP

#if !defined(WINDOWS_STORE_RT) && !defined(__native_client__)

#if MC_PLATFORM_IOS

// Lifed from SLikeNet because *just* iOS 6 was bitching and moaning 25% of the time
// iOS 6 SDK makes phones hate this for some reason
// gethostbyname didn't return NULL on iPhone 5 running iOS 6.1.4 on iOS 5 SDK,
// but did return NULL when running on the same hardware on iOS 6.1 SDK

#ifndef _WIN32
#include <sys/types.h> // used for getifaddrs()
#include <ifaddrs.h>   // used for getifaddrs()
#endif // _WIN32

#include <string> // used for std::string

// based on https://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine#265978
void GetMyIP_Linux(SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS], const bool includeIPv6)
{
	struct ifaddrs* pAddresses = nullptr;

	// #med - add error check to getifaddrs()-call
	getifaddrs(&pAddresses);

	struct ifaddrs* pCurAdapter = pAddresses;
	size_t outAddressIndex = 0;
	while ((pCurAdapter != nullptr) && (outAddressIndex < MAXIMUM_NUMBER_OF_INTERNAL_IDS)) {
		// skip interfaces which don't have any address assigned (according to the manual, this would only apply for BSD, but still we'd check for null here just in case)
		if (pCurAdapter->ifa_addr != nullptr) {
			// skip loopback adapters
			if ((pCurAdapter->ifa_flags & IFF_LOOPBACK) == 0) {
				if (pCurAdapter->ifa_addr->sa_family == AF_INET) {
					const sockaddr_in* const curSocketAddress = reinterpret_cast<const sockaddr_in*>(pCurAdapter->ifa_addr);

					char buffer[INET_ADDRSTRLEN] = { 0 };
					// #med - add return value check
					inet_ntop(AF_INET, &(curSocketAddress->sin_addr), buffer, INET_ADDRSTRLEN);

					const std::string ipv4String(buffer);

					// #med - review, is this really necessary?
					// skip source only addresses (aka: 0.0.0.0/8 - see RFC1700 p.4)
					if (ipv4String.find("0.") != 0) {
						// store the adapter's address
						addresses[outAddressIndex++].address.addr4 = *curSocketAddress;
					}
				}
#if RAKNET_SUPPORT_IPV6 == 1
				else if (includeIPv6 && pCurAdapter->ifa_addr->sa_family == AF_INET6) {
					const sockaddr_in6* const curSocketAddress = reinterpret_cast<const sockaddr_in6*>(pCurAdapter->ifa_addr);

					char buffer[INET6_ADDRSTRLEN] = { 0 };
					// #med - add return value check
					inet_ntop(AF_INET6, &(curSocketAddress->sin6_addr), buffer, INET6_ADDRSTRLEN);

					const std::string ipv6String(buffer);

					// detect and skip non-external addresses
					bool isLocal = false;
					bool isSpecial = false;
					if (ipv6String.find("fe") == 0) {
						const char c = ipv6String[2];
						if (c == '8' || c == '9' || c == 'a' || c == 'b') {
							isLocal = true;
						}
					}
					else if (ipv6String.find("2001:0:") == 0) {
						isSpecial = true;
					}

					if (!(isLocal || isSpecial)) {
						// store the adapter's address
						addresses[outAddressIndex++].address.addr6 = *curSocketAddress;
					}
				}
#endif // RAKNET_SUPPORT_IPV6 == 1
				// else skip the address (neither IPv4 nor IPv6 address)
			}
		}

		pCurAdapter = pCurAdapter->ifa_next;
	}

	if (pAddresses != nullptr)
		freeifaddrs(pAddresses);

	while (outAddressIndex < MAXIMUM_NUMBER_OF_INTERNAL_IDS) {
		addresses[outAddressIndex++] = UNASSIGNED_SYSTEM_ADDRESS;
	}
}

#else // !MC_PLATFORM_IOS

#if RAKNET_SUPPORT_IPV6==1

void PrepareAddrInfoHints2(addrinfo *hints)
{
	memset(hints, 0, sizeof (addrinfo)); // make sure the struct is empty
	hints->ai_socktype = SOCK_DGRAM; // UDP sockets
	hints->ai_flags = AI_PASSIVE;     // fill in my IP for me
}

void GetMyIP_Windows_Linux_IPV4And6( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	int idx=0;
	char ac[ 80 ];
	int err = gethostname( ac, sizeof( ac ) );
	RakAssert(err != -1);
	
	struct addrinfo hints;
	struct addrinfo *servinfo=0, *aip;  // will point to the results
	PrepareAddrInfoHints2(&hints);
	getaddrinfo(ac, "", &hints, &servinfo);

	for (idx=0, aip = servinfo; aip != NULL && idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; aip = aip->ai_next, idx++)
	{
		if (aip->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)aip->ai_addr;
			memcpy(&addresses[idx].address.addr4,ipv4,sizeof(sockaddr_in));
		}
		else
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)aip->ai_addr;
			memcpy(&addresses[idx].address.addr4,ipv6,sizeof(sockaddr_in6));
		}

	}

	freeaddrinfo(servinfo); // free the linked-list
	
	while (idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS)
	{
		addresses[idx]=UNASSIGNED_SYSTEM_ADDRESS;
		idx++;
	}
}

#else

#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(__WIN32__)
#include <netdb.h>
#endif
void GetMyIP_Windows_Linux_IPV4( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{



	int idx=0;
	char ac[ 80 ];
	int err = gethostname( ac, sizeof( ac ) );
    (void) err;
	RakAssert(err != -1);
	
	struct hostent *phe = gethostbyname( ac );

	if ( phe == 0 )
	{
		printf("gethostbyname hostname: %s, errno: %d\n", ac, h_errno);
		RakAssert(phe!=0);
		return ;
	}
	for ( idx = 0; idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS; ++idx )
	{
		if (phe->h_addr_list[ idx ] == 0)
			break;

		memcpy(&addresses[idx].address.addr4.sin_addr,phe->h_addr_list[ idx ],sizeof(struct in_addr));
	}
	
	while (idx < MAXIMUM_NUMBER_OF_INTERNAL_IDS)
	{
		addresses[idx]=UNASSIGNED_SYSTEM_ADDRESS;
		idx++;
	}

}

#endif // RAKNET_SUPPORT_IPV6==1

#endif // MC_PLATFORM_IOS


void GetMyIP_Windows_Linux( SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS] )
{
	#if RAKNET_SUPPORT_IPV6==1
		#if MC_PLATFORM_IOS
			GetMyIP_Linux(addresses, true);
		#else
			GetMyIP_Windows_Linux_IPV4And6(addresses);
		#endif
	#else
		#if MC_PLATFORM_IOS
			GetMyIP_Linux(addresses, false);
		#else
			GetMyIP_Windows_Linux_IPV4(addresses);
		#endif
	#endif
}


#endif // Windows and Linux

#endif // file header

#endif // #ifdef RAKNET_SOCKET_2_INLINE_FUNCTIONS
