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

import java.nio.ByteBuffer;




public abstract class JFuseFS
{
    
    /* Loads the library */
    static
    {
        System.loadLibrary("jfuselib");
    }
    
    /* java.lang.UnsupportedOperationException */
    /* Lookup method */
    public void init(Object userdata, ConnInfo conn)
    {
        throw new UnsupportedOperationException("init");
    }
    
    public void destroy(Object userdata)
    {
        throw new UnsupportedOperationException("destroy");
    }
    
    public void lookup(FuseRequest request, long parent_ino, String path)
    {
        throw new UnsupportedOperationException("lookup");
    }
    
    public void forget(FuseRequest request, long ino, long nlookup)
    {
        throw new UnsupportedOperationException("forget");
    }
    
    public void mkdir(FuseRequest request, long parent_ino, String name, Mode mode)
    {
        throw new UnsupportedOperationException("forget");
    }
    
    public void getattr(FuseRequest request, long ino)
    {
        throw new UnsupportedOperationException("getattr");
    }
    
    public void open(FuseRequest request, long ino, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("open");
    }
    
    public void read(FuseRequest request, long ino, long size, long offset, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("read");
    }
    
    public void write(FuseRequest req, long ino, ByteBuffer buf, long size, long off, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("write");
    }
    
    public void release(FuseRequest request, long ino, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("release");
    }
    
    public void unlink(FuseRequest request, long parent, String filename)
    {
        throw new UnsupportedOperationException("unlink");
    }
    
    public void opendir(FuseRequest request, long ino, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("opendir");
    }
    
    public void readdir(FuseRequest request, long ino, long size, long off, 
            FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("readdir");
    }
    
    public void releasedir(FuseRequest request, long ino, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("releasedir");
    }
    
    public void create(FuseRequest request, long parent, String name, Mode mode, 
                        FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("create");
    }
    
    public void setattr(FuseRequest request, long ino, FuseStat stat, 
                        long to_set, FileInfo fileInfo)
    {
        throw new UnsupportedOperationException("setattr");
    }
    
    public void setxattr(FuseRequest request, long ino, String name, 
                         ByteBuffer value, int flags)
    {
        throw new UnsupportedOperationException("setxattr");
    }
    
    public void getxattr(FuseRequest request, long ino, String name, long size)
    {
        throw new UnsupportedOperationException("getxattr");
    }
    
    public void listxattr(FuseRequest request, long ino, long size)
    {
        throw new UnsupportedOperationException("setxattr");
    }
    
    public void removexattr(FuseRequest request, long ino, String name)
    {
        throw new UnsupportedOperationException("removexattr");
    }
    
    public void ioctl(FuseRequest req, long ino, int cmd, ByteBuffer arg, 
            FileInfo fi, long flags, ByteBuffer in_buf, long in_bufsz, 
            long out_bufsz)
    {
        throw new UnsupportedOperationException("ioctl");
    }
    
    
    public void fsync(FuseRequest req, long ino, boolean dataOnly, 
            FileInfo fileInfo)
    {
        
        throw new UnsupportedOperationException("fsync");
    }
    
    public void getlk(FuseRequest request, long ino, FileInfo fileInfo,
            FLock flock)
    {
        throw new UnsupportedOperationException("getlk");
    }
    
    public void setlk(FuseRequest request, long ino, FileInfo fileInfo, 
            FLock flock, boolean canSleep)
    {
        throw new UnsupportedOperationException("setlk");
    }
    
    public void rename(FuseRequest request, long parentIno, String name, 
            long newParentIno, String newName)
    {
        throw new UnsupportedOperationException("rename");
    }
    
    /* Mounts the directory */
    public void mount(String mountpoint, FuseArgs args, int numThreads)
    {
        fuselib.mount(this, mountpoint, args, numThreads);
    }
}