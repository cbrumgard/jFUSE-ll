/*  jFUSE-ll provides low level bindings to FUSE in Java
    Copyright (C) <2011>  <Christopher Brumgard>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "jni.h"


#define FUSE_USE_VERSION 28


#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"

#include "jFUSE_lowlevel_fuselib.h"
#include "fuse_util.h"
#include "fuse_errno.h"


/* Global for the java object */
static JavaVM *g_vm	   = NULL;
static jobject g_fusefs = NULL;

static bool thread_reuse = false;

int fuse_fd = -1;


struct fuse_userdata
{
	jobject obj;
};


static void java_release(bool unattach)
{
	if(thread_reuse == false && unattach)
	{
		(*g_vm)->DetachCurrentThread(g_vm);
	}
}


static int java_init(JNIEnv **env, bool *unattach)
{
	/* Declarations */
	jclass threadClass = NULL;
	jclass classLoader = NULL;
	jmethodID mid	   = NULL;
	jobject systemClassLoader = NULL;
	jobject thread	   = NULL;


	/* Gets the JNI env for the thread */
	int retval = (*g_vm)->GetEnv(g_vm, (void **)env, JNI_VERSION_1_2);

	switch(retval)
	{
		/* Gets the env */
		case JNI_OK:
			*unattach = false;
			break;

		/* Thread is not already attached */
		default:
			(*g_vm)->AttachCurrentThread(g_vm, (void **)env, NULL);
			*unattach = true;
			break;
	}


	/***----------------------------- Following code does ------------------***
	 * Thread.currentThread().setContextClassLoader(
	 * 								ClassLoader.getSystemClassLoader());
	 ***--------------------------------------------------------------------***/

	/* Finds the class loader class */
	classLoader = (**env)->FindClass(*env, "java/lang/ClassLoader");

	if(classLoader == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Finds the thread class */
	threadClass = (**env)->FindClass(*env, "java/lang/Thread");

	if(threadClass == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the ClassLoader.getSystemClassLoader static method to get the
	 * class loader */
	mid = (**env)->GetStaticMethodID(*env, classLoader, "getSystemClassLoader",
									"()Ljava/lang/ClassLoader;");

	if(mid == NULL)
	{
		goto ERROR_HANDLER;
	}

	systemClassLoader = (**env)->CallStaticObjectMethod(*env, classLoader, mid);

	if(systemClassLoader == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Class the currentThread method on Thread to get the Thread object */
	mid = (**env)->GetStaticMethodID(*env, threadClass, "currentThread", "()Ljava/lang/Thread;");


	if(mid == NULL)
	{
		goto ERROR_HANDLER;
	}

	thread = (**env)->CallStaticObjectMethod(*env, threadClass, mid);

	if(thread == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Calls the setContextClassLoader on the Thread object to set a thread
	 * loader */
	mid = (**env)->GetMethodID(*env, threadClass, "setContextClassLoader",
								"(Ljava/lang/ClassLoader;)V");

	if(mid == NULL)
	{
		goto ERROR_HANDLER;
	}

	(**env)->CallVoidMethod(*env, thread, mid, systemClassLoader);

	if((**env)->ExceptionOccurred(*env))
	{
		goto ERROR_HANDLER;
	}

	/* Success */
	return 0;


	/* Error handler */
	ERROR_HANDLER:

		java_release(unattach);

		return -1;
}





static void java_fuse_init(void *userdata, struct fuse_conn_info *fuse_conn_info)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject userobj 	= NULL;
	jobject connInfo = NULL;
	jclass cls = NULL;
	jmethodID mid = NULL;
	jfieldID ptrID = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the user object if there is one */
	if(userdata != NULL)
	{
		userobj = ((struct fuse_userdata*)userdata)->obj;
	}else
	{
		userobj = (*env)->NewGlobalRef(env, NULL);
	}



	/* Creates a new ConnInfo object */
	if(((cls = (*env)->FindClass(env, "LjFUSE/lowlevel/ConnInfo;")) == NULL) ||
	   ((mid = (*env)->GetMethodID(env, cls, "<init>", "()V")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	connInfo = (*env)->NewObject(env, cls, mid);

	if(connInfo == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Stores the pointer to the fuse_conn_info struct */
	if((ptrID = (*env)->GetFieldID(env, cls, "fuse_conn_info_ptr", "J")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	(*env)->SetLongField(env, connInfo, ptrID, (jlong)fuse_conn_info);


	/* Gets the init method for the fuse object */
	if(((cls = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((mid = (*env)->GetMethodID(env, cls, "init",
			   "(Ljava/lang/Object;LjFUSE/lowlevel/ConnInfo;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Call the init method on the fuse object */
	(*env)->CallVoidMethod(env, g_fusefs, mid, userobj, connInfo);

	/* Check if there was an exception in the java fuse call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Print the exception */
		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		/* Detach the thread from java */
		java_release(unattach);


		/* Return */
		return;
}


static void java_fuse_destroy(void *userdata)
{
	/* Declarations */
	JNIEnv *env = NULL;
	jobject userObj = NULL;
	jclass	cls     = NULL;
	jmethodID mid   = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the user object */
	if(userdata != NULL)
	{
		userObj = ((struct fuse_userdata*)(userdata))->obj;
	}else
	{
		userObj = (*env)->NewGlobalRef(env, NULL);
	}


	/* Calls the destroy method */
	if(((cls = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((mid = (*env)->GetMethodID(env, cls, "destroy",
			   	   	   	   "(Ljava/lang/Object;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	(*env)->CallVoidMethod(env, g_fusefs, mid, userObj);

	/* Check if there was an exception in the java fuse call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Print the exception */
		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		/* Detach the thread from java */
		java_release(unattach);

		/* Return */
		return;
}


static void java_fuse_lookup(fuse_req_t req, fuse_ino_t parent, const char *path)
{
	/* Declarations */
	JNIEnv *env = NULL;
	jstring jpath = NULL;
	jobject jrequest  = NULL;
	jclass requestClass;
	jmethodID requestID;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	jclass cls = (*env)->GetObjectClass(env, g_fusefs);
	jmethodID mid = (*env)->GetMethodID(env, cls, "lookup",
						"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;)V");

	assert(mid != 0);

	/* Request */
	if((requestClass = (*env)->FindClass(env, "jFUSE/lowlevel/FuseRequest")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	if((requestID = (*env)->GetMethodID(env, requestClass, "<init>", "(J)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	if((jrequest = (*env)->NewObject(env, requestClass, requestID, (jlong)req)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Java string for path */
	jpath = (*env)->NewStringUTF(env, path);

	if(jpath == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the lookup method of FuseFS*/
	(*env)->CallVoidMethod(env, g_fusefs, mid, jrequest, (jlong)parent, jpath);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);


	/* Return successfully */
	return;

	/* Error Hanlder */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;
}


static void java_fuse_forget(fuse_req_t req, fuse_ino_t ino, unsigned long nlookup)
{
	/* Declarations */
	JNIEnv *env = NULL;
	jobject request = NULL;
	jclass cls;
	jmethodID mid;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the fuse request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the forget method from the FuseFS subclass */
	if((cls = (*env)->GetObjectClass(env, g_fusefs)) == NULL ||
	   (mid = (*env)->GetMethodID(env, cls, "forget",
			   	   	   	   	   	   "(LjFUSE/lowlevel/FuseRequest;JJ)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Call the forget method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, (jlong)nlookup);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Detach the thread from java */
			java_release(unattach);
		}


		/* No error is allowed */
		fuse_reply_none(req);

		return;
}


static void java_fuse_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name,
							mode_t mode)
{
	/* Declarations */
	JNIEnv   *env     = NULL;
	jclass    cls     = NULL;
	jmethodID mid     = NULL;
	jobject   request = NULL;
	jobject   jname   = NULL;
	jobject   jmode   = NULL;
	bool unattach = false;


	/* Assert */
	assert(sizeof(parent) <= sizeof(jlong));


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates a java string for the name of the file */
	if((jname = (*env)->NewStringUTF(env, name)) == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the constructor for Mode*/
	if(((cls = (*env)->FindClass(env, "jFUSE/lowlevel/Mode")) == NULL) ||
		(mid = (*env)->GetMethodID(env, cls, "<init>", "(J)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Allocates a Mode object with set of permissions */
	jmode = (*env)->NewObject(env, cls, mid, (jlong)mode);

	if(jmode == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the mkdir method to call */
	if(((cls = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((mid = (*env)->GetMethodID(env, cls, "mkdir",
			   "(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;LjFUSE/lowlevel/Mode;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}


	/* Calls the mkdir java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)parent, jname,
							jmode);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the string */
	(*env)->DeleteLocalRef(env, jname);
	(*env)->DeleteLocalRef(env, jmode);

	/* Detach the thread from java */
	java_release(unattach);



	/* Return success */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Frees the local reference to the string */
			if(jname != NULL)
			{
				(*env)->DeleteLocalRef(env, jname);
			}

			if(jmode != NULL)
			{
				(*env)->DeleteLocalRef(env, jmode);
			}

			/* Detach the thread from java */
			java_release(unattach);

		}


		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Error */
		return;
}



static void java_fuse_getattr(fuse_req_t req, fuse_ino_t ino,
							struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for FuseFs.getattr */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
		(mid = (*env)->GetMethodID(env, class, "getattr",
									"(LjFUSE/lowlevel/FuseRequest;J)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;
}




static void java_fuse_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	jobject	fileInfo = NULL;
	jmethodID constructorID = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for FuseFs.getattr */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
		(mid = (*env)->GetMethodID(env, class, "open",
				"(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FileInfo;)V")) == NULL)
	{
		(*env)->ExceptionDescribe(env);
		goto ERROR_HANDLER;
	}

	/* Gets the constructor for FileInfo */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FileInfo")) == NULL) ||
	    (constructorID = (*env)->GetMethodID(env, class, "<init>", "(J)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Allocates the object passing the pointer to the struct */
	fileInfo = (*env)->NewObject(env, class, constructorID, (jlong)fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}


	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;
}


static void java_fuse_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t offset,
					struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jobject fileInfo = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Makes a FileInfo object */
	fileInfo = new_fuse_file_info(env, fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for FuseFs.getattr */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
		(mid = (*env)->GetMethodID(env, class, "read",
									"(LjFUSE/lowlevel/FuseRequest;JJJLjFUSE/lowlevel/FileInfo;)V")) == NULL)
	{
		(*env)->ExceptionDescribe(env);
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, (jlong)size,
							(jlong)offset, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;
}


static void java_fuse_write(fuse_req_t req, fuse_ino_t ino, const char *buf,
					size_t size, off_t off, struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jobject fileInfo = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	jobject jbuf    = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Makes a FileInfo object */
	fileInfo = new_fuse_file_info(env, fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for FuseFs.getattr */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
		(mid = (*env)->GetMethodID(env, class, "write",
					"(LjFUSE/lowlevel/FuseRequest;JLjava/nio/ByteBuffer;JJLjFUSE/lowlevel/FileInfo;)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	if((jbuf = make_byte_array_for_buffer(env, (void*)buf, (jlong)size)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, jbuf,
			               (jlong)size, (jlong)off, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);



	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;

}





static void java_fuse_release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jobject fuseFileInfo = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	bool unattach = false;


	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates a fuse file info for the release call */
	fuseFileInfo = new_fuse_file_info(env, fi);

	if(fuseFileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the class and method id for FuseFs.getattr */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
		(mid = (*env)->GetMethodID(env, class, "release",
				"(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FileInfo;)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, fuseFileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);



	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		printf("%s:%s:%d\n", __FILE__,__FUNCTION__,__LINE__);

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;
}


static void java_fuse_opendir(fuse_req_t req, fuse_ino_t ino,
							struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	 = NULL;
	jobject request  = NULL;
	jobject fileInfo = NULL;
	jclass  class    = NULL;
	jmethodID mid    = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Makes a FileInfo object */
	fileInfo = new_fuse_file_info(env, fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the opendir method to call */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((mid   = (*env)->GetMethodID(env, class, "opendir",
			   "(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FileInfo;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the opendir method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}


	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;
}


static void java_fuse_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
								off_t off, struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	jobject fileInfo = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Makes a FileInfo object */
	fileInfo = new_fuse_file_info(env, fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the readdir method to call */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((mid   = (*env)->GetMethodID(env, class, "readdir",
			   "(LjFUSE/lowlevel/FuseRequest;JJJLjFUSE/lowlevel/FileInfo;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the readdir method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, (jlong)size,
							(jlong)off, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

	if(env != NULL)
	{
		/* Check for the exception */
		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		java_release(unattach);

	}

	/* Post a generic error */
	fuse_reply_err(req, EIO);

	/* Return */
	return;
}

static void java_fuse_releasedir(fuse_req_t req, fuse_ino_t ino,
									struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	jobject fileInfo = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Makes a FileInfo object */
	fileInfo = new_fuse_file_info(env, fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the readdir method to call */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((mid   = (*env)->GetMethodID(env, class, "releasedir",
			   "(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FileInfo;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the readdir method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)ino, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;
}


static void java_fuse_create(fuse_req_t req, fuse_ino_t parent, const char *name,
						mode_t mode, struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env 	= NULL;
	jobject request = NULL;
	jclass  class   = NULL;
	jmethodID mid   = NULL;
	jstring	jname   = NULL;
	jobject	fileInfo = NULL;
	jmethodID constructorID = NULL;
	jobject jmode	 = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	request = new_fuse_request(env, req);

	if(request == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for FuseFs.getattr */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
		(mid = (*env)->GetMethodID(env, class, "create",
			"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;LjFUSE/lowlevel/Mode;LjFUSE/lowlevel/FileInfo;)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates a java string for the name of the file */
	if((jname = (*env)->NewStringUTF(env, name)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the constructor for FileInfo */
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/FileInfo")) == NULL) ||
	    (constructorID = (*env)->GetMethodID(env, class, "<init>", "(J)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Allocates the object passing the pointer to the struct */
	fileInfo = (*env)->NewObject(env, class, constructorID, (jlong)fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the constructor for Mode*/
	if(((class = (*env)->FindClass(env, "jFUSE/lowlevel/Mode")) == NULL) ||
		    (constructorID = (*env)->GetMethodID(env, class, "<init>", "(J)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Allocates a Mode object with set of permissions */
	jmode = (*env)->NewObject(env, class, constructorID, (jlong) mode);

	if(jmode == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, mid, request, (jlong)parent, jname,
							jmode, fileInfo);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the string and FileInfo */
	(*env)->DeleteLocalRef(env, jname);
	(*env)->DeleteLocalRef(env, fileInfo);

	/* Detach the thread from java */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Frees the local reference to the string */
			if(jname != NULL)
			{
				(*env)->DeleteLocalRef(env, jname);
			}

			if(fileInfo != NULL)
			{
				(*env)->DeleteLocalRef(env, fileInfo);
			}

			if(jmode != NULL)
			{
				(*env)->DeleteLocalRef(env, jmode);
			}

			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		/* Return */
		return;



}



static void java_fuse_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
						int to_set, struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv *env = NULL;
	jclass classID = NULL;
	jmethodID methodID = NULL;
	bool unattach = false;


	/* Attaches the current thread to java */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	jobject fuseRequest = NULL;
	jobject fuseStat     = NULL;
	jobject fuseFileInfo = NULL;


	/* Creates the fuse file info */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates the fuseStat */
	fuseStat = new_fuse_stat(env, attr);

	if(fuseStat == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates the fuse file info */
	if(fi != NULL)
	{
		fuseFileInfo = new_fuse_file_info(env, fi);

		if(fuseFileInfo == NULL)
		{
			goto ERROR_HANDLER;
		}

	}else
	{
		fuseFileInfo = NULL;
	}

	/* Gets the method for setattr */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((methodID = (*env)->GetMethodID(env, classID, "setattr",
			   "(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FuseStat;"
			   "JLjFUSE/lowlevel/FileInfo;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the method and checks for any errors */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
							fuseStat, (jlong)to_set, fuseFileInfo);

	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detaches the current thread */
	java_release(unattach);


	/* Return */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Detaches the current thread */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;
}


static void java_fuse_ioctl(fuse_req_t req, fuse_ino_t ino, int cmd, void *arg,
		struct fuse_file_info *fi, unsigned flags,
		const void *in_buf, size_t in_bufsz, size_t out_bufsz)
{
	/* Declarations */
	JNIEnv *env = NULL;
	jclass classID = NULL;
	jmethodID methodID = NULL;
	jobject fuseRequest = NULL;
	jobject fuseFileInfo = NULL;
	bool unattach = false;


	/* Attaches the current thread to java */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Creates the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Creates the fuse file info */
	fuseFileInfo = new_fuse_file_info(env, fi);

	if(fuseFileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the method for setattr */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((methodID = (*env)->GetMethodID(env, classID, "ioctl",
					"(LjFUSE/lowlevel/FuseRequest;JILjava/nio/ByteBuffer;"
					"LjFUSE/lowlevel/FileInfo;JLjava/nio/ByteBuffer;JJ)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the method and checks for any errors */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
						   (jlong)cmd, NULL, fuseFileInfo, (jlong)flags, NULL, (jlong)in_bufsz,
						   (jlong)out_bufsz);

	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}


	/* Detaches the current thread */
	java_release(unattach);


	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Detaches the current thread */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;
}

static void java_fuse_readlink(fuse_req_t req, fuse_ino_t ino)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
					mode_t mode, dev_t rdev)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_unlink(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	/* Declarations */
	JNIEnv *env = NULL;
	jstring jname = NULL;
	jobject fuseRequest  = NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	jclass cls = (*env)->GetObjectClass(env, g_fusefs);
	jmethodID mid = (*env)->GetMethodID(env, cls, "unlink",
			"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;)V");

	assert(mid != 0);


	/* Creates the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Java string for path */
	jname = (*env)->NewStringUTF(env, name);

	if(jname == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the lookup method of FuseFS*/
	(*env)->CallVoidMethod(env, g_fusefs, mid, fuseRequest, (jlong)parent, jname);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Detach the thread from java */
	java_release(unattach);


	/* Return successfully */
	return;



	/* Error Hanlder */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;

}

static void java_fuse_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_symlink(fuse_req_t req, const char *link, fuse_ino_t parent,
		const char *name)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_rename(fuse_req_t req, fuse_ino_t parent, const char *name,
		fuse_ino_t newparent, const char *newname)
{
	/* Declarations */
	JNIEnv *env 		= NULL;
	jobject fuseRequest = NULL;
	jstring jname    	= NULL;
	jstring jnewname 	= NULL;
	jclass  clsID    	= NULL;
	jmethodID methodID 	= NULL;
	bool unattach = false;


	/* Connect this thread to the jvm */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Creates the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Java string for current name */
	jname = (*env)->NewStringUTF(env, name);

	if(jname == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Java string for new name */
	jnewname = (*env)->NewStringUTF(env, newname);

	if(jnewname == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the method to call */
	if(((clsID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
        (methodID = (*env)->GetMethodID(env, clsID, "rename",
				"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;JLjava/lang/String;)V")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the rename method with the given arguments */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)parent,
				jname, (jlong)newparent, jnewname);

	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}


	/* Detaches the current thread */
	java_release(unattach);


	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;
}


static void java_fuse_link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent,
					const char *newname)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_fsync(fuse_req_t req, fuse_ino_t ino, int datasync,
						struct fuse_file_info *fi)
{
	/* Declarations */
	JNIEnv  *env			= NULL;
	jobject fuseRequest 	= NULL;
	jobject fileInfo		= NULL;
	jboolean onlyDataSync 	= JNI_FALSE;
	jclass	 class			= NULL;
	jmethodID methodID 		= NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the fuse file info object */
	fileInfo = new_fuse_file_info(env, fi);

	if(fileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Sets whether to seek the metadata as well as the data */
	onlyDataSync = (datasync != 0) ? JNI_TRUE : JNI_FALSE;

	/* Gets the class and method id for fsync */
	if(((class = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((methodID = (*env)->GetMethodID(env, class, "fsync",
			   "(LjFUSE/lowlevel/FuseRequest;JZLjFUSE/lowlevel/FileInfo;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the fsync method */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
							onlyDataSync, fileInfo);


	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the string and FileInfo */
	(*env)->DeleteLocalRef(env, fuseRequest);
	(*env)->DeleteLocalRef(env, fileInfo);

	/* Detach the thread from java */
	java_release(unattach);

	/* Return successfully */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Frees the resources */
		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);
			}

			if(fileInfo != NULL)
			{
				(*env)->DeleteLocalRef(env, fileInfo);
			}

			java_release(unattach);
		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;
}

static void java_fuse_fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync,
						struct fuse_file_info *fi)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

static void java_fuse_statfs(fuse_req_t req, fuse_ino_t ino)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}

/* Apple */
#ifdef __APPLE__
void  java_fuse_setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                          const char *value, size_t size, int flags,
                          uint32_t position)

/* Linux */
#else
static void java_fuse_setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
							const char *value, size_t size, int flags)
#endif
{
	/* Declarations */
	JNIEnv *env = NULL;
	jclass classID = NULL;
	jmethodID methodID = NULL;
	jobject fuseRequest  = NULL;
	jstring attrname	= NULL;
	jobject valueBuffer	= NULL;
	bool unattach = false;


	/* Attaches the current thread to java */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Creates the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Java string for path */
	attrname = (*env)->NewStringUTF(env, name);

	if(attrname == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates a java buffer for the bytes */
	valueBuffer = make_byte_array_for_buffer(env, (void *)value, (jlong)size);

	if(valueBuffer == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the method for setattr */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((methodID = (*env)->GetMethodID(env, classID, "setxattr",
			"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;Ljava/nio/ByteBuffer;I)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the method and checks for any errors */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
						  	  attrname, valueBuffer, (jint)flags);


	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Fress resources */
	(*env)->DeleteLocalRef(env, fuseRequest);
	(*env)->DeleteLocalRef(env, attrname);
	(*env)->DeleteLocalRef(env, valueBuffer);

	java_release(unattach);

	/* Return ok */
	return;


	/* Error handler */
	ERROR_HANDLER:

		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}

			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);

				if(attrname != NULL)
				{
					(*env)->DeleteLocalRef(env, attrname);

					if(valueBuffer != NULL)
					{
						(*env)->DeleteLocalRef(env, valueBuffer);
					}
				}
			}

			/* Detaches the current thread */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);

		return;
}

#ifdef __APPLE__
static void java_fuse_getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
						size_t size, uint32_t position)
#else
static void java_fuse_getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
						size_t size)
#endif
{
	/* Declarations */
	JNIEnv  *env			= NULL;
	jclass classID 			= NULL;
	jmethodID methodID 		= NULL;
	jobject fuseRequest 	= NULL;
	jstring attrname		= NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Java string for path */
	attrname = (*env)->NewStringUTF(env, name);

	if(attrname == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for fsync */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
			((methodID = (*env)->GetMethodID(env, classID, "getxattr",
					"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;J)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getxattr method */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
			attrname, (jlong)size);


	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, fuseRequest);

	/* Detach the thread from java */
	java_release(unattach);


	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Frees resources */
		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}


			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);
			}

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);
}

static void java_fuse_listxattr(fuse_req_t req, fuse_ino_t ino, size_t size)
{
	/* Declarations */
	JNIEnv  *env			= NULL;
	jclass classID 			= NULL;
	jmethodID methodID 		= NULL;
	jobject fuseRequest 	= NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for fsync */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
	   ((methodID = (*env)->GetMethodID(env, classID, "listxattr",
					"(LjFUSE/lowlevel/FuseRequest;JJ)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the listxattr method */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
							(jlong)size);


	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, fuseRequest);

	/* Detach the thread from java */
	java_release(unattach);


	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Frees resources */
		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}


			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);
			}

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);
}


static void java_fuse_removexattr(fuse_req_t req, fuse_ino_t ino, const char *name)
{
	/* Declarations */
	JNIEnv  *env			= NULL;
	jclass classID 			= NULL;
	jmethodID methodID 		= NULL;
	jobject fuseRequest 	= NULL;
	jstring attrname		= NULL;
	bool unattach = false;


	/* Gets the JNI env for the thread */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Java string for path */
	attrname = (*env)->NewStringUTF(env, name);

	if(attrname == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for fsync */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
			((methodID = (*env)->GetMethodID(env, classID, "removexattr",
					"(LjFUSE/lowlevel/FuseRequest;JLjava/lang/String;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the removexattr method */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
			attrname);


	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, fuseRequest);
	(*env)->DeleteLocalRef(env, attrname);

	/* Detach the thread from java */
	java_release(unattach);


	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Frees resources */
		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}


			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);

				if(attrname != NULL)
				{
					(*env)->DeleteLocalRef(env, attrname);
				}
			}

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);
}

static void java_fuse_access(fuse_req_t req, fuse_ino_t ino, int mask)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}



static void java_fuse_getlk(fuse_req_t req, fuse_ino_t ino,
		struct fuse_file_info *fi, struct flock *lock)
{
	/* Declarations */
	JNIEnv *env = NULL;
	struct flock *copyflock = NULL;
	jobject fuseRequest = NULL;
	jobject fuseFileInfo = NULL;
	jobject fuseFlock = NULL;
	jclass  classID   = NULL;
	jmethodID methodID   = NULL;
	bool unattach = false;


	/* Initializes the jvm env and attaches the thread to java */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the FileInfo */
	fuseFileInfo = new_fuse_file_info(env, fi);

	if(fuseFileInfo == NULL)
	{
		(*env)->ThrowNew(env,
				(*env)->FindClass(env, "java/lang/OutOfMemoryError"),
				"Can not allocate a C struct flock from heap");
		goto ERROR_HANDLER;
	}

	/* Creates a copy of the flock so java can have its own */
	copyflock = malloc(sizeof(struct flock));

	if(copyflock == NULL)
	{
		(*env)->ThrowNew(env,
					(*env)->FindClass(env, "java/lang/OutOfMemoryError"),
					"Can not allocate a C struct flock from heap");
		goto ERROR_HANDLER;
	}

	memcpy(copyflock, lock, sizeof(struct flock));


	/* Creates a fuse flock */
	fuseFlock = new_fuse_flock(env, copyflock);

	if(fuseFlock == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the class and method id for fsync */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
			((methodID = (*env)->GetMethodID(env, classID, "getlk",
					"(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FileInfo;LjFUSE/lowlevel/FLock;)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
			fuseFileInfo, fuseFlock);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, fuseRequest);
	(*env)->DeleteLocalRef(env, fuseFileInfo);
	(*env)->DeleteLocalRef(env, fuseFlock);

	/* Detach the thread from java */
	java_release(unattach);


	/* Return success */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Frees resources */
		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}


			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);
			}

			if(fuseFileInfo != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseFileInfo);
			}


			/*if(fuseFlock != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseFlock);
			}else if(copyflock != NULL)
			{
				free(copyflock);
			}*/

			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);
}



static void java_fuse_setlk(fuse_req_t req, fuse_ino_t ino,
		struct fuse_file_info *fi, struct flock *lock, int sleep)
{
	/* Declarations */
	JNIEnv *env = NULL;
	struct flock *copyflock = NULL;
	jobject fuseRequest = NULL;
	jobject fuseFileInfo = NULL;
	jobject fuseFlock = NULL;
	jclass  classID   = NULL;
	jmethodID methodID   = NULL;
	bool unattach = false;


	/* Initializes the jvm env and attaches the thread to java */
	if((unattach = java_init(&env, &unattach)) != 0)
	{
		if(env == NULL)
		{
			return;
		}

		goto ERROR_HANDLER;
	}

	/* Gets the request */
	fuseRequest = new_fuse_request(env, req);

	if(fuseRequest == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates a fuse file info for the release call */
	fuseFileInfo = new_fuse_file_info(env, fi);

	if(fuseFileInfo == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Creates a copy of the flock so java can have its own */
	copyflock = malloc(sizeof(struct flock));

	if(copyflock == NULL)
	{
		(*env)->ThrowNew(env,
				(*env)->FindClass(env, "java/lang/OutOfMemoryError"),
				"Can not allocate a C struct flock from heap");
		goto ERROR_HANDLER;
	}

	memcpy(copyflock, lock, sizeof(struct flock));


	/* Creates a fuse flock */
	fuseFlock = new_fuse_flock(env, copyflock);

	if(fuseFlock == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class and method id for fsync */
	if(((classID = (*env)->GetObjectClass(env, g_fusefs)) == NULL) ||
			((methodID = (*env)->GetMethodID(env, classID, "setlk",
					"(LjFUSE/lowlevel/FuseRequest;JLjFUSE/lowlevel/FileInfo;LjFUSE/lowlevel/FLock;Z)V")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Calls the getattr java method */
	(*env)->CallVoidMethod(env, g_fusefs, methodID, fuseRequest, (jlong)ino,
			fuseFileInfo, fuseFlock, (jboolean)sleep);

	/* Check for an uncaught exception from the call */
	if((*env)->ExceptionCheck(env) == JNI_TRUE)
	{
		goto ERROR_HANDLER;
	}

	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, fuseRequest);
	(*env)->DeleteLocalRef(env, fuseFlock);

	/* Detach the thread from java */
	java_release(unattach);

	/* Return success */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Frees resources */
		if(env != NULL)
		{
			/* Check for the exception */
			if((*env)->ExceptionCheck(env) == JNI_TRUE)
			{
				(*env)->ExceptionDescribe(env);
			}


			if(fuseRequest != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseRequest);
			}

			if(fuseFlock != NULL)
			{
				(*env)->DeleteLocalRef(env, fuseFlock);
			}else if(copyflock != NULL)
			{
				free(copyflock);
			}


			/* Detach the thread from java */
			java_release(unattach);

		}

		/* Post a generic error */
		fuse_reply_err(req, EIO);
}

static void java_fuse_bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize, uint64_t idx)
{
	fprintf(stderr, "%s not implemented\n", __FUNCTION__);
	fuse_reply_err(req, EIO);
}





static struct fuse_lowlevel_ops fuse_ops =
	{
		.init 	 		= NULL,
		.destroy 		= NULL,
		.lookup  		= NULL,
		.forget  		= NULL,
		.getattr 		= NULL,
		.setattr 		= NULL, //java_fuse_setattr,
		.readlink 		= NULL, //java_fuse_readlink,
		.mknod			= NULL, //java_fuse_mknod,
		.mkdir			= java_fuse_mkdir,
		.unlink			= NULL, //java_fuse_unlink,
		.rmdir			= NULL, //java_fuse_rmdir,
		.symlink		= java_fuse_symlink,
		.rename			= NULL, //java_fuse_rename,
		.link			= NULL, //java_fuse_link,
		.open			= NULL,
		.read			= NULL,
		.write			= NULL,
		.flush			= NULL, //java_fuse_flush,
		.release		= NULL,
		.fsync			= NULL, //java_fuse_fsync,
		.opendir		= NULL,
		.readdir		= NULL,
		.releasedir 	= NULL,
		.fsyncdir		= NULL, //java_fuse_fsyncdir,
		.statfs			= NULL, //java_fuse_statfs,
		.setxattr		= NULL, //java_fuse_setxattr,
		.getxattr		= NULL, //java_fuse_getxattr,
		.listxattr		= NULL, //java_fuse_listxattr,
		.removexattr 	= NULL,  //java_fuse_removexattr,
		.access			= NULL, //java_fuse_access,
		.create			= NULL,
		.getlk			= NULL, //java_fuse_getlk,
		.setlk			= NULL, //java_fuse_setlk,
		.bmap			= NULL, //java_fuse_bmap,

#if FUSE_VERSION >= 28
		.ioctl			= NULL,
#endif

		/*.poll			   = NULL */
	};



static int load_fuse_methods(JNIEnv *env, jobject fusefs,
							struct fuse_lowlevel_ops *f_ops)
{
	/* Declarations */
	jclass    cls;
	jmethodID mid;
	jobject   classObj;
	jobject   methods;
	jsize i;
	jsize numMethods;
	jobject method;
	jstring methodName;
	char *name;


	/* Gets the class id of the fusefs object */
	if((cls = (*env)->GetObjectClass(env, fusefs)) == NULL)
	{
		goto ERROR_HANDLER;

	}

	/* Gets the getClass method for the class */
	if((mid = (*env)->GetMethodID(env, cls, "getClass",
				"()Ljava/lang/Class;")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/*  Calls the method to get the class object*/
	if((classObj = (*env)->CallObjectMethod(env, fusefs, mid)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class id for the class object */
	if((cls = (*env)->GetObjectClass(env, classObj)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the getDeclaredMethods for the class object */
	/*if((mid = (*env)->GetMethodID(env, cls, "getDeclaredMethods",
				"()[Ljava/lang/reflect/Method;")) == NULL)
	{
		goto ERROR_HANDLER;
	}*/

	if((mid = (*env)->GetMethodID(env, cls, "getMethods",
					"()[Ljava/lang/reflect/Method;")) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Calls the method to get an array of methods */
	if((methods = (*env)->CallObjectMethod(env, classObj, mid)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Number of methods */
	numMethods = (*env)->GetArrayLength(env, methods);

	/* Iterates over the methods filling in callback methods for fuse ops */
	for(i=0; i<numMethods; i++)
	{
		/* Gets the current method object */
		method = (*env)->GetObjectArrayElement(env, methods, i);

		/* Gets the method name */
		if((cls = (*env)->GetObjectClass(env, method)) == NULL)
		{
			goto ERROR_HANDLER;
		}

		if((mid = (*env)->GetMethodID(env, cls, "getName", "()Ljava/lang/String;")) == NULL)
		{
			goto ERROR_HANDLER;
		}

		if((methodName = (*env)->CallObjectMethod(env, method, mid)) == NULL)
		{
			goto ERROR_HANDLER;
		}

		/* Gets a cstring for the name */
		if((name = (char*)(*env)->GetStringUTFChars(env, methodName, NULL)) == NULL)
		{
			goto ERROR_HANDLER;
		}

		printf("Method is %s\n", name);


		/* If the method is one of the fuse methods then it assigns a callback
		 * function for it */
		if(strcmp(name, "init") == 0)
		{
			f_ops->init     = java_fuse_init;
		}else if(strcmp(name, "destroy") == 0)
		{
			f_ops->destroy  = java_fuse_destroy;
		}else if(strcmp(name, "lookup") == 0)
		{
			f_ops->lookup 	= java_fuse_lookup;
		}else if(strcmp(name, "forget") == 0)
		{
			f_ops->forget 	= java_fuse_forget;
		}else if(strcmp(name, "getattr") == 0)
		{
			f_ops->getattr 	= java_fuse_getattr;
		}else if(strcmp(name, "setattr") == 0)
		{
			f_ops->setattr 	= java_fuse_setattr;
		}else if(strcmp(name, "readlink") == 0)
		{
			f_ops->readlink 	= java_fuse_readlink;
		}else if(strcmp(name, "mknod") == 0)
		{
			f_ops->mknod		= java_fuse_mknod;
		}else if(strcmp(name, "mkdir") == 0)
		{
			f_ops->mkdir		= java_fuse_mkdir;
		}else if(strcmp(name, "unlink") == 0)
		{
			f_ops->unlink		= java_fuse_unlink;
		}else if(strcmp(name, "rmdir") == 0)
		{
			f_ops->rmdir		= java_fuse_rmdir;
		}else if(strcmp(name, "symlink") == 0)
		{
			f_ops->symlink	= java_fuse_symlink;
		}else if(strcmp(name, "rename") == 0)
		{
			f_ops->rename		= java_fuse_rename;
		}else if(strcmp(name, "link") == 0)
		{
			f_ops->link		= java_fuse_link;
		}else if(strcmp(name, "open") == 0)
		{
			f_ops->open		= java_fuse_open;
		}else if(strcmp(name, "read") == 0)
		{
			f_ops->read		= java_fuse_read;
		}else if(strcmp(name, "write") == 0)
		{
			f_ops->write	= java_fuse_write;
		}else if(strcmp(name, "flush") == 0)
		{
			f_ops->flush	= java_fuse_flush;
		}else if(strcmp(name, "release") == 0)
		{
			f_ops->release	= java_fuse_release;
		}else if(strcmp(name, "fsync") == 0)
		{
			f_ops->fsync  = java_fuse_fsync;
		}else if(strcmp(name, "opendir") == 0)
		{
			f_ops->opendir	= java_fuse_opendir;
		}else if(strcmp(name, "readdir") == 0)
		{
			f_ops->readdir	= java_fuse_readdir;
		}else if(strcmp(name, "releasedir") == 0)
		{
			f_ops->releasedir = java_fuse_releasedir;
		}else if(strcmp(name, "fsyncdir") == 0)
		{
			f_ops->fsyncdir	= java_fuse_fsyncdir;
		}else if(strcmp(name, "statfs") == 0)
		{
			f_ops->statfs		= java_fuse_statfs;
		}else if(strcmp(name, "setxattr") == 0)
		{
			f_ops->setxattr	= java_fuse_setxattr;
		}else if(strcmp(name, "getxattr") == 0)
		{
			f_ops->getxattr	= java_fuse_getxattr;
		}else if(strcmp(name, "listxattr") == 0)
		{
			f_ops->listxattr	= java_fuse_listxattr;
		}else if(strcmp(name, "removexattr") == 0)
		{
			f_ops->removexattr = java_fuse_removexattr;
		}else if(strcmp(name, "access") == 0)
		{
			f_ops->access		= java_fuse_access;
		}else if(strcmp(name, "create") == 0)
		{
			f_ops->create		= java_fuse_create;
		}else if(strcmp(name, "getlk") == 0)
		{
			f_ops->getlk		= java_fuse_getlk;
		}else if(strcmp(name, "setlk") == 0)
		{
			f_ops->setlk		= java_fuse_setlk;
		}else if(strcmp(name, "bmap") == 0)
		{
			f_ops->bmap			= java_fuse_bmap;
		}

#if FUSE_VERSION >= 28

		else if(strcmp(name, "ioctl") == 0)
		{
			printf("Assigning ioctl\n");
			f_ops->ioctl		= java_fuse_ioctl;
		}
#endif

		/* Releases the cstring */
		(*env)->ReleaseStringUTFChars(env, methodName, name);
	}


	/* Return  */
	return 0;


	/* Error */
	ERROR_HANDLER:

		/* Check for the exception */
		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return -1;

}


static void print_handler(int sig)
{
	fprintf(stderr, "Signal %d occurred\n", sig);
}

static int set_one_signal_handler(int sig, void (*handler)(int))
{
	struct sigaction sa;
	struct sigaction old_sa;

	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handler;
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = 0;

	if (sigaction(sig, NULL, &old_sa) == -1)
	{
		perror("fuse: cannot get old signal handler");
		return -1;
	}

	if (old_sa.sa_handler == SIG_DFL &&
			sigaction(sig, &sa, NULL) == -1)
	{
		perror("fuse: cannot set signal handler");
		return -1;
	}

	return 0;
}



struct fuse_thread_info
{
	struct fuse_session *se;
	pthread_mutex_t		 lock;
	pthread_cond_t       done;
};

static void *fuse_thread_loop(void *arg)
{
	/* Declarations */
	struct fuse_thread_info *info	= (struct fuse_thread_info *)arg;
	struct fuse_chan *fuse_chan = NULL;
	size_t buf_size				= 0;
	char *recvbuf				= NULL;
	int  result					= 0;


	printf("At %s:%d in %s\n", __FILE__, __LINE__, __FUNCTION__);

	/* Gets the fuse channel */
	fuse_chan = fuse_session_next_chan(info->se, NULL);

	/* Gets the maximum buffer size  and allocates a buffer for it */
	buf_size = fuse_chan_bufsize(fuse_chan);

	if((recvbuf = (char *)malloc(buf_size)) == NULL)
	{
		fprintf(stderr, "Error allocating fuse buffer");
		exit(-1);
	}

	printf("At %s:%d in %s\n", __FILE__, __LINE__, __FUNCTION__);

	/* Processes requests */
	while(!fuse_session_exited(info->se))
	{
		struct fuse_chan *tmpch = fuse_chan;

		/* Gets the next request */
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		result = fuse_chan_recv(&fuse_chan, recvbuf, buf_size);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

		/* Interruption */
		if(result == -EINTR)
		{
			continue;

		/* Error */
		}else if(result <= 0)
		{
			break;
		}

		printf("result = %d\n", result);

		/* Process the fuse request */
		fuse_session_process(info->se, recvbuf, result, tmpch);
	}

	printf("At %s:%d in %s\n", __FILE__, __LINE__, __FUNCTION__);

	/* Done */
	pthread_cond_signal(&(info->done));

	/* Frees the buffer */
	free(recvbuf);

	/* Exited thread */
	return NULL;
}



static int fuse_session_loop_mt_limited(struct fuse_session *se,
										uint32_t numThreads)
{
	/* Declarations */
	pthread_t fuse_threads[numThreads];
	struct fuse_thread_info info =
		{
			.se = se,
			.lock = PTHREAD_MUTEX_INITIALIZER,
			.done = PTHREAD_COND_INITIALIZER
		};

	uint32_t i=0;

	/* Creates the threads */
	for(i=0; i<numThreads; i++)
	{
		if(pthread_create(fuse_threads+i, NULL, fuse_thread_loop, &info) != 0)
		{
			fprintf(stderr, "Error creating fuse worker thread");
			exit(-1);
		}
	}

	/* Wait for a thread to exit */
	pthread_mutex_lock(&(info.lock));
	pthread_cond_wait(&(info.done), &(info.lock));
	pthread_mutex_unlock(&(info.lock));

	/* Kills the thread if possible */
	for(i=0; i<numThreads; i++)
	{
		pthread_cancel(fuse_threads[i]);
		pthread_join(fuse_threads[i], NULL);
	}


	/* Resets session and frees resources */
	pthread_mutex_destroy(&(info.lock));
	pthread_cond_destroy(&(info.done));
	fuse_session_reset(se);

	/* Sets the threads to being reused */
	thread_reuse = true;

	printf("At %s:%d in %s\n", __FILE__, __LINE__, __FUNCTION__);

	return 0;
}





/*
 * Class:     jFUSE_lowlevel_fuselib
 * Method:    mount
 * Signature: (LjFUSE/lowlevel/JFuseFS;Ljava/lang/String;LjFUSE/lowlevel/FuseArgs;Z)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_fuselib_mount(
		JNIEnv *env,
		jclass obj,
		jobject fusefs,
		jstring mountpointString,
		jobject fuseArgs,
		jint    numThreads)
{
	/* Declarations */
	struct fuse_chan *fuse_channel = NULL;
	struct fuse_session *fuse_session = NULL;
	char *mountpoint = NULL;
	char *errorMsg    = NULL;
	jclass   classID = NULL;
	jfieldID fieldID = NULL;
	struct fuse_args *fargs_ptr = NULL;
	int i;



	/* Initialize the fuse arguments */
    struct fuse_args fuse_args = FUSE_ARGS_INIT(0, NULL);

	/* Detects the implemented methods in the fusefs object for fuse and
	 * adds them to fuse function ops for use. */
	if(load_fuse_methods(env, fusefs, &fuse_ops) != 0)
	{
		errorMsg = "Error loading fuse modules";
		goto ERROR_HANDLER;
	}

	/* Gets a cstring for the mount point (should throw an exception) */
	if((mountpoint = (char*)(*env)->GetStringUTFChars(env, mountpointString, NULL)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Handles the fuse args */
	if(fuseArgs == NULL)
	{
		fargs_ptr = &fuse_args;

	}else
	{
		/* Gets the fuse args */
		if(((classID = (*env)->GetObjectClass(env, fuseArgs)) == NULL) ||
		   ((fieldID = (*env)->GetFieldID(env, classID, "fuse_args_ptr", "J")) == NULL))
		{
			goto ERROR_HANDLER;
		}

		fargs_ptr = (struct fuse_args*)(*env)->GetLongField(env, fuseArgs, fieldID);

		if(fargs_ptr == NULL)
		{
			(*env)->ThrowNew(env,
							 (*env)->FindClass(env, "java/lang/NullPointerException"),
							"Fuse args improperly initialized");
			goto ERROR_HANDLER;
		}
	}

	printf("Argc = %d\n", fargs_ptr->argc);


	if(fargs_ptr->argc > 0)
	{
		int i;
		for(i=0; i<fargs_ptr->argc; i++)
		{
			printf("Arg[%d] = %s\n", i, fargs_ptr->argv[i]);
		}

		/* Parses the command linke args */
		if(fuse_parse_cmdline(fargs_ptr, NULL, NULL, NULL) != 0)
		{
			errorMsg = "Error parsing command line args";
			goto ERROR_HANDLER;
		}
	}

	for(i=0; i<fargs_ptr->argc; i++)
	{
		printf("before mount Option %d are %s\n", i, fargs_ptr->argv[i]);
	}

	/* Creates fuse channel for the given mount point */
	fuse_channel = fuse_mount(mountpoint, fargs_ptr);

	if(fuse_channel == NULL)
	{
		errorMsg = "Error occurred while mounting";
		goto ERROR_HANDLER;
	}

	fuse_fd = fuse_chan_fd(fuse_channel);

	for(i=0; i<fargs_ptr->argc; i++)
	{
		printf("Option %d are %s\n", i, fargs_ptr->argv[i]);
	}


	/* Creates a new lowlevel session */
	fuse_session = fuse_lowlevel_new(fargs_ptr, &fuse_ops, sizeof(fuse_ops), NULL);

	if(fuse_session == NULL)
	{
		errorMsg = "Error loading fuse ops";
		goto ERROR_HANDLER;
	}

	/* Adds the fuse channel to the session */
	fuse_session_add_chan(fuse_session, fuse_channel);


	/* Sets the signal handlers */
	if(set_one_signal_handler(SIGHUP,  print_handler) == -1 ||
	  /* set_one_signal_handler(SIGINT,  print_handler) == -1 ||
	   set_one_signal_handler(SIGTERM, print_handler) == -1 || */
	   set_one_signal_handler(SIGPIPE, print_handler) == -1)
	{
		errorMsg = "Error occurred while setting up signal handlers";
		goto ERROR_HANDLER;
	}

	/* Gets the java virtual matchine */
	if((*env)->GetJavaVM(env, &g_vm) != 0)
	{
		goto ERROR_HANDLER;
	}

	/* Sets the global fusefs id */
	if((g_fusefs = (*env)->NewGlobalRef(env, fusefs)) == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Multithreaded */
	if(numThreads < 0)
	{
		printf("Running in multithreaded mode\n");
		if(fuse_session_loop_mt(fuse_session) != 0)
		{
			errorMsg = "fuse loop encountered an error";
			goto ERROR_HANDLER;
		}

	/* Single threaded */
/*	}else if(numThreads == 1)
	{
		printf("Running in single threaded mode\n");

		if(fuse_session_loop(fuse_session) != 0)
		{
			errorMsg = "fuse loop encountered an error";
			goto ERROR_HANDLER;
		}
*/
	/* Limited number of threads */
	}else if(numThreads > 0)
	{
		printf("Running with %d threaded mode\n", numThreads);

		if(fuse_session_loop_mt_limited(fuse_session, numThreads) != 0)
		{
			errorMsg = "fuse loop encountered an error";
			goto ERROR_HANDLER;
		}
	}

	/* Unmount the mount point */
	fuse_unmount(mountpoint, fuse_channel);

	/* Free the string */
	(*env)->ReleaseStringUTFChars(env, mountpointString, mountpoint);

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		/* Throw an exception */
		if(errorMsg != NULL)
		{
			throw_FuseException(env, errorMsg);
		}

		if(mountpoint != NULL)
		{
			(*env)->ReleaseStringUTFChars(env, mountpointString, mountpoint);
		}
}

