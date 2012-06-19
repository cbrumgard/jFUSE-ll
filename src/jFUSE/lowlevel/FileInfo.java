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

public class FileInfo
{
    protected long fuse_file_info_ptr = 0;
    
    
    protected FileInfo(long fuse_file_info_ptr)
    {
        this.fuse_file_info_ptr = fuse_file_info_ptr;
    }
    
    public native void      setFlags(long flags);
    public native long      getFlags();
    
    public native void      setWritePage(boolean value);
    public native boolean   getWritePage();
    
    public native void      setDirectIO(boolean value);
    public native boolean   getDirectIO(boolean value);
    
    public native void      setKeepCache(boolean value);
    public native boolean   getKeepCache(boolean value);
    
    public native void      setFlush(boolean value);
    public native boolean   setFlush();
    
    public native void      setNonSeekable(boolean value);
    public native boolean   getNonSeekable();
    
    public native void      setFh(long  fh);
    public native long      getFh();
    
    public native void      setLockOwner(long lockOwnwer);
    public native long      getLockOwner();
}
