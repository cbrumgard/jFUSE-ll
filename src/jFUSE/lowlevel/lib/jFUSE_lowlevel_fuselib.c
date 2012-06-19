/*
 * jFUSE_lowlevel_fuselib.c
 *
 *  Created on: Jan 20, 2011
 *      Author: brumgard
 */




#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <assert.h>
#include <jni.h>



#define FUSE_USE_VERSION 28


#define JFUSE_VERSION	"0.6"


#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"


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
  (JNIEnv *env, jclass obj, jstring constant_str)
{
	/* Declarations */
	jlong val = 0;
	char *cstring = NULL;
	jboolean isCopy = JNI_FALSE;


	/* Gets the native string for the constant */
	cstring = (char*)(*env)->GetStringUTFChars(env, constant_str, &isCopy);

	if(cstring == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Finds the matching fuse constant */
	if(strcmp(cstring,"FUSE_SET_ATTR_MODE") == 0)
	{
		val = FUSE_SET_ATTR_MODE;
	}else if(strcmp(cstring, "FUSE_SET_ATTR_UID") == 0)
	{
		val = FUSE_SET_ATTR_UID;
	}else if(strcmp(cstring, "FUSE_SET_ATTR_GID") == 0)
	{
		val = FUSE_SET_ATTR_GID;
	}else if(strcmp(cstring, "FUSE_SET_ATTR_SIZE") == 0)
	{
		val = FUSE_SET_ATTR_SIZE;
	}else if(strcmp(cstring, "FUSE_SET_ATTR_ATIME") == 0)
	{
		val = FUSE_SET_ATTR_ATIME;
	}else if(strcmp(cstring, "FUSE_SET_ATTR_MTIME") == 0)
	{
		val = FUSE_SET_ATTR_MTIME;

	}else if(strcmp(cstring, "O_ACCMODE") == 0)
	{
		val = O_ACCMODE;
    }else if(strcmp(cstring, "O_RDONLY") == 0)
	{
		val = O_RDONLY;
    }else if(strcmp(cstring, "O_WRONLY") == 0)
	{
		val = O_WRONLY;
    }else if(strcmp(cstring, "O_RDWR") == 0)
	{
		val = O_RDWR;
    }else if(strcmp(cstring, "O_APPEND") == 0)
	{
		val = O_APPEND;
    }else if(strcmp(cstring, "O_ASYNC") == 0)
	{
		val = O_ASYNC;
    }else if(strcmp(cstring, "O_CREAT") == 0)
	{
		val = O_CREAT;
    }else if(strcmp(cstring, "O_EXCL") == 0)
	{
		val = O_EXCL;
    }else if(strcmp(cstring, "O_NOCTTY") == 0)
	{
		val = O_NOCTTY;
    /*}else if(strcmp(cstring, "O_NOFOLLOW") == 0)
	{
		val = O_NOFOLLOW; */
    }else if(strcmp(cstring, "O_NONBLOCK") == 0)
	{
		val = O_NONBLOCK;
    }else if(strcmp(cstring, "O_NDELAY") == 0)
	{
		val = O_NDELAY;
    }else if(strcmp(cstring, "O_SYNC") == 0)
	{
		val = O_SYNC;
    }else if(strcmp(cstring, "O_TRUNC") == 0)
	{
		val = O_TRUNC;
	}else if(strcmp(cstring, "XATTR_CREATE") == 0)
	{
		val = XATTR_CREATE;
	}else if(strcmp(cstring, "XATTR_REPLACE") == 0)
	{
		val = XATTR_REPLACE;
	}else
	{
		assert("Unknown constant str");
	}



	/* Releases the native string */
	(*env)->ReleaseStringUTFChars(env, constant_str, cstring);

	/* Return val */
	return val;


	/* Error handler */
	ERROR_HANDLER:

		/* Frees any resources */
		if(cstring != NULL)
		{
			(*env)->ReleaseStringUTFChars(env, constant_str, cstring);
		}

		/* Print the exception */
		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		/* Return */
		return 0;
}

/*
 * Class:     jFUSE_lowlevel_fuselib
 * Method:    getFuseVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_fuselib_getFuseVersion
  (JNIEnv *env, jclass cls)
{
	return (jint)FUSE_VERSION;
}




/*
 * Class:     jFUSE_lowlevel_fuselib
 * Method:    getJFuseVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jFUSE_lowlevel_fuselib_getJFuseVersion
  (JNIEnv *env, jclass cls)
{
	/* Creates a java string for the version and returns it */
	return (*env)->NewStringUTF(env, JFUSE_VERSION);
}


#ifdef __cplusplus
}
#endif
#endif






