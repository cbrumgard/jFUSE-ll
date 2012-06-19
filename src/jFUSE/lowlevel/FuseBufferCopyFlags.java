package jFUSE.lowlevel;

public class FuseBufferCopyFlags
{
    /* Constants for controlling how splice is used */
    final public static int NONE                     = 0;
    final public static int FUSE_BUF_NO_SPLICE       = _getFUSE_BUF_NO_SPLICE();
    final public static int FUSE_BUF_FORCE_SPLICE    = _getFUSE_BUF_FORCE_SPLICE();
    final public static int FUSE_BUF_SPLICE_MOVE     = _getFUSE_BUF_SPLICE_MOVE();
    final public static int FUSE_BUF_SPLICE_NONBLOCK = _getFUSE_BUF_SPLICE_NONBLOCK();
    
    /* Private accessor methods that set the constant values above. */
    private static native int _getFUSE_BUF_NO_SPLICE();
    private static native int _getFUSE_BUF_FORCE_SPLICE();
    private static native int _getFUSE_BUF_SPLICE_MOVE();
    private static native int _getFUSE_BUF_SPLICE_NONBLOCK();
}
