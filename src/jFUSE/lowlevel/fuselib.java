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


public class fuselib
{
    /* Set attribute flags */
    public static final long FUSE_SET_ATTR_MODE     = getFuseConstants("FUSE_SET_ATTR_MODE");
    public static final long FUSE_SET_ATTR_UID      = getFuseConstants("FUSE_SET_ATTR_UID");     
    public static final long FUSE_SET_ATTR_GID      = getFuseConstants("FUSE_SET_ATTR_GID");
    public static final long FUSE_SET_ATTR_SIZE     = getFuseConstants("FUSE_SET_ATTR_SIZE");
    public static final long FUSE_SET_ATTR_ATIME    = getFuseConstants("FUSE_SET_ATTR_ATIME"); 
    public static final long FUSE_SET_ATTR_MTIME    = getFuseConstants("FUSE_SET_ATTR_MTIME");
    
    /* Open flags */
    public static final long O_ACCMODE              = getFuseConstants("O_ACCMODE");
    public static final long O_RDONLY               = getFuseConstants("O_RDONLY");
    public static final long O_WRONLY               = getFuseConstants("O_WRONLY");
    public static final long O_RDWR                 = getFuseConstants("O_RDWR");
    public static final long O_APPEND               = getFuseConstants("O_APPEND");
    public static final long O_ASYNC                = getFuseConstants("O_ASYNC");
    public static final long O_EXCL                 = getFuseConstants("O_EXCL");
    public static final long O_NOCTTY               = getFuseConstants("O_NOCTTY");
    public static final long O_NOFOLLOW             = getFuseConstants("O_NOFOLLOW");
    public static final long O_NONBLOCK             = getFuseConstants("O_NONBLOCK");
    public static final long O_NDELAY               = getFuseConstants("O_NDELAY");
    public static final long O_SYNC                 = getFuseConstants("O_SYNC");
    public static final long O_TRUNC                = getFuseConstants("O_TRUNC");
    
    /* Xattr flags */
    public static final long XATTR_CREATE           = getFuseConstants("XATTR_CREATE");
    public static final long XATTR_REPLACE          = getFuseConstants("XATTR_REPLACE");
    
    
    protected static native long getFuseConstants(String value);
    
    public static native int    getFuseVersion();
    public static native String getJFuseVersion();
    static native void mount(JFuseFS fusefs, String mountpoint, FuseArgs args,
                                int numThreads);
}