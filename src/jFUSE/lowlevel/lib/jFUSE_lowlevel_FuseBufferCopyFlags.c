

#include <stdlib.h>
#include <unistd.h>

#include <jni.h>


#define FUSE_USE_VERSION 29


#include "fuse.h"

/*
 * Class:     jFUSE_lowlevel_FuseBufferCopyFlags
 * Method:    _getFUSE_BUF_NO_SPLICE
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_FuseBufferCopyFlags__1getFUSE_1BUF_1NO_1SPLICE
  (JNIEnv *env, jclass jFuseBufferCopyFlagsClass)
{

#if FUSE_VERSION >= FUSE_MAKE_VERSION(2, 9)
	return FUSE_BUF_NO_SPLICE;
#else
	return 0;
#endif
}

/*
 * Class:     jFUSE_lowlevel_FuseBufferCopyFlags
 * Method:    _getFUSE_BUF_FORCE_SPLICE
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_FuseBufferCopyFlags__1getFUSE_1BUF_1FORCE_1SPLICE
  (JNIEnv *env, jclass jFuseBufferCopyFlagsClass)
{
#if FUSE_VERSION >= FUSE_MAKE_VERSION(2, 9)
	return FUSE_BUF_FORCE_SPLICE;
#else
	return 0;
#endif
}

/*
 * Class:     jFUSE_lowlevel_FuseBufferCopyFlags
 * Method:    _getFUSE_BUF_SPLICE_MOVE
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_FuseBufferCopyFlags__1getFUSE_1BUF_1SPLICE_1MOVE
  (JNIEnv *env, jclass jFuseBufferCopyFlagsClass)
{
#if FUSE_VERSION >= FUSE_MAKE_VERSION(2, 9)
	return FUSE_BUF_SPLICE_MOVE;
#else
	return 0;
#endif

}

/*
 * Class:     jFUSE_lowlevel_FuseBufferCopyFlags
 * Method:    _getFUSE_BUF_SPLICE_NONBLOCK
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_FuseBufferCopyFlags__1getFUSE_1BUF_1SPLICE_1NONBLOCK
  (JNIEnv *env, jclass jFuseBufferCopyFlagsClass)
{
#if FUSE_VERSION >= FUSE_MAKE_VERSION(2, 9)
	return FUSE_BUF_SPLICE_NONBLOCK;
#else
	return 0;
#endif
}


