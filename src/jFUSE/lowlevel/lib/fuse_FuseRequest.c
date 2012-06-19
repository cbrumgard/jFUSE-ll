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
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <errno.h>

#include "jni.h"



#define FUSE_USE_VERSION 28

#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"

#include "jFUSE_lowlevel_fuselib.h"
#include "fuse_util.h"
#include "fuse_errno.h"
#include "config.h"

/*
 * Class:     fuse_FuseRequest
 * Method:    reply_none
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1none
  (JNIEnv *env, jobject fuseRequest)
{
	/* Declarations */
	fuse_req_t req 	 = NULL;


	/* Get the fuse_req_t */
	req = getFuseRequest(env, fuseRequest);

	if(req == NULL)
	{
		/* Throws an exception */
		return;
	}

	/* Notify fuse of being done */
	fuse_reply_none(req);

	/* Return */
	return;
}

/*
 * Class:     fuse_FuseRequest
 * Method:    reply_err
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1err
  (JNIEnv *env, jobject fuseRequest, jint fuseErrNum)
{
	/* Declarations */
	fuse_req_t req 	 = NULL;


	/* Get the fuse_req_t */
	req = getFuseRequest(env, fuseRequest);

	if(req == NULL)
	{
		/* Throws an exception */
		return;
	}

	/* Replies to with the answer */
	if(fuse_reply_err(req, errno_convert(fuseErrNum)) != 0)
	{
		/* Throw exception if there is an error */
		throw_FuseException(env, "fuse_reply_err failed");
	}

	/* Return */
	return;
}

/*
 * Class:     fuse_FuseRequest
 * Method:    reply_entry
 * Signature: (LjFUSE/lowlevel/FuseEntryParam;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1entry
  (JNIEnv *env, jobject fuseRequest, jobject fuseEntryParam)
{
	/* Declarations */
	fuse_req_t req 	 = NULL;
	struct fuse_entry_param param;
	jclass	   class  = NULL;
	jfieldID cfuse_reqField;
	jfieldID inoField;
	jfieldID generationField;
	jfieldID attrField;
	jfieldID attr_timeoutField;
	jfieldID entry_timeoutField;
	jobject  fuseStat;
	jfieldID st_inoField;
	jfieldID st_sizeField;


	/* Gets the "get" method for the FuseRequest so the pointer address can
	 * obtained */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseRequest")) == NULL) ||
	   ((cfuse_reqField = (*env)->GetFieldID(env, class, "cfuse_req", "J")) == NULL))
	{
		/* Throws an exception */
		return;
	}

	/* Gets the native fuse_req_t */
	req = (fuse_req_t)(*env)->GetLongField(env, fuseRequest, cfuse_reqField);


	/* Fill in param */
	if((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseEntryParam")) == NULL)
	{
		/* Throws an exception */
		return;
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
		return;
	}

	/* Finds the class for stat */
	if((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseStat")) == NULL)
	{
		/* Throws an exception */
		return;
	}

	/* Gets the fuse stat */
	if((fuseStat = (*env)->GetObjectField(env, fuseEntryParam, attrField)) == NULL)
	{
		/* Throws an exception */
		return;
	}

	/* Gets the field ids for the fuseStat */
	st_inoField = (*env)->GetFieldID(env, class, "inode", "J");
	st_sizeField = (*env)->GetFieldID(env, class, "size", "J");

	if(st_inoField == NULL || st_sizeField == NULL)
	{
		/* handle error */
		throw_FuseException(env, "fuse_reply_entry error");
		return;
	}


	/* Initializes the return param */
	memset(&param, 0, sizeof(param));


	/* Fills in the param */
	param.ino =  (*env)->GetLongField(env, fuseEntryParam, inoField);
	param.generation = (*env)->GetLongField(env, fuseEntryParam, generationField);
	param.attr_timeout = (*env)->GetDoubleField(env, fuseEntryParam, attr_timeoutField);
	param.entry_timeout = (*env)->GetDoubleField(env, fuseEntryParam, entry_timeoutField);

	/* Fills in the stat */
	if(convertFuseStat(env, fuseStat, &(param.attr)) != 0)
	{
		/* handle error */
		throw_FuseException(env, "fuse_reply_entry error");
		return;
	}


	/* Send the fuse reply */
	if(fuse_reply_entry(req, &param) != 0)
	{
		/* throw an error */
		throw_FuseException(env, "fuse_reply_entry error");
		return;
	}


	/* Return */
	return;
}

/*
 * Class:     fuse_FuseRequest
 * Method:    reply_attr
 * Signature: (LjFUSE/lowlevel/FuseStat;D)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1attr
  (JNIEnv *env, jobject fuseRequest, jobject fuseStat, jdouble timeout)
{
	/* Declarations */
	fuse_req_t request 		= NULL;
	struct stat statbuf;


	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);

	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_attr error");
		return;
	}

	/* Gets a statbuf */
	if(convertFuseStat(env, fuseStat, &statbuf) != 0)
	{
		throw_FuseException(env, "fuse_reply_attr error");
		return;
	}


	/* Sends the reply */
	if(fuse_reply_attr(request, &statbuf, (double)timeout) != 0)
	{
		throw_FuseException(env, "fuse_reply_attr error");
		return;
	}

	/* Returns */
	return;
}

