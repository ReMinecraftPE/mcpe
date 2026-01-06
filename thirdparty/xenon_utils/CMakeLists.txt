cmake_minimum_required(VERSION 3.16.0)
project(xenon_utils C ASM)

# Build
add_library(xenon_utils STATIC
	netdb.c
	pthread.c
	spinlock.S
	sockets.c
)
target_include_directories(xenon_utils PUBLIC .)
