/*
 * jFUSE_lowlevel_FuseArgs.c
 *
 *  Created on: Jan 19, 2011
 *      Author: brumgard
 */

#include <stdlib.h>
#include <string.h>



#define FUSE_USE_VERSION 28

#include "fuse.h"
#include "fuse/fuse_lowlevel.h"
#include "fuse/fuse_lowlevel_compat.h"
#include "fuse/fuse_opt.h"

#include "jFUSE_lowlevel_FuseArgs.h"
#include "fuse_util.h"


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Class:     jFUSE_lowlevel_FuseArgs
 * Method:    _initializeFuseArgs
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseArgs__1initializeFuseArgs
  (JNIEnv *env, jobject fuseArgs, jobjectArray args)
{
	/* Declarations */
	jclass 		classid 	= NULL;
	jfieldID 	fieldid 	= NULL;
	jsize       argc        = 0;
	char       **argv       = NULL;
	char        *cstr       = NULL;
	char        *cstrcpy    = NULL;
	jsize       i;
	struct fuse_args *args_ptr = NULL;


	if(args != NULL)
	{
		argc = (*env)->GetArrayLength(env, args);
	}

	if(argc > 0)
	{
		if((argv = malloc(argc * sizeof(char*))) == NULL)
		{
			(*env)->ThrowNew(env,
							(*env)->FindClass(env, "java/lang/NullPointerException"),
							"Malloc allocation failure");
			goto ERROR_HANDLER;
		}


		for(i=0; i<argc; i++)
		{
			/* Gets the arg string */
			jstring arg = (*env)->GetObjectArrayElement(env, args, i);

			cstr = (char*)(*env)->GetStringUTFChars(env, arg, 0);

			if(cstr == NULL)
			{
				goto ERROR_HANDLER;
			}


			/* Copies the c string */
			if((cstrcpy = strdup(cstr)) == NULL)
			{
				(*env)->ThrowNew(env,
								 (*env)->FindClass(env, "java/lang/NullPointerException"),
								"Malloc allocation failure");
				goto ERROR_HANDLER;
			}

			/* Stories the array */
			argv[i] = cstrcpy;

			/* Release the reference to the char */
			(*env)->ReleaseStringUTFChars(env, arg, cstr);
		}
	}else
	{
		argc = 0;
		argv = NULL;
	}

	/* Initialize the fuse structs */
	struct fuse_args fargs = FUSE_ARGS_INIT(argc, argv);

	/* Allocats the native fuse args*/
	args_ptr = malloc(sizeof(struct fuse_args));

	/* Throws an exception if the malloc fails */
	if(args_ptr == NULL)
	{
		(*env)->ThrowNew(env,
				(*env)->FindClass(env, "java/lang/OutOfMemoryError"),
				"Malloc allocation failure");
		goto ERROR_HANDLER;
	}

	/* Copies the contents */
	memcpy(args_ptr, &fargs, sizeof(struct fuse_args));

	/* Gets the fuse args ptr */
	if(((classid = (*env)->GetObjectClass(env, fuseArgs)) == NULL) ||
	   ((fieldid = (*env)->GetFieldID(env, classid, "fuse_args_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Sets the pointer */
	(*env)->SetLongField(env, fuseArgs, fieldid, (jlong)args_ptr);

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:

		if(args_ptr != NULL)
		{
			free(args_ptr);
		}

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
}



/*
 * Class:     jFUSE_lowlevel_FuseArgs
 * Method:    _freeFuseArgs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseArgs__1freeFuseArgs
  (JNIEnv *env, jobject fuseArgs)
{
	/* Declarations */
	jclass 		classid 	= NULL;
	jfieldID 	fieldid 	= NULL;
	struct fuse_args *args 	= NULL;
	int    i;

	/* Gets the fuse args ptr */
	if(((classid = (*env)->GetObjectClass(env, fuseArgs)) == NULL) ||
	  ((fieldid = (*env)->GetFieldID(env, classid, "fuse_args_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the pointer */
	args = (struct fuse_args*)(*env)->GetLongField(env, fuseArgs, fieldid);

	/* Frees the memory */
	if(args != NULL)
	{
		for(i=0; i<args->argc; i++)
		{
			free(args->argv[i]);
		}

		free(args);
	}

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
 * Class:     jFUSE_lowlevel_FuseArgs
 * Method:    addArg
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseArgs_addArg
  (JNIEnv *env, jobject fuseArgs, jstring arg)
{
	/* Declarations */
	jclass classid   = NULL;
	jfieldID fieldid = NULL;
	char  *cstring   = NULL;
	char  *strcopy   = NULL;
	struct fuse_args *fuse_args_ptr = NULL;



	/* Gets the fuse args from the object */
	if(((classid = (*env)->GetObjectClass(env, fuseArgs)) == NULL) ||
	   ((fieldid = (*env)->GetFieldID(env, classid, "fuse_args_ptr", "J")) == NULL))
	{
		goto ERROR_HANDLER;
	}

	fuse_args_ptr = (struct fuse_args*)(*env)->GetLongField(env, fuseArgs, fieldid);

	/* Checks the pointer */
	if(fuse_args_ptr == NULL)
	{
		(*env)->ThrowNew(env,
						(*env)->FindClass(env, "java/lang/NullPointerException"),
						"Malloc allocation failure");
		goto ERROR_HANDLER;
	}

	/* Gets the cstring */
	cstring = (char*)(*env)->GetStringUTFChars(env, arg, 0);

	if(cstring == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Makes an independent copy of the string */
	strcopy = strdup(cstring);

	if(strcopy == NULL)
	{
		(*env)->ThrowNew(env,
						(*env)->FindClass(env, "java/lang/OutOfMemoryError"),
						"Malloc allocation failure");
		goto ERROR_HANDLER;
	}


	/* Adds the option */
	fuse_opt_add_arg(fuse_args_ptr, strcopy);


	/* Releases the cstring */
	(*env)->ReleaseStringUTFChars(env, arg, cstring);

	/* Return */
	return;


	/* Error Handler */
	ERROR_HANDLER:


		/* Free resources */
		if(cstring != NULL)
		{
			(*env)->ReleaseStringUTFChars(env, arg, cstring);

			if(strcopy != NULL)
			{
				free(strcopy);
			}
		}

		if((*env)->ExceptionCheck(env) == JNI_TRUE)
		{
			(*env)->ExceptionDescribe(env);
		}

		return;
}

/*
 * Class:     jFUSE_lowlevel_FuseArgs
 * Method:    optParse
 * Signature: ([LjFUSE/lowlevel/FuseOpt;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FuseArgs_optParse
  (JNIEnv *env, jobject fuseArgs, jobjectArray fuseOpts)
{


}

#ifdef __cplusplus
}
#endif










