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

/*
 * jFUSE_lowlevel_ConnInfo.c
 *
 *  Created on: Jan 5, 2011
 *      Author: brumgard
 */



#include <stdlib.h>

#include "jni.h"


#define FUSE_USE_VERSION 28

#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"


#include "fuse_util.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getProtoMajor
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getProtoMajor
  (JNIEnv *env, jobject connInfo)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the proto_minor */
	return fuse_conn_info->proto_major;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getProtoMinor
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getProtoMinor
  (JNIEnv *env, jobject connInfo)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
					(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the proto_minor */
	return fuse_conn_info->proto_minor;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getAsyncRead
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getAsyncRead
  (JNIEnv *env, jobject connInfo)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
					(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the async read */
	return fuse_conn_info->async_read;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    setAsyncRead
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_ConnInfo_setAsyncRead
  (JNIEnv *env, jobject connInfo, jint value)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
					(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}

	/* Sets the async read */
	fuse_conn_info->async_read = value;

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getMaxWrite
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getMaxWrite
  (JNIEnv *env, jobject connInfo)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the max_write */
	return fuse_conn_info->max_write;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;

}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    setMaxWrite
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_ConnInfo_setMaxWrite
  (JNIEnv *env, jobject connInfo, jint value)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}

	/* Sets the max write */
	fuse_conn_info->max_write = value;

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getMaxReadahead
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getMaxReadahead
  (JNIEnv *env, jobject connInfo)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the max read ahead */
	return fuse_conn_info->max_readahead;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    setMaxReadahead
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_ConnInfo_setMaxReadahead
  (JNIEnv *env, jobject connInfo, jint value)
{
	/* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;


	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}

	/* Sets the max read ahead */
	fuse_conn_info->max_readahead = value;

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getCapable
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getCapable
  (JNIEnv *env, jobject connInfo)
{

#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8

    /* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;

	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
				(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the capable */
	return fuse_conn_info->capable;


#else

	/* Throws unsupported operation exception for fuse less than 2.8 */
	jclass class = (*env)->FindClass(env,
								"java.lang.UnsupportedOperationException");

	if(class == NULL)
	{
		return -1;
	}

	(*env)->ThrowNew(env, class, "getCapable not implmented for fuse < 2.8");

	return -1;

#endif

#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8

    /* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
#endif
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    setCapable
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_ConnInfo_setCapable
  (JNIEnv *env, jobject connInfo, jint value)
{

#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8
	
    /* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;

	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}

	/* Sets the capable field */
	fuse_conn_info->capable = value;

	/* Return */
	return;

#else

	/* Throws unsupported operation exception for fuse less than 2.8 */
	jclass class = (*env)->FindClass(env,
								"java.lang.UnsupportedOperationException");

	if(class == NULL)
	{
		return;
	}

	(*env)->ThrowNew(env, class, "setCapable not implmented for fuse < 2.8");

	return;

#endif


#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8

    /* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
#endif
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    getWant
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_ConnInfo_getWant
  (JNIEnv *env, jobject connInfo)
{

#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8
	
    /* Declarations */
	jclass classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_conn_info *fuse_conn_info = NULL;

	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}


	/* Return the want */
	return fuse_conn_info->want;

#else

	/* Throws unsupported operation exception for fuse less than 2.8 */
	jclass class = (*env)->FindClass(env,
								"java.lang.UnsupportedOperationException");

	if(class == NULL)
	{
		return -1;
	}

	(*env)->ThrowNew(env, class, "getWant not implmented for fuse < 2.8");

	return -1;

#endif

#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8

    /* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
#endif
}

/*
 * Class:     jFUSE_lowlevel_ConnInfo
 * Method:    setWant
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_ConnInfo_setWant
  (JNIEnv *env, jobject connInfo, jint value)
{


#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8
	
    /* Declarations */
	struct fuse_conn_info *fuse_conn_info = NULL;
	jclass classID = NULL;
	jfieldID fieldID = NULL;

	/* Gets the ID of the class and field */
	if(((classID = (*env)->GetObjectClass(env, connInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, classID,
			   	   	   	   	   	   	   "fuse_conn_info_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Gets the value of the field */
	if((fuse_conn_info = (struct fuse_conn_info*)
			(*env)->GetLongField(env, connInfo, fieldID)) == NULL)
	{
		throw_FuseException(env, "Invalid value for ConnInfo.fuse_conn_info_ptr");
		goto ERROR_HANDLER;
	}

	/* Sets the want */
	fuse_conn_info->want = value;

	/* Return */
	return;

#else

	/* Throws unsupported operation exception for fuse less than 2.8 */
	jclass class = (*env)->FindClass(env,
								"java.lang.UnsupportedOperationException");

	if(class == NULL)
	{
		return;
	}

	(*env)->ThrowNew(env, class, "getWant not implmented for fuse < 2.8");

	return;

#endif


#if FUSE_VERSION_MAJOR >= 2 && FUSE_MAJOR_VERSION >= 8
	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
#endif
}

#ifdef __cplusplus
}
#endif
