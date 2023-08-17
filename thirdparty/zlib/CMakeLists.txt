cmake_minimum_required(VERSION 3.16.0)
project(zlib)

# Build
add_library(zlib STATIC
    adler32.c
	compress.c
	crc32.c
	deflate.c
	gzclose.c
	gzlib.c
	gzread.c
	gzwrite.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)
target_include_directories(zlib PUBLIC .)
