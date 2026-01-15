#include "XBox360Includes.h"

#ifdef _XBOX

struct hostent * gethostbyname(__in_z const char * name)
{
	RakAssert("Not yet supported" && 0);
	return NULL;
}

int gethostname (__out_bcount_part(namelen, return) char * name, __in int namelen)
{
	RakAssert("Not yet supported" && 0);
	return NULL;
}

char * inet_ntoa (__in struct in_addr in)
{
	RakAssert("Not yet supported" && 0);
	return NULL;
}

#endif