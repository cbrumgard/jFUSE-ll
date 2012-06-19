/*
 * jFUSE_lowlevel_Splice.c
 *
 *  Created on: Jun 4, 2012
 *      Author: brumgard
 */


#define _GNU_SOURCE		1
#define __USE_GNU		1


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "jni.h"

#include "config.h"




#ifdef __LINUX__

static int _getFDFromFileDescriptor(JNIEnv *env, jobject jfileDescriptor)
{
	return (*env)->GetIntField(env, jfileDescriptor, (*env)->GetFieldID(env,
				(*env)->GetObjectClass(env, jfileDescriptor),"fd","I"));
}
#endif


/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    _SPLICE_F_MOVE
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_Splice__1SPLICE_1F_1MOVE
  (JNIEnv *env, jclass jspliceClass)
{
	#ifdef SPLICE_F_MOVE
		return SPLICE_F_MOVE;
	#else
		return 0;
	#endif
}

/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    _SPLICE_F_NONBLOCK
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_Splice__1SPLICE_1F_1NONBLOCK
  (JNIEnv *env, jclass jspliceClass)
{
	#ifdef SPLICE_F_NONBLOCK
		return SPLICE_F_NONBLOCK;
	#else
		return 0;
	#endif
}

/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    _SPLICE_F_MORE
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_Splice__1SPLICE_1F_1MORE
  (JNIEnv *env, jclass jspliceClass)
{
	#ifdef SPLICE_F_MORE
		return SPLICE_F_MORE;
	#else
		return 0;
	#endif
}

/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    _SPLICE_F_GIFT
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_Splice__1SPLICE_1F_1GIFT
  (JNIEnv *env, jclass jspliceClass)
{
	#ifdef SPLICE_F_GIFT
		return SPLICE_F_GIFT;
	#else
		return 0;
	#endif
}



/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    createPipeFileDescriptor
 * Signature: ()LjFUSE/lowlevel/Splice/Pipe;
 */
