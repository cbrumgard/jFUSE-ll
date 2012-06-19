/*  jFUSE-ll provides low level bindings to FUSE in Java
    Copyright (C) <2011>  <Christopher Brumgard>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include <sys/xattr.h>

#if __LINUX__ > 0
	#include <attr/xattr.h>
#endif

#include <errno.h>



#define FUSE_EPERM      1      /* Operation not permitted */
#define FUSE_ENOENT     2      /* No such file or directory */
#define FUSE_ESRCH      3      /* No such process */
#define FUSE_EINTR      4      /* Interrupted system call */
#define FUSE_EIO        5      /* I/O error */
#define FUSE_ENXIO      6      /* No such device or address */
#define FUSE_E2BIG      7      /* Argument list too long */
#define FUSE_ENOEXEC    8      /* Exec format error */
#define FUSE_EBADF      9      /* Bad file number */
#define FUSE_ECHILD    10      /* No child processes */
#define FUSE_EAGAIN    11      /* Try again */
#define FUSE_ENOMEM    12      /* Out of memory */
#define FUSE_EACCES    13      /* Permission denied */
#define FUSE_EFAULT    14      /* Bad address */
#define FUSE_ENOTBLK   15      /* Block device required */
#define FUSE_EBUSY     16      /* Device or resource busy */
#define FUSE_EEXIST    17      /* File exists */
#define FUSE_EXDEV     18      /* Cross-device link */
#define FUSE_ENODEV    19      /* No such device */
#define FUSE_ENOTDIR   20      /* Not a directory */
#define FUSE_EISDIR    21      /* Is a directory */
#define FUSE_EINVAL    22      /* Invalid argument */
#define FUSE_ENFILE    23      /* File table overflow */
#define FUSE_EMFILE    24      /* Too many open files */
#define FUSE_ENOTTY    25      /* Not a typewriter */
#define FUSE_ETXTBSY   26      /* Text file busy */
#define FUSE_EFBIG     27      /* File too large */
#define FUSE_ENOSPC    28      /* No space left on device */
#define FUSE_ESPIPE    29      /* Illegal seek */
#define FUSE_EROFS     30      /* Read-only file system */
#define EMLINK    	   31      /* Too many links */
#define FUSE_EPIPE     32      /* Broken pipe */
#define FUSE_EDOM      33      /* Math argument out of domain of func */
#define FUSE_ERANGE    34      /* Math result not representable */
#define FUSE_ENOATTR   35      /* Attribute doesn't exist */

static int errno_code[] =
	{
		0,
		EPERM,      /* Operation not permitted */
		ENOENT,      /* No such file or directory */
		ESRCH,      /* No such process */
		EINTR,      /* Interrupted system call */
		EIO,      /* I/O error */
		ENXIO,      /* No such device or address */
		E2BIG,      /* Argument list too long */
		ENOEXEC,      /* Exec format error */
		EBADF,      /* Bad file number */
		ECHILD,      /* No child processes */
		EAGAIN,      /* Try again */
		ENOMEM,      /* Out of memory */
		EACCES,      /* Permission denied */
		EFAULT,      /* Bad address */
		ENOTBLK,      /* Block device required */
		EBUSY,      /* Device or resource busy */
		EEXIST,      /* File exists */
		EXDEV,      /* Cross-device link */
		ENODEV,      /* No such device */
		ENOTDIR,      /* Not a directory */
		EISDIR,      /* Is a directory */
		EINVAL,      /* Invalid argument */
		ENFILE,      /* File table overflow */
		EMFILE,      /* Too many open files */
		ENOTTY,      /* Not a typewriter */
		ETXTBSY,      /* Text file busy */
		EFBIG,      /* File too large */
		ENOSPC,      /* No space left on device */
		ESPIPE,      /* Illegal seek */
		EROFS,      /* Read-only file system */
		EMLINK,      /* Too many links */
		EPIPE,      /* Broken pipe */
		EDOM,      /* Math argument out of domain of func */
		ERANGE,      /* Math result not representable */
		ENOATTR		/* Attribute doesn't exist */
	};


int errno_convert(int fuse_errno)
{
	/* Gets the absolute value of fuse_errno */
	fuse_errno = (fuse_errno < 0) ? -fuse_errno : fuse_errno;

	/* Unknown error */
	if(fuse_errno > (sizeof(errno_code) / sizeof(int)))
	{
		return 1;
	}

	/* Returns the posix error value */
	return errno_code[fuse_errno];
}
