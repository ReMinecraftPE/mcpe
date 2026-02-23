// The macOS 'arch' command outputs 'i386' on x86_64 Tiger, so unsuitable for this

#include <stdio.h>

int main(void) {
#if defined(__x86_64__)
    puts("x86_64");
#elif defined(__i386__)
    puts("i386");
#elif defined(__aarch64__)
    puts("arm64");
#elif defined(__POWERPC__)
#ifdef __LP64__
    puts("ppc64");
#else
    puts("powerpc");
#endif
#else
#error unknown mac architecture
#endif
    return 0;
}
