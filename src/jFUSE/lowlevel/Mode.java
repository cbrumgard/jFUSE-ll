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



public class Mode
{
    /* Gets the constants from the system so they match the native constants */
    public final static long S_IFMT = getModeConstants("S_IFMT"); // bitmask for the file type bitfields
    public final static long S_IFSOCK = getModeConstants("S_IFSOCK"); // socket
    public final static long S_IFLNK = getModeConstants("S_IFLNK"); // symbolic link
    public final static long S_IFREG = getModeConstants("S_IFREG"); // regular file
    public final static long S_IFBLK = getModeConstants("S_IFBLK"); // block device
    public final static long S_IFDIR = getModeConstants("S_IFDIR"); // directory
    public final static long S_IFCHR = getModeConstants("S_IFCHR"); // character device
    public final static long S_IFIFO = getModeConstants("S_IFIFO"); // FIFO
    
    public final static long S_ISUID = getModeConstants("S_ISUID"); // set UID bit
    public final static long S_ISGID = getModeConstants("S_ISGID"); // set-group-ID bit (see below)
    public final static long S_ISVTX = getModeConstants("S_ISVTX"); // sticky bit (see below)
    
    public final static long S_IRWXU = getModeConstants("S_IRWXU"); // mask for file owner permissions
    public final static long S_IRUSR = getModeConstants("S_IRUSR"); // owner has read permission
    public final static long S_IWUSR = getModeConstants("S_IWUSR"); // owner has write permission
    public final static long S_IXUSR = getModeConstants("S_IXUSR"); // owner has execute permission
    public final static long S_IRWXG = getModeConstants("S_IRWXG"); // mask for group permissions
    public final static long S_IRGRP = getModeConstants("S_IRGRP"); // group has read permission
    public final static long S_IWGRP = getModeConstants("S_IWGRP"); // group has write permission
    public final static long S_IXGRP = getModeConstants("S_IXGRP"); // group has execute permission
    public final static long S_IRWXO = getModeConstants("S_IRWXO"); // mask for permissions for others (not in group)
    public final static long S_IROTH = getModeConstants("S_IROTH"); // others have read permission
    public final static long S_IWOTH = getModeConstants("S_IWOTH"); // others have write permission
    public final static long S_IXOTH = getModeConstants("S_IXOTH"); // others have execute permission

    /* Instance variables */
    protected long mode;
    
    protected static native long getModeConstants(String value);
    
    
    
    public Mode(long mode)
    {
        
        this.mode = mode;
    }
    
    
    
    public Mode()
    {
        this(0L);
    }
    
    public long getMode()
    {
        return this.mode;
    }
    
    public boolean isSock()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFSOCK;
    }
    
    public boolean isLink()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFLNK;
    }
    
    public boolean isReg()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFREG;
    }
    
    public boolean isBlock()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFBLK;
    }
    
    public boolean isDir()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFDIR;
    }
    
    public boolean isCHR()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFCHR;
    }
    
    public boolean isFIFO()
    {
        return (this.mode & Mode.S_IFMT) == Mode.S_IFIFO;
    }
    
    public boolean isUID()
    {
        return (this.mode & Mode.S_ISUID) == Mode.S_ISUID;
    }
    
    public boolean isGID()
    {
        return (this.mode & Mode.S_ISGID) == Mode.S_ISGID;
    }
    
    public boolean isSticky()
    {
        return (this.mode & Mode.S_ISVTX) == Mode.S_ISVTX;
    }
    
    public boolean isUserReadable()
    {
        return (this.mode & Mode.S_IRUSR) == Mode.S_IRUSR;
    }
    
    public boolean isUserWritable()
    {
        return (this.mode & Mode.S_IWUSR) == Mode.S_IWUSR;
    }
    
    public boolean isUserExecutable()
    {
        return (this.mode & Mode.S_IXUSR) == Mode.S_IXUSR;
    }
    
    public boolean isGroupReadable()
    {
        return (this.mode & Mode.S_IRGRP) == Mode.S_IRGRP;
    }
    
    public boolean isGroupWritable()
    {
        return (this.mode & Mode.S_IWGRP) == Mode.S_IWGRP;
    }
    
    public boolean isGroupExecutable()
    {
        return (this.mode & Mode.S_IXGRP) == Mode.S_IXGRP;
    }
    
    public boolean isOtherReadable()
    {
        return (this.mode & Mode.S_IROTH) == Mode.S_IROTH;
    }
    
    public boolean isOtherWritable()
    {
        return (this.mode & Mode.S_IWOTH) == Mode.S_IWOTH;
    }
    
    public boolean isOtherExecutable()
    {
        return (this.mode & Mode.S_IXOTH) == Mode.S_IXOTH;
    }     
    
    
           
    public void setSock(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT); 
        this.mode = (b) ? this.mode | Mode.S_IFSOCK : this.mode&~Mode.S_IFSOCK;
    }
    
    public void setLink(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT); 
        this.mode = (b) ? this.mode | Mode.S_IFLNK : this.mode&~Mode.S_IFLNK;
    }
    
    public void setReg(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT); 
        this.mode = (b) ? this.mode | Mode.S_IFREG : this.mode&~Mode.S_IFREG;
    }
    
    public void setBlock(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT); 
        this.mode = (b) ? this.mode | Mode.S_IFBLK : this.mode&~Mode.S_IFBLK;
    }
    
    public void setDir(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT); 
        this.mode = (b) ? this.mode | Mode.S_IFDIR : this.mode&~Mode.S_IFDIR;
    }
    
    public void setCHR(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT); 
        this.mode = (b) ? this.mode | Mode.S_IFCHR : this.mode&~Mode.S_IFCHR;
    }
    
    public void setFIFO(boolean b)
    {
        this.mode = (this.mode & ~Mode.S_IFMT) | Mode.S_IFIFO;
    }
    
    public void setUID(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_ISUID : this.mode&~Mode.S_ISUID;
    }
    
    public void setGID(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_ISGID : this.mode&~Mode.S_ISGID;
    }

    public void setSticky(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_ISVTX : this.mode&~Mode.S_ISVTX;
    }
    
    public void setUserReadable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IRUSR : this.mode&~Mode.S_IRUSR;
    }
    
    public void setUserWritable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IWUSR : this.mode&~Mode.S_IWUSR;
    }
    
    public void setUserExecutable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IXUSR : this.mode&~Mode.S_IXUSR;
    }
    
    public void setGroupReadable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IRGRP : this.mode&~Mode.S_IRGRP;
    }
    
    public void setGroupWritable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IWGRP : this.mode&~Mode.S_IWGRP;
    }
    
    public void setGroupExecutable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IXGRP : this.mode&~Mode.S_IXGRP;
    }
    
    public void setOtherReadable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IROTH : this.mode&~Mode.S_IROTH;
    }
    
    public void setOtherWritable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IWOTH : this.mode&~Mode.S_IWOTH;
    }
    
    public void setOtherExecutable(boolean b)
    {
        this.mode = (b) ? this.mode|Mode.S_IXOTH : this.mode&~Mode.S_IXOTH;
    }
}