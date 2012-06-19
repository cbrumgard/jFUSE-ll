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


#include "jni.h"


#define FUSE_USE_VERSION 28

#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"

#include "config.h"


/*
 * Class:     fuse_FuseCTX
 * Method:    getUid
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FuseCTX_getUid(JNIEnv *env, jobject jfuse_ctx)
{
	/* Declarations */
	jclass	 class 			  = NULL;
	jfieldID fieldID 		  = NULL;
	struct fuse_ctx *fuse_ctx = NULL;



	/* Gets the class and the field ID for the internal pointer */
	if(((class = (*env)->GetObjectClass(env, jfuse_ctx)) == NULL) ||
       ((fieldID = (*env)->GetFieldID(env, class, "fuse_ctx_ptr", "J")) == NULL))
    {
		goto ERROR_HANDLER;
    }

	/* Gets a pointer to the internal fuse ctx pointer */
	fuse_ctx = (struct fuse_ctx*)(*env)->GetLongField(env, jfuse_ctx, fieldID);

	if(fuse_ctx == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Returns the uid as a long  */
	return (jlong)(fuse_ctx->uid);

	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}

/*
 * Class:     fuse_FuseCTX
 * Method:    getGid
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FuseCTX_getGid(JNIEnv *env, jobject jfuse_ctx)
{
	/* Declarations */
	jclass	 class 			  = NULL;
	jfieldID fieldID 		  = NULL;
	struct fuse_ctx *fuse_ctx = NULL;



	/* Gets the class and the field ID for the internal pointer */
	if(((class = (*env)->GetObjectClass(env, jfuse_ctx)) == NULL) ||
       ((fieldID = (*env)->GetFieldID(env, class, "fuse_ctx_ptr", "J")) == NULL))
    {
		goto ERROR_HANDLER;
    }

	/* Gets a pointer to the internal fuse ctx pointer */
	fuse_ctx = (struct fuse_ctx*)(*env)->GetLongField(env, jfuse_ctx, fieldID);

	if(fuse_ctx == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Returns the uid as a long  */
	return (jlong)(fuse_ctx->gid);

	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}


/*
 * Class:     fuse_FuseCTX
 * Method:    getPid
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FuseCTX_getPid(JNIEnv *env, jobject jfuse_ctx)
{
	/* Declarations */
	jclass	 class 			  = NULL;
	jfieldID fieldID 		  = NULL;
	struct fuse_ctx *fuse_ctx = NULL;



	/* Gets the class and the field ID for the internal pointer */
	if(((class = (*env)->GetObjectClass(env, jfuse_ctx)) == NULL) ||
       ((fieldID = (*env)->GetFieldID(env, class, "fuse_ctx_ptr", "J")) == NULL))
    {
		goto ERROR_HANDLER;
    }

	/* Gets a pointer to the internal fuse ctx pointer */
	fuse_ctx = (struct fuse_ctx*)(*env)->GetLongField(env, jfuse_ctx, fieldID);

	if(fuse_ctx == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Returns the uid as a long  */
	return (jlong)(fuse_ctx->pid);

	/* Error Handler */
	ERROR_HANDLER:

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;
}
