#include <lwip/sockets.h>

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen){
    return 0;
}

int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen){
    return 0;
}

int lwip_shutdown(int s, int how){
    return 0;
}

int lwip_getpeername(int s, struct sockaddr *name, socklen_t *namelen){
    return 0;
}

int lwip_getsockname(int s, struct sockaddr *name, socklen_t *namelen){
    return 0;
}

int lwip_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen){
    return 0;
}

int lwip_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen){
    return 0;
}

/*int lwip_close(int s){
    return 0;
}*/

int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen){
    return 0;
}

int lwip_listen(int s, int backlog){
    return 0;
}

ssize_t lwip_recv(int s, void *mem, size_t len, int flags){
    return 0;
}

/*ssize_t lwip_read(int s, void *mem, size_t len){
    return 0;
}*/

ssize_t lwip_readv(int s, const struct iovec *iov, int iovcnt){
    return 0;
}

ssize_t lwip_recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen){
    return 0;
}

ssize_t lwip_recvmsg(int s, struct msghdr *message, int flags){
    return 0;
}

ssize_t lwip_send(int s, const void *dataptr, size_t size, int flags){
    return 0;
}

ssize_t lwip_sendmsg(int s, const struct msghdr *message, int flags){
    return 0;
}

ssize_t lwip_sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen){
    return 0;
}

int lwip_socket(int domain, int type, int protocol){
    return 0;
}

/*ssize_t lwip_write(int s, const void *dataptr, size_t size){
    return 0;
}*/

ssize_t lwip_writev(int s, const struct iovec *iov, int iovcnt){
    return 0;
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout){
    return 0;
}

int lwip_ioctl(int s, long cmd, void *argp){
    return 0;
}

int lwip_fcntl(int s, int cmd, int val){
    return 0;
}

const char *lwip_inet_ntop(int af, const void *src, char *dst, socklen_t size){
    return 0;
}

int lwip_inet_pton(int af, const char *src, void *dst){
    return 0;
}
