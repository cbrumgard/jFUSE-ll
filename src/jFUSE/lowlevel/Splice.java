package jFUSE.lowlevel;

import java.io.FileDescriptor;
import java.io.IOException;
import java.net.Socket;
import java.nio.channels.SocketChannel;

public class Splice
{
    /* Splice Constants */
    final public static int SPLICE_F_MOVE      = _SPLICE_F_MOVE();
    final public static int SPLICE_F_NONBLOCK  = _SPLICE_F_NONBLOCK();
    final public static int SPLICE_F_MORE      = _SPLICE_F_MORE();
    final public static int SPLICE_F_GIFT      = _SPLICE_F_GIFT();     

    static public class Pipe
    {
        private FileDescriptor in   = null;
        private FileDescriptor out  = null;
        
        private native void _setSize(FileDescriptor fd, long pipeSize);
        private native long _getSize(FileDescriptor fd);
        private native void _close(FileDescriptor in, FileDescriptor out);
        
        Pipe(FileDescriptor in, FileDescriptor out)
        { 
            this.in = in;
            this.out = out;
        }
        
        public FileDescriptor getReadFileDescriptor()
        {
            return in;
        }
      
        public FileDescriptor getWriteFileDescriptor()
        {
            return out;
        }
        
        public void setSize(long pipeSize)
        {
            _setSize(this.in, pipeSize);
        }
        
        public long getSize()
        {
            return _getSize(this.in);
        }
        
        public void close() throws IOException
        {
            _close(this.in, this.out);
            
            this.in = null;
            this.out = null;
        }
        
        @Override
        protected void finalize() throws Throwable
        {
            super.finalize();
        
            if(this.in != null)
            {
                this._close(this.in, this.out);
            }
        }
    }
    
    /* Accessors for constants */
    private static native int _SPLICE_F_MOVE();
    private static native int _SPLICE_F_NONBLOCK();
    private static native int _SPLICE_F_MORE();
    private static native int _SPLICE_F_GIFT();
    
    public native static long splice(FileDescriptor inputFD, long inputOffset, 
            FileDescriptor outputFD, long outputOffset, long length, int flags) 
                    throws IOException;
    
    public native static long tee(FileDescriptor inputFD, 
            FileDescriptor outputFD, long length, int flags) 
                    throws IOException;
    
    public native static FileDescriptor getFileDescriptorFromSocket(Socket socket);
    public native static FileDescriptor getFileDescriptorFromSocketChannel(SocketChannel socket);
    
    public native static Pipe createPipeFileDescriptor() throws IOException;
}
