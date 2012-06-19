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
import java.util.List;




public class FuseRequest
{
    private long cfuse_req;
    
    
    FuseRequest(long cfuse_req)
    {
        this.cfuse_req = cfuse_req;
    }
    
    public native void reply_none();
    public native void reply_err(int err);
    public native void reply_entry(FuseEntryParam e);
    public native void reply_attr(FuseStat stat, double timeout);
    public native void reply_open(FileInfo fi);
    public native void reply_write(long size);
    
    public native void reply_buf(byte[] buf, long size);
    public native void reply_mbuf(ByteBuffer buf);
    
    public native void reply_iov(ByteBuffer[] buf);
    
    public native void reply_create(FuseEntryParam e, FileInfo fi);
    
    public native void reply_lock(FLock flock);
    
    public native void reply_xattr(long size);
    
    public native FuseCTX getCTX();
    
    public native boolean add_direntry(ByteBuffer buf, String name, 
            FuseStat stat, long off);
    
    
    public native void reply_data(List<FuseBuffer> fuseBuffers, int flags);
}