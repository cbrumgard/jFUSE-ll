package jFUSE.lowlevel;

import java.io.FileDescriptor;
import java.nio.ByteBuffer;

public class FuseBuffer
{
    protected final long size;
    protected final ByteBuffer buffer;
    protected final FileDescriptor fileDescriptor;
    protected final long filePos;
    
    protected FuseBuffer(long size, ByteBuffer buffer, FileDescriptor fileDescriptor, 
            long filePos)
    {
        this.size = size;
        this.buffer = buffer;
        this.fileDescriptor = fileDescriptor;
        this.filePos        = filePos;
    }
    
    public FuseBuffer(long size, ByteBuffer buffer)
    {
       this(size, buffer, null, -1);
    }
    

    public FuseBuffer(long size, FileDescriptor fileDescriptor)
    {
        this(size, null, fileDescriptor, -1);
    }
    
    public FuseBuffer(long size, FileDescriptor fileDescriptor, long filePosition)
    {
        this(size, null, fileDescriptor, filePosition);
    }
}