/*
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    reply_open
 * Signature: (LjFUSE/lowlevel/FileInfo;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1open
  (JNIEnv *env, jobject fuseRequest, jobject fuseInfo)
{
	/* Declarations */
	fuse_req_t request 		= NULL;
	struct fuse_file_info *fi;


	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);

	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_open error");
		return;
	}

	/* TODO need to implement fuse_file_ino passing */
	if((fi = convert_to_native_fuse_file_info(env, fuseInfo)) == NULL)
	{
		return;
	}

	/* Send back the response */
	if(fuse_reply_open(request, fi) != 0)
	{
		throw_FuseException(env, "fuse_reply_open error");
		return;
	}

	/* Return */
	return;
}


/*
 * Class:     fuse_FuseRequest
 * Method:    reply_write
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1write
  (JNIEnv *env, jobject fuseRequest, jlong size)
{
	/* Declarations */
	fuse_req_t	request = NULL;


	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);

	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_write error");
		return;
	}

	if(fuse_reply_write(request, size) != 0)
	{
		throw_FuseException(env, "fuse_reply_write error");
		return;
	}
}

/*
 * Class:     fuse_FuseRequest
 * Method:    reply_buf
 * Signature: ([BJ)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1buf
  (JNIEnv *env, jobject fuseRequest, jbyteArray buf, jlong size)
{
	/* Declarations */
	fuse_req_t request = NULL;
	jbyte *    jbuf    = NULL;



	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);

	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_buf error");
		return;
	}


	/* Gets access to the buffer */
	if((jbuf = (*env)->GetByteArrayElements(env, buf, NULL)) == NULL)
	{
		throw_FuseException(env, "fuse_reply_buf error");
		return;
	}


	/* Sends the data back to fuse */
	if(fuse_reply_buf(request, (char *)jbuf, (size_t)size) != 0)
	{
		throw_FuseException(env, "fuse_reply_buf error");
		goto ERROR_HANDLER;
	}

	/* Releases the  native buffer */
	(*env)->ReleaseByteArrayElements(env, buf, jbuf, JNI_ABORT);


	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:


		/* Free the buffer */
		if(jbuf != NULL)
		{
			(*env)->ReleaseByteArrayElements(env, buf, jbuf, JNI_ABORT);
		}

		/* Return */
		return;
}


