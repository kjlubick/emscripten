#ifdef __EMSCRIPTEN__
#include <stropts.h>
#endif
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"

int fchown(int fd, uid_t uid, gid_t gid)
{
	int ret = __syscall(SYS_fchown, fd, uid, gid);
#if __EMSCRIPTEN__
	if (ret != -EBADF || !__wasi_fd_is_valid(fd))
		return __syscall_ret(ret);
#else
	if (ret != -EBADF || __syscall(SYS_fcntl, fd, F_GETFD) < 0)
		return __syscall_ret(ret);
#endif

	char buf[15+3*sizeof(int)];
	__procfdname(buf, fd);
#ifdef SYS_chown
	return syscall(SYS_chown, buf, uid, gid);
#else
	return syscall(SYS_fchownat, AT_FDCWD, buf, uid, gid, 0);
#endif

}
