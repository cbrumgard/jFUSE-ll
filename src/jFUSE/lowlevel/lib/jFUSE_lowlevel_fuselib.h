/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class jFUSE_lowlevel_fuselib */

#ifndef _Included_jFUSE_lowlevel_fuselib
#define _Included_jFUSE_lowlevel_fuselib
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     jFUSE_lowlevel_fuselib
 * Method:    getFuseConstants
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_fuselib_getFuseConstants
  (JNIEnv *, jclass, jstring);

/*
 * Class:     jFUSE_lowlevel_fuselib
 * Method:    mount
 * Signature: (LjFUSE/lowlevel/JFuseFS;Ljava/lang/String;LjFUSE/lowlevel/FuseArgs;Z)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_fuselib_mount
  (JNIEnv *, jclass, jobject, jstring, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
