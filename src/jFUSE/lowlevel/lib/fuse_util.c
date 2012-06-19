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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>

#include "jni.h"


#define FUSE_USE_VERSION 28


#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"



void throw_FuseException(JNIEnv *env, const char *msg)
{
	/* Declarations */
	jclass cls = (*env)->FindClass(env, "jFUSE/lowlevel/FuseException");

    /* Throw the exception */
    if(cls != NULL)
    {
    	(*env)->ThrowNew(env, cls, msg);
    }
}


fuse_req_t getFuseRequest(JNIEnv *env, jobject fuseRequest)
{
	/* Declarations */
	jclass	class  	 = NULL;
	jfieldID fieldID = NULL;


	/* Gets the "get" method for the FuseRequest so the pointer address can
	 * obtained */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseRequest")) == NULL) ||
			((fieldID = (*env)->GetFieldID(env, class, "cfuse_req", "J")) == NULL))
	{
		// Throws an exception
		return NULL;
	}

	/* Gets the native fuse_req_t */
	return (fuse_req_t)(*env)->GetLongField(env, fuseRequest, fieldID);
}


int convertFuseStat(JNIEnv *env, jobject fuseStat, struct stat *statbuf)
{
	/* Declarations */
	jclass class;
	jclass fuseStatModeClass;
	jfieldID st_inoField;
	jfieldID st_sizeField;
	jfieldID st_modeField;
	jfieldID st_modeModeField;
	jfieldID st_atimeField;
	jfieldID st_mtimeField;
	jfieldID st_ctimeField;
	jobject  fuseStatMode;


	/* Finds the class for stat */
	if((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseStat")) == NULL)
	{
		/* Throws an exception */
		(*env)->ExceptionDescribe(env);
		return -1;
	}

	/* Gets the field ids for the fuseStat */
	st_inoField = (*env)->GetFieldID(env, class, "inode", "J");
	st_sizeField = (*env)->GetFieldID(env, class, "size", "J");
	st_modeField = (*env)->GetFieldID(env, class, "mode", "LjFUSE/lowlevel/Mode;");
	st_atimeField = (*env)->GetFieldID(env, class, "atime", "J");
	st_mtimeField = (*env)->GetFieldID(env, class, "mtime", "J");
	st_ctimeField = (*env)->GetFieldID(env, class, "ctime", "J");



	if(st_inoField == NULL || st_sizeField == NULL || st_modeField == NULL ||
       st_atimeField == NULL || st_mtimeField == NULL || st_ctimeField == NULL)
	{
		(*env)->ExceptionDescribe(env);
		return -1;
	}


	if(((fuseStatMode = (*env)->GetObjectField(env, fuseStat, st_modeField)) == NULL) ||
	   ((fuseStatModeClass = (*env)->GetObjectClass(env, fuseStatMode)) == NULL) ||
	   ((st_modeModeField = (*env)->GetFieldID(env, fuseStatModeClass, "mode", "J")) == NULL))
	{
		return -1;
	}

	/* Initialize the statbuf */
	memset(statbuf, 0, sizeof(struct stat));

	statbuf->st_ino  = (*env)->GetLongField(env, fuseStat, st_inoField);
	statbuf->st_size = (*env)->GetLongField(env, fuseStat, st_sizeField);

	statbuf->st_mode = (mode_t)(*env)->GetLongField(env, fuseStatMode, st_modeModeField);

	statbuf->st_atime = (*env)->GetLongField(env, fuseStat, st_atimeField);
	statbuf->st_mtime = (*env)->GetLongField(env, fuseStat, st_mtimeField);
	statbuf->st_ctime = (*env)->GetLongField(env, fuseStat, st_ctimeField);

	statbuf->st_blksize = (32 * getpagesize()) - 16;

	statbuf->st_uid = getuid();
	statbuf->st_gid = getgid();

	/* Return success */
	return 0;
}





jobject new_fuse_request(JNIEnv *env, fuse_req_t req)
{
	/* Declarations */
	jobject jrequest  = NULL;
	jclass requestClass;
	jmethodID requestID;


	/* Request */
	if((requestClass = (*env)->FindClass(env, "jFUSE/lowlevel/FuseRequest")) == NULL)
	{
		return NULL;
	}

	if((requestID = (*env)->GetMethodID(env, requestClass, "<init>", "(J)V")) == NULL)
	{
		return NULL;
	}

	if((jrequest = (*env)->NewObject(env, requestClass, requestID, (jlong)req)) == NULL)
	{
		return NULL;
	}

	/* Returns the request */
	return jrequest;
}



jobject new_fuse_stat(JNIEnv *env, struct stat *statbuf)
{
	/* Declarations */
	jobject fuseStat = NULL;
	jclass  classID  = NULL;
	jmethodID methodID = NULL;
	jlong     atime;
	jlong     mtime;
	jlong     ctime;


	/* Gets the class for FuseStat */
	classID = (*env)->FindClass(env, "jFUSE/lowlevel/FuseStat");

	if(classID == NULL)
	{
		return NULL;
	}

	/* Gets the constructor method */
	methodID = (*env)->GetMethodID(env, classID, "<init>", "(JJJ)V");

	if(methodID == NULL)
	{
		return NULL;
	}

	/* Time */
	atime = statbuf->st_atime;
	mtime = statbuf->st_mtime;
	ctime = statbuf->st_ctime;

	/* Allocates the class or returns NULL for failure */
	fuseStat = (*env)->NewObject(env, classID, methodID,
						(jlong)(statbuf->st_ino), (jlong)(statbuf->st_size),
						(jlong)(statbuf->st_mode), atime, mtime, ctime);


	/* Returns the fuse Stat */
	return fuseStat;
}


int convert_to_native_fuse_param(JNIEnv *env, jobject fuseEntryParam,
								struct fuse_entry_param *param)
{
	/* Declarations */
	jclass	   class  = NULL;
	jobject  fuseStat = NULL;
	jfieldID inoField;
	jfieldID generationField;
	jfieldID attrField;
	jfieldID attr_timeoutField;
	jfieldID entry_timeoutField;
	jfieldID st_inoField;
	jfieldID st_sizeField;


	/* Fill in param */
	if((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseEntryParam")) == NULL)
	{
		/* Throws an exception */
		return -1;
	}

	/* Gets the field ids of the FuseEntryParam */
	inoField     	   = (*env)->GetFieldID(env, class, "ino", "J");
	generationField    = (*env)->GetFieldID(env, class, "generation", "J");
	attrField		   = (*env)->GetFieldID(env, class, "attr", "LjFUSE/lowlevel/FuseStat;");
	attr_timeoutField  = (*env)->GetFieldID(env, class, "attr_timeout", "D");
	entry_timeoutField = (*env)->GetFieldID(env, class, "entry_timeout", "D");

	if(inoField == NULL || generationField == NULL || attrField == NULL ||
	   attr_timeoutField == NULL || entry_timeoutField == NULL)
	{
		/* handle error */
		throw_FuseException(env, "fuse_reply_entry error");
		return -1;
	}




	/* Gets the fuse stat */
	if((fuseStat = (*env)->GetObjectField(env, fuseEntryParam, attrField)) == NULL)
	{
		/* Throws an exception */
		return -1;
	}

	/* Finds the class for stat */
	if((class = (*env)->GetObjectClass(env, fuseStat)) == NULL)
				//FindClass(env, "jFUSE/lowlevel/FuseStat")) == NULL)
	{
		/* Throws an exception */
		return -1;
	}

	/* Gets the field ids for the fuseStat */
	st_inoField = (*env)->GetFieldID(env, class, "inode", "J");
	st_sizeField = (*env)->GetFieldID(env, class, "size", "J");

	if(st_inoField == NULL || st_sizeField == NULL)
	{
		/* handle error */
		throw_FuseException(env, "fuse_reply_entry error");
		return -1;
	}


	/* Initializes the return param */
	memset(param, 0, sizeof(param));

	/* Fills in the param statbuf */
	if(convertFuseStat(env, fuseStat, &param->attr) != 0)
	{
		return -1;
	}


	/* Fills in the param */
	param->ino =  (*env)->GetLongField(env, fuseEntryParam, inoField);
	param->generation = (*env)->GetLongField(env, fuseEntryParam, generationField);
	param->attr_timeout = (*env)->GetDoubleField(env, fuseEntryParam, attr_timeoutField);
	param->entry_timeout = (*env)->GetDoubleField(env, fuseEntryParam, entry_timeoutField);

	/* Success */
	return 0;
}


jobject new_fuse_file_info(JNIEnv *env, struct fuse_file_info *fi)
{
	/* Declarations */
	jclass class = NULL;
	jmethodID constructorID = NULL;


	/* Gets the constructor for FileInfo */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FileInfo")) == NULL) ||
	    (constructorID = (*env)->GetMethodID(env, class, "<init>", "(J)V")) == NULL)
	{
		return NULL;
	}

	/* Allocates the object passing the pointer to the struct */
	return (*env)->NewObject(env, class, constructorID, (jlong)fi);
}

struct fuse_file_info *convert_to_native_fuse_file_info(
		JNIEnv *env, jobject fuseInfo)
{
	/* Declarations */
	jclass class 		= NULL;
	jfieldID fieldID 	= NULL;



	if(((class = (*env)->GetObjectClass(env, fuseInfo)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, class,
			   	   	   	   	   	   	   "fuse_file_info_ptr", "J")) == NULL))
	{
		return NULL;
	}


	return (struct fuse_file_info *)(*env)->GetLongField(env, fuseInfo, fieldID);
}


jobject new_fuse_flock(JNIEnv *env, struct flock *flock)
{
	/* Declarations */
	jclass class = NULL;
	jmethodID constructorID = NULL;


	/* Gets the constructor for FileInfo */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FLock")) == NULL) ||
			(constructorID = (*env)->GetMethodID(env, class, "<init>", "(J)V")) == NULL)
	{
		return NULL;
	}

	/* Allocates the object passing the pointer to the struct */
	return (*env)->NewObject(env, class, constructorID, (jlong)flock);
}


struct flock *convert_to_native_flock(JNIEnv *env, jobject flock)
{
	/* Declarations */
	jclass class 		= NULL;
	jfieldID fieldID 	= NULL;



	if(((class = (*env)->GetObjectClass(env, flock)) == NULL) ||
			((fieldID = (*env)->GetFieldID(env, class,
					"_flock_ptr", "J")) == NULL))
	{
		return NULL;
	}


	return (struct flock *)(*env)->GetLongField(env, flock, fieldID);
}


jobject make_byte_array_for_buffer(JNIEnv *env, void *buf,  jlong size)
{
	/* Declarations */
	jobject  byteBuffer = NULL;




	if((byteBuffer = (*env)->NewDirectByteBuffer(env, buf, (jlong)size)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Return */
	return byteBuffer;


	/* Error Handler */
	ERROR_HANDLER:

		if(byteBuffer != NULL)
		{
			(*env)->DeleteLocalRef(env, byteBuffer);
		}

		return NULL;
}