JNIEXPORT jobject JNICALL Java_jFUSE_lowlevel_Splice_createPipeFileDescriptor
  (JNIEnv *env, jclass jSpliceClass)
{
	/* Declarations */
	int pipefds[2] = { -1, -1 };

	/* Creates the pipe */
	if(pipe(pipefds) != 0)
	{
		(*env)->ThrowNew(env, (*env)->FindClass(env, "java/io/IOException"),
		    			strerror(errno));
		goto ERROR_HANDLER;
	}


	/* Gets the file descriptor class */
	jclass fileDescriptorClass = (*env)->FindClass(env, "java/io/FileDescriptor");

	if(fileDescriptorClass == NULL || (*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the constructor ID */
	jmethodID constructorID = (*env)->GetMethodID(env, fileDescriptorClass,
									"<init>", "()V");

	if(constructorID == NULL || (*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Creates instances of the file descriptor */
	jobject jreadFileDescriptor = (*env)->NewObject(env,
										fileDescriptorClass, constructorID);
	jobject jwriteFileDescriptor = (*env)->NewObject(env,
										fileDescriptorClass, constructorID);

	if(jreadFileDescriptor == NULL || jwriteFileDescriptor == NULL ||
			(*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the access method to set the internal file descriptors */
	jmethodID accessMethod = (*env)->GetStaticMethodID(env,
										fileDescriptorClass, "access$002",
										"(Ljava/io/FileDescriptor;I)I");

	if((*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}


	/* Sets the internal pipe file descriptors for the two file
	 * descriptor objects */
	(*env)->CallStaticIntMethod(env, fileDescriptorClass, accessMethod,
								jreadFileDescriptor, pipefds[0]);
	(*env)->CallStaticIntMethod(env, fileDescriptorClass, accessMethod,
									jwriteFileDescriptor, pipefds[1]);

	if((*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class of the pipe */
	jclass pipeClass = (*env)->FindClass(env, "jFUSE/lowlevel/Splice$Pipe");

	if(pipeClass == NULL || (*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Gets the constructor for the pipe object */
	jmethodID pipeConstructor = (*env)->GetMethodID(env, pipeClass, "<init>",
			"(Ljava/io/FileDescriptor;Ljava/io/FileDescriptor;)V");

	if(pipeConstructor == NULL || (*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Creates the pipe object */
	jobject jpipe = (*env)->NewObject(env, pipeClass, pipeConstructor,
									jreadFileDescriptor, jwriteFileDescriptor);

	if(jpipe == NULL || (*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Returns the pipe object */
	return jpipe;


	/* Error Handler */
	ERROR_HANDLER:

		if(pipefds[0] != -1) { close(pipefds[0]); }
		if(pipefds[1] != -1) { close(pipefds[1]); }

		return NULL;
}

/*
 * Class:     jFUSE_lowlevel_Splice_Pipe
 * Method:    _setSize
 * Signature: (Ljava/io/FileDescriptor;J)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_Splice_00024Pipe__1setSize
  (JNIEnv *env, jobject jpipe, jobject jfileDescriptor, jlong jsize)
{
	#if __LINUX__ != 0
		#ifdef F_GETPIPE_SZ
			int fd = _getFDFromFileDescriptor(env, jfileDescriptor);
			fcntl(fd, F_SETPIPE_SZ, jsize);
		#endif
	#endif
}

/*
 * Class:     jFUSE_lowlevel_Splice_Pipe
 * Method:    _getSize
 * Signature: (Ljava/io/FileDescriptor;)J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_Splice_00024Pipe__1getSize
  (JNIEnv *env, jobject jpipe, jobject jfileDescriptor)
{
	/* Declarations */


	/* Returns the pipe size */
	#if __LINUX__ != 0

		/* Use fcntl to get the size of the pipe */
		#ifdef F_GETPIPE_SZ

			/* Gets the internal file descriptor */
			int fd = _getFDFromFileDescriptor(env, jfileDescriptor);

			printf("From fcntl\n");
			return fcntl(fd, F_GETPIPE_SZ);

		/* Default constant, pipe may be larger but PIPE_BUF is the
		 * safe mininum to assume */
		#else

			#include "/usr/include/linux/version.h"
			#include "/usr/include/linux/limits.h"

			printf("Not from fcntl\n");
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
				return 	16*PIPE_BUF;
			#else
				return PIPE_BUF;
			#endif

		#endif


	#elif __DARWIN__ != 0
		#include "/usr/include/sys/types.h"
		#include "/usr/include/sys/pipe.h"
		return PIPE_SIZE;
	#endif
}





/*
 * Class:     jFUSE_lowlevel_Splice_Pipe
 * Method:    _close
 * Signature: (Ljava/io/FileDescriptor;Ljava/io/FileDescriptor;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_Splice_00024Pipe__1close
  (JNIEnv *env, jobject jpipe, jobject jinputFD, jobject joutputFD)
{
	/* Declarations */
	int inputFD = -1;
	int outputFD = -1;




	/* Gets the unix file descriptors */
	inputFD  = _getFDFromFileDescriptor(env, jinputFD);
	outputFD = _getFDFromFileDescriptor(env, joutputFD);

	/* Closes them */
	close(inputFD);
	close(outputFD);
}


/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    splice
 * Signature: (Ljava/io/FileDescriptor;JLjava/io/FileDescriptor;JJI)J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_Splice_splice
  (JNIEnv *env, jclass jSpliceClass, jobject jinputFD, jlong jinputOffset,
   jobject joutputFD, jlong joutputOffset, jlong length, jint flags)
{

/* Not linux so throw UnSupportException */
#if __LINUX__ == 0

	(*env)->ThrowNew(env,
			(*env)->FindClass(env, "java/lang/UnsupportedOperationException"),
			"Splice is not supported on this platform");
	return 0;

#else

	/* Declarations */
	int inputFD = -1;
	int outputFD = -1;
	loff_t *inputOffset  =  (jinputOffset > -1) ? &jinputOffset : NULL;
	loff_t *outputOffset =  (joutputOffset > -1) ? &joutputOffset : NULL;
	jlong retval   = -1;


	/* Gets the input fds */
	inputFD  = _getFDFromFileDescriptor(env, jinputFD);
	outputFD = _getFDFromFileDescriptor(env, joutputFD);

	if((*env)->ExceptionCheck(env))
	{
		return -1;
	}


	/* Splices the inputFD to the outputFD */
	retval = splice(inputFD, inputOffset, outputFD, outputOffset, length,
					(int)flags);



	/* Error */
	if(retval < 0)
	{
		(*env)->ThrowNew(env, (*env)->FindClass(env, "java/io/IOException"),
		    			strerror(errno));
		goto ERROR_HANDLER;
	}

	/* Return the value from splice */
	return retval;


	/* Error Handler */
	ERROR_HANDLER:

		/* Error occurred should have an exception ready */
		return -1;

#endif
}


/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    tee
 * Signature: (Ljava/io/FileDescriptor;Ljava/io/FileDescriptor;JI)J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_Splice_tee
  (JNIEnv *env, jclass jSpliceClass, jobject jinputFD, jobject joutputFD,
		   jlong length, jint flags)
{
/* Not linux so throw UnSupportException */
#if __LINUX__ == 0

	(*env)->ThrowNew(env,
			(*env)->FindClass(env, "java/lang/UnsupportedOperationException"),
			"Tee is not supported on this platform");
	return 0;

#else

	/* Declarations */
	int inputFD = -1;
	int outputFD = -1;
	ssize_t amtTransferred;


	/* Gets the input fds */
	inputFD  = _getFDFromFileDescriptor(env, jinputFD);
	outputFD = _getFDFromFileDescriptor(env, joutputFD);


	if((*env)->ExceptionCheck(env))
	{
		return -1;
	}

	/* Connect the to pipes
	 * (the read end of the first one to the write end of the second) */
	amtTransferred = tee(inputFD, outputFD, length, (int)flags);

	if(amtTransferred < 0)
	{
		(*env)->ThrowNew(env, (*env)->FindClass(env, "java/io/IOException"),
				strerror(errno));
		return -1;
	}

	/* Return the result */
	return amtTransferred;

#endif
}


/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    getFileDescriptorFromSocket
 * Signature: (Ljava/net/Socket;)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_jFUSE_lowlevel_Splice_getFileDescriptorFromSocket
  (JNIEnv *env, jclass jSpliceClass, jobject jsocket)
{
	/* Declarations */
	jobject jsocketImpl = NULL;


	/* Gets the internal socket implementation */
	jsocketImpl = (*env)->GetObjectField(env, jsocket,
							(*env)->GetFieldID(env,
									(*env)->GetObjectClass(env, jsocket),
									"impl",
									"Ljava/net/SocketImpl;"));

	printf("jsocketImpl = %p\n", jsocketImpl);


	printf("jsocketImpl class = %p\n", (*env)->GetObjectClass(env,jsocketImpl));

	printf("field ID = %p\n", (*env)->GetFieldID(env,
						(*env)->GetObjectClass(env,jsocketImpl),
						"fd",
						"Ljava/io/FileDescriptor;"));


	jobject obj = (*env)->CallObjectMethod(env, jsocketImpl,
			(*env)->GetMethodID(env, (*env)->GetObjectClass(env,jsocketImpl),
					"getFileDescriptor", "()Ljava/io/FileDescriptor;"));

	printf("obj = %p\n", obj);

	(*env)->ExceptionDescribe(env);


	/* Returns the file descriptor object */
	return (*env)->GetObjectField(env, jsocketImpl,
						(*env)->GetFieldID(env,
									(*env)->GetObjectClass(env,jsocketImpl),
									"fd",
									"Ljava/io/FileDescriptor;"));
}



/*
 * Class:     jFUSE_lowlevel_Splice
 * Method:    getFileDescriptorFromSocketChannel
 * Signature: (Ljava/nio/channels/SocketChannel;)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_jFUSE_lowlevel_Splice_getFileDescriptorFromSocketChannel
  (JNIEnv *env, jclass jSpliceClass, jobject jsocketChannel)
{
	//jclass clazz = (*env)->FindClass(env, "sun/nio/ch/SocketChannelImpl");

	/* Gets the field id  of the file descriptor */
	jfieldID jfileDescriptorfieldID =
				(*env)->GetFieldID(env,
							(*env)->GetObjectClass(env, jsocketChannel),
							"fd",
							"Ljava/io/FileDescriptor;");


	/* Error, no such field */
	if(jfileDescriptorfieldID == NULL || (*env)->ExceptionCheck(env))
	{
		return NULL;
	}

	/* Grabs the file descriptor object */
	return (*env)->GetObjectField(env, jsocketChannel, jfileDescriptorfieldID);
}















