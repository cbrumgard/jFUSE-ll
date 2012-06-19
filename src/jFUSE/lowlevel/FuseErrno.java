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


package jFUSE.lowlevel;

public class FuseErrno
{
    public final static int EPERM    =  1;      /* Operation not permitted */
    public final static int ENOENT   =  2;      /* No such file or directory */
    public final static int ESRCH    =  3;      /* No such process */
    public final static int EINTR    =  4;      /* Interrupted system call */
    public final static int EIO      =  5;      /* I/O error */
    public final static int ENXIO    =  6;      /* No such device or address */
    public final static int E2BIG    =  7;      /* Argument list too long */
    public final static int ENOEXEC  =  8;      /* Exec format error */
    public final static int EBADF    =  9;      /* Bad file number */
    public final static int ECHILD   = 10;      /* No child processes */
    public final static int EAGAIN   = 11;      /* Try again */
    public final static int ENOMEM   = 12;      /* Out of memory */
    public final static int EACCES   = 13;      /* Permission denied */
    public final static int EFAULT   = 14;      /* Bad address */
    public final static int ENOTBLK  = 15;      /* Block device required */
    public final static int EBUSY    = 16;      /* Device or resource busy */
    public final static int EEXIST   = 17;      /* File exists */
    public final static int EXDEV    = 18;      /* Cross-device link */
    public final static int ENODEV   = 19;      /* No such device */
    public final static int ENOTDIR  = 20;      /* Not a directory */
    public final static int EISDIR   = 21;      /* Is a directory */
    public final static int EINVAL   = 22;      /* Invalid argument */
    public final static int ENFILE   = 23;      /* File table overflow */
    public final static int EMFILE   = 24;      /* Too many open files */
    public final static int ENOTTY   = 25;      /* Not a typewriter */
    public final static int ETXTBSY  = 26;      /* Text file busy */
    public final static int EFBIG    = 27;      /* File too large */
    public final static int ENOSPC   = 28;      /* No space left on device */
    public final static int ESPIPE   = 29;      /* Illegal seek */
    public final static int EROFS    = 30;      /* Read-only file system */
    public final static int EMLINK   = 31;      /* Too many links */
    public final static int EPIPE    = 32;      /* Broken pipe */
    public final static int EDOM     = 33;      /* Math argument out of domain of func */
    public final static int ERANGE   = 34;      /* Math result not representable */
    public final static int ENOATTR  = 35;      /* Attribute doesn't exist */
}