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

public class FuseStat
{
    protected long inode;
    protected long size;
    protected Mode mode;
    protected long atime;
    protected long mtime;
    protected long ctime;
    
    public FuseStat(long inode, long size, Mode mode, long atime, long mtime,
            long ctime)
    {
        /*TODO add the stat stuff */  
        
        this.inode = inode;
        this.size  = size;
        this.mode  = mode;
        this.atime = atime;
        this.mtime = mtime;
        this.ctime = ctime;
    }
    
    public FuseStat(long inode, long size, long atime, long mtime, long ctime)
    {
        /*TODO add the stat stuff */  
        
        this(inode, size, new Mode(), atime, mtime, ctime);
    }
    
    public FuseStat(long inode, long size, long mode)
    {
        this.inode = inode;
        this.size  = size;
        this.mode  = new Mode(mode);
    }
    
    public void setInode(long inode)
    {
        this.inode = inode;
    }
    
    public void setSize(long size)
    {
        this.size = size;
    }

    public Mode getMode()
    {
        return mode;
    }

    public void setMode(Mode mode)
    {
        this.mode = mode;
    }

    public long getInode()
    {
        return inode;
    }

    public long getSize()
    {
        return size;
    }
    
    public long getAtime()
    {
        return atime;
    }

    public void setAtime(long atime)
    {
        this.atime = atime;
    }

    public long getMtime()
    {
        return mtime;
    }

    public void setMtime(long mtime)
    {
        this.mtime = mtime;
    }

    public long getCtime()
    {
        return ctime;
    }

    public void setCtime(long ctime)
    {
        this.ctime = ctime;
    }
}