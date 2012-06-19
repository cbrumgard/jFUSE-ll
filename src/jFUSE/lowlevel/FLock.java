package jFUSE.lowlevel;

public class FLock
{
    public enum Type
    {
        F_RDLCK(assignValue("F_RDLCK")),
        F_WRLCK(assignValue("F_WRLCK")),
        F_UNLCK(assignValue("F_UNLCK"));
       
        
        final protected int _value;
        
        Type(int value)
        {
            this._value = value;
        }
        
        protected static native int assignValue(String name); 
    }
    
    public enum Whence
    {
        SEEK_SET(assignValue("SEEK_SET")),
        SEEK_CUR(assignValue("SEEK_CUR")),
        SEEK_END(assignValue("SEEK_END"));
        
        final protected int _value;
        
        Whence(int value)
        {
            this._value = value;
        }
        
        protected static native int assignValue(String name); 
    }
    
    protected final long _flock_ptr;
    
    protected native static long _allocate_flock();
    protected native void _free_flock();
 
    protected FLock(long val)
    {
        this._flock_ptr = val;
    }
    
    public FLock(Type type, Whence whence, long start, long len, long pid)
    {
        this(_allocate_flock());
        
        this.setType(type);
        this.setWhence(whence);
        this.setStart(start);
        this.setLen(len);
        this.setPid(pid);
    }
    
    public native Type getType();
    public native void setType(Type type);
    
    public native Whence getWhence();
    public native void   setWhence(Whence whence);
    
    public native long getStart();
    public native void setStart(long start);
    
    public native long getLen();
    public native void setLen(long len);
    
    public native long getPid();
    public native void setPid(long pid);
    
    @Override
    protected void finalize() throws Throwable
    {
        System.out.println("Freeing flock");
        _free_flock();
    }
}