/*
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    reply_mbuf
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1mbuf
  (JNIEnv *env, jobject fuseRequest, jobject jbuf)
{
	/* Declarations */
	fuse_req_t request = NULL;
	void *     buf	   = NULL;
	jclass     class   = NULL;
	jmethodID  mid     = NULL;
	size_t     bufsize = 0;


	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);

	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_buf error");
		return;
	}

	if((class = (*env)->FindClass(env, "java/nio/Buffer")) == NULL ||
	   ((mid = (*env)->GetMethodID(env, class, "limit", "()I")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the length of the buffer to use */
	bufsize = (*env)->CallIntMethod(env, jbuf, mid);

	/* Gets access to the direct buffer */
	if(bufsize > 0)
	{
		buf = (*env)->GetDirectBufferAddress(env, jbuf);

		if(buf == NULL)
		{
			throw_FuseException(env, "fuse_reply_buf error");
			return;
		}
	}

	/* Sends the data back to fuse */
	if(fuse_reply_buf(request, (char *)buf, bufsize) != 0)
	{
		throw_FuseException(env, "fuse_reply_buf error");
		goto ERROR_HANDLER;
	}

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Return */
		return;
}

/*
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    reply_iov
 * Signature: ([Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1iov
  (JNIEnv *env, jobject fuseRequest, jobjectArray jbufArray)
{
	/* Declarations */
	fuse_req_t 	  request = NULL;
	struct iovec *iovec   = NULL;
	jobject	      jbuf    = NULL;
	jclass 	      class = NULL;
	jmethodID     mid = NULL;
	jsize 		  i=0,
				  n=0;

	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);

	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_iov error");
		return;
	}


	jsize count = 0;

	/* Gets the size of array */
	count = (*env)->GetArrayLength(env, jbufArray);

	/* Allocates an array of iovec structs */
	iovec = malloc((count+1) * sizeof(struct iovec));

	if(iovec == NULL)
	{
		(*env)->ThrowNew(env,
						 (*env)->FindClass(env, "java/lang/OutOfMemoryError"),
						 "Malloc allocation failure");
		goto ERROR_HANDLER;
	}



	if((class = (*env)->FindClass(env, "java/nio/Buffer")) == NULL ||
	   ((mid = (*env)->GetMethodID(env, class, "limit", "()I")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Builds the array of iovec structs */
	for(i=0, n=0; i<count; i++)
	{
		/* Gets the current buffer in the list */
		jbuf = (*env)->GetObjectArrayElement(env, jbufArray, i);

		if(jbuf == NULL)
		{
			continue;
		}

		/* Gets the length of the buffer to use */
		iovec[n].iov_len = (*env)->CallIntMethod(env, jbuf, mid);

		//iovec[n].iov_len = (size_t)(*env)->GetDirectBufferCapacity(env, jbuf);

		if(iovec[n].iov_len > 0)
		{
			/* Gets access to the direct buffer */
			iovec[n].iov_base = (*env)->GetDirectBufferAddress(env, jbuf);

			if(iovec[n].iov_base  == NULL)
			{
				throw_FuseException(env, "fuse_reply_ivo error");
				goto ERROR_HANDLER;
			}

			/* Increments the index for iovec */
			n++;
		}
	}

	/* Sends the reply back to fuse */
	if(fuse_reply_iov(request, iovec, n) != 0)
	{
		throw_FuseException(env, "fuse_reply_ivo error");
		goto ERROR_HANDLER;
	}


	/* Frees the iovec */
	free(iovec);


	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		if(iovec != NULL)
		{
			free(iovec);
		}

		/* Return */
		return;
}


/*
 * Class:     fuse_FuseRequest
 * Method:    reply_create
 * Signature: (LjFUSE/lowlevel/FuseEntryParam;LjFUSE/lowlevel/FileInfo;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1create
  (JNIEnv *env, jobject fuseRequest, jobject fuseEntryParam, jobject fuseInfo)
{
	/* Declarations */
	fuse_req_t request = NULL;
	struct fuse_entry_param param;
	struct fuse_file_info *fi;


	/* Gets the fuse_req_t struct */
	request = getFuseRequest(env, fuseRequest);


	if(request == NULL)
	{
		throw_FuseException(env, "fuse_reply_create error");
		return;
	}

	if(convert_to_native_fuse_param(env, fuseEntryParam, &param) != 0)
	{
		throw_FuseException(env, "fuse_reply_create error");
		return;
	}

	/* TODO implment fuse_file_info stuff */
	if((fi = convert_to_native_fuse_file_info(env, fuseInfo)) == NULL)
	{
		return;
	}

	/* Send reply */
	if(fuse_reply_create(request, &param, fi) != 0)
	{
		throw_FuseException(env, "fuse_reply_create error");
		goto ERROR_HANDLER;
	}

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
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    reply_lock
 * Signature: (LjFUSE/lowlevel/FLock;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1lock
  (JNIEnv *env, jobject jfuseRequest, jobject jflock)
{
	/* Declarations */
	fuse_req_t fuse_req 	 = NULL;
	struct flock *flock            = NULL;


	/* Gets the internal pointer to the fuse request */
	if((fuse_req = getFuseRequest(env, jfuseRequest)) == NULL)
	{
		return;
	}

	/* Gets the internal pointer to the flock  */
	flock = convert_to_native_flock(env, jflock);

	if(flock == NULL)
	{
		return;
	}

	printf("HERE1\n");
	/* Send back the reply */
	if(fuse_reply_lock(fuse_req, flock) != 0)
	{
		throw_FuseException(env, "fuse_reply_lock error");
		return;
	}
	printf("HERE2\n");

	/* Return */
	return;

}


/*
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    reply_xattr
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1xattr
  (JNIEnv *env, jobject jfuseRequest, jlong size)
{
	/* Declarations */
	fuse_req_t fuse_req 	 = NULL;

	/* Gets the internal pointer to the fuse request */
	if((fuse_req = getFuseRequest(env, jfuseRequest)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Send back the reply */
	if(fuse_reply_xattr(fuse_req, size) != 0)
	{
		throw_FuseException(env, "fuse_reply_xattr error");
		goto ERROR_HANDLER;
	}


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
 * Class:     fuse_FuseRequest
 * Method:    getCTX
 * Signature: ()LjFUSE/lowlevel/FuseCTX;
 */
JNIEXPORT jobject JNICALL Java_jFUSE_lowlevel_FuseRequest_getCTX
	(JNIEnv *env, jobject jfuseRequest)
{
	/* Declarations */
	jclass	class 		      = NULL;
	jmethodID methodID		  = NULL;
	fuse_req_t fuse_req 	 = NULL;
	struct fuse_ctx *fuse_ctx = NULL;


	/* Gets the internal pointer to the fuse request */
	if((fuse_req = getFuseRequest(env, jfuseRequest)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the fuse ctx */
	if((fuse_ctx = (struct fuse_ctx*)fuse_req_ctx(fuse_req)) == NULL)
	{
		throw_FuseException(env, "Error getting fuse_ctx");
		goto ERROR_HANDLER;
	}

	/* Gets the constructor for the FuseCTX class */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FuseCTX")) == NULL) ||
	   ((methodID = (*env)->GetMethodID(env, class, "<init>", "(J)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Allocates a fuse ctx object and returns it */
	return (*env)->NewObject(env, class, methodID, (jlong)fuse_ctx);


	/* error handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return NULL;

}



/*
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    add_direntry
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;LjFUSE/lowlevel/FuseStat;J)Z
 */
JNIEXPORT jboolean JNICALL Java_jFUSE_lowlevel_FuseRequest_add_1direntry
 	 (JNIEnv *env, jobject jfuseRequest, jobject jbuf, jstring jname,
 	  jobject fuseStat, jlong off)
{
	/* Declarations */
	jclass class = NULL;
	jmethodID getPositionID = NULL;
	jmethodID setPositionID = NULL;
	struct stat statbuf;
	jbyte     *buf     	= NULL;
	jlong     capacity  = 0;
	char      *name    	= NULL;
	fuse_req_t fuse_req = NULL;
	jint	  position  = 0;
	jlong     retSize   = 0;
    jboolean  isCopy    = JNI_FALSE;


	/* Gets the internal pointer to the fuse request */
	if((fuse_req = getFuseRequest(env, jfuseRequest)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the c buffer and size */
	buf      = (*env)->GetDirectBufferAddress(env, jbuf);
	capacity = (*env)->GetDirectBufferCapacity(env, jbuf);

	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the cstring for the name */
	if((name = (char *)(*env)->GetStringUTFChars(env, jname, &isCopy)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets a statbuf */
	if(convertFuseStat(env, fuseStat, &statbuf) != 0)
	{
		throw_FuseException(env, "fuse_reply_attr error");
		goto ERROR_HANDLER;
	}

	/* Gets the position method ids */
	if(((class = (*env)->GetObjectClass(env, jbuf)) == NULL) ||
	   ((getPositionID = (*env)->GetMethodID(env, class, "position",
			   	   	   	   	   	   	   	   "()I")) == NULL) ||
	   ((setPositionID = (*env)->GetMethodID(env, class, "position",
			   	   	   	   	   	   	   	   "(I)Ljava/nio/Buffer;")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the current position */
	position = (*env)->CallIntMethod(env, jbuf, getPositionID);

	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Adds the directory */
	retSize = fuse_add_direntry(fuse_req, (char*)buf+position, capacity - position,
								name, &statbuf, off);

	/* Free the string */
	if(isCopy == JNI_TRUE)
	{
		(*env)->ReleaseStringUTFChars(env, jname, name);
	}

	/* Updates the position of the buffer and returns true if there was
	 * enough room in the buffer */
	if(position+retSize < capacity)
	{
		(*env)->CallVoidMethod(env, jbuf, setPositionID, (jint)(position+retSize));

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			goto ERROR_HANDLER;
		}

		return JNI_TRUE;

	/* Failed to add the new entry */
	}else
	{
		return JNI_FALSE;
	}

	/* Error handler */
	ERROR_HANDLER:

		/* Print info about the exception */
		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		/* Free the string */
		if(name != NULL && isCopy == JNI_TRUE)
		{
			(*env)->ReleaseStringUTFChars(env, jname, name);
		}

		/* Failure */
		return JNI_FALSE;
}



/*
 * Class:     jFUSE_lowlevel_FuseRequest
 * Method:    reply_data
 * Signature: (Ljava/util/List;I)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseRequest_reply_1data
  (JNIEnv *env, jobject jfuseRequest, jobject jfuseBufferList, jint fuseBufferCopyFlags)
{
/* No linux so throw NotSupportException exception */
#if __LINUX__ == 0 || FUSE_VERSION < FUSE_MAKE_VERSION(2, 9)
		(*env)->ThrowNew(env,
				(*env)->FindClass(env, "java/lang/UnsupportedOperationException"),
				"fuse_reply_data is not supported on this platform");
		return;
#else

	/* Declarations */
	fuse_req_t fuse_req = NULL;
	struct fuse_bufvec *fuse_bufvec = NULL;
	jint listSize;
	jmethodID listGetMethod = NULL;
	jobject jfuseBuffer = NULL;
	jint i;

	/* Checks the args */
	if(jfuseBufferList == NULL)
	{
		(*env)->ThrowNew(env,
				         (*env)->FindClass(env, "java/lang/NullPointerException"),
				         "null argument to reply_data method");
		return;
	}

	/* Gets the internal pointer to the fuse request */
	if((fuse_req = getFuseRequest(env, jfuseRequest)) == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the size of the list */
	listSize = (*env)->CallIntMethod(env, jfuseBufferList,
					(*env)->GetMethodID(env,
							(*env)->GetObjectClass(env, jfuseBufferList),
							"size", "()I"));

	if((*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the get method for the list object */
	listGetMethod = (*env)->GetMethodID(env,
						(*env)->GetObjectClass(env, jfuseBufferList),
						"get", "(I)Ljava/lang/Object;");

	if(listGetMethod == NULL || (*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}



	/* Allocates space for the fuse bufvec */
	fuse_bufvec = calloc(1, sizeof(struct fuse_bufvec)+((listSize-1)*sizeof(struct fuse_buf)));

	if(fuse_bufvec == NULL)
	{
		(*env)->ThrowNew(env,
						 (*env)->FindClass(env, "java/lang/OutOfMemoryError"),
						 "Malloc allocation failure");
		goto ERROR_HANDLER;
	}

	/* Iterates over the list */
	for(i=0; i<listSize; i++)
	{
		/* Gets the current fuse buffer */
		jfuseBuffer = (*env)->CallObjectMethod(env, jfuseBufferList, listGetMethod, i);

		printf("jfuseBuffer = %p\n", jfuseBuffer);

		if(jfuseBuffer == NULL || (*env)->ExceptionCheck(env))
		{
			goto ERROR_HANDLER;
		}

		/* Gets the contents of the jfusebuffer */
		jclass jfuseBufferClass = (*env)->GetObjectClass(env, jfuseBuffer);
		jlong size = (*env)->GetLongField(env, jfuseBuffer,
								(*env)->GetFieldID(env, jfuseBufferClass,
										"size", "J"));
		jobject jbuffer = (*env)->GetObjectField(env, jfuseBuffer,
								(*env)->GetFieldID(env, jfuseBufferClass,
										"buffer", "Ljava/nio/ByteBuffer;"));

		jobject jfileDescriptor = (*env)->GetObjectField(env, jfuseBuffer,
						  (*env)->GetFieldID(env, jfuseBufferClass,
								"fileDescriptor", "Ljava/io/FileDescriptor;"));

		jlong filePos = (*env)->GetLongField(env, jfuseBuffer,
									(*env)->GetFieldID(env, jfuseBufferClass,
											"filePos", "J"));

		/* Checks for errors */
		if((*env)->ExceptionCheck(env))
		{
			goto ERROR_HANDLER;
		}


		/* Sets up the current fuse buf */
		fuse_bufvec->buf[i].size = size;

		if(jfileDescriptor == NULL)
		{
			fuse_bufvec->buf[i].mem = (*env)->GetDirectBufferAddress(env, jbuffer);

			if(fuse_bufvec->buf[i].mem == NULL || (*env)->ExceptionCheck(env))
			{
				goto ERROR_HANDLER;
			}

		}else
		{
			jclass   clazz = (*env)->GetObjectClass(env, jfileDescriptor);
			jfieldID fid   = (*env)->GetFieldID(env,clazz,"fd","I");

			/* return the descriptor */
			int fd = (*env)->GetIntField(env, jfileDescriptor, fid);


			printf("FD is = %d\n", fd);
			printf("Pos is = %d\n", filePos);
			printf("Size is = %d\n", size);

			fuse_bufvec->buf[i].fd    = fd;
			fuse_bufvec->buf[i].flags = FUSE_BUF_IS_FD|FUSE_BUF_FD_RETRY;

			if(filePos != -1)
			{
				fuse_bufvec->buf[i].pos   = filePos;
				fuse_bufvec->buf[i].flags |= FUSE_BUF_FD_SEEK;
			}
		}

		/* Increments the number of buffers */
		fuse_bufvec->count++;

		printf("here1\n");
	}


	/* Reply with the fuse_bufvec */
	if(fuse_reply_data(fuse_req, fuse_bufvec, fuseBufferCopyFlags) != 0)
	{
		throw_FuseException(env, "fuse_reply_data");
		goto ERROR_HANDLER;
	}


	printf("Data sent\n");


	/* Return successfully */
	return;

	/* Error occurred */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env))
		{
			(*env)->ExceptionDescribe(env);
		}

		/* Return failure */
		return;

#endif
}

















