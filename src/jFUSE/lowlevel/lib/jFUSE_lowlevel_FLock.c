
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <jni.h>




static inline struct flock *get_flock(JNIEnv *env, jobject jFlock)
{
	/* Delcarations */
	jclass class = NULL;
	jfieldID fieldID = NULL;


	/* Gets the field id from the object and class */
	if(((class = (*env)->GetObjectClass(env, jFlock)) == NULL) ||
	   ((fieldID = (*env)->GetFieldID(env, class,
			   	   	   	   	   	   	   "_flock_ptr", "J")) == NULL))
	{
		return NULL;
	}

	/* Returns the pointer */
	return (struct flock *)((*env)->GetLongField(env, jFlock, fieldID));
}


/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    _allocate_flock
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FLock__1allocate_1flock
  (JNIEnv *env, jclass flockClass)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Allocate and initialize an flock struct */
	flock = calloc(1, sizeof(struct flock));

	if(flock == NULL)
	{
		(*env)->ThrowNew(env,
			(*env)->FindClass(env, "java/lang/OutOfMemoryError"),
			"Can not allocate a C struct flock from heap");

		return -1;
	}

	return (jlong)flock;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    _free_flock
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FLock__1free_1flock
  (JNIEnv *env, jobject jflock)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jflock);

	if(flock == NULL)
	{
		return;
	}

	free(flock);
}



/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    getType
 * Signature: ()LjFUSE/lowlevel/FLock/Type;
 */
JNIEXPORT jobject JNICALL Java_jFUSE_lowlevel_FLock_getType
  (JNIEnv *env, jobject jFLock)
{
	/* Declarations */
	struct flock *flock = NULL;
	jclass classID = NULL;
	jmethodID valueOfMethodID = NULL;
	jstring   enumName = NULL;
	jobject    enumValue = NULL;

	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		goto ERROR_HANDLER;
	}


	/* Gets the class of the enum type */
	classID = (*env)->FindClass(env, "jFUSE/lowlevel/FLock$Type");

	if(classID == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the static method for valueOf */
	valueOfMethodID = (*env)->GetStaticMethodID(env, classID, "valueOf",
			"(Ljava/lang/String;)LjFUSE/lowlevel/FLock$Type;");


	if(valueOfMethodID == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the enum value based on the type of lock specified */
	switch(flock->l_type)
	{
		case F_RDLCK:
			enumName = (*env)->NewStringUTF(env, "F_RDLCK");
			enumValue = (*env)->CallStaticObjectMethod(env, classID,
									valueOfMethodID, enumName);

			break;

		case F_WRLCK:
			enumName = (*env)->NewStringUTF(env, "F_WRLCK");
			enumValue = (*env)->CallStaticObjectMethod(env, classID,
									valueOfMethodID, enumName);

			break;

		case F_UNLCK:
			enumName = (*env)->NewStringUTF(env, "F_UNLCK");
			enumValue = (*env)->CallStaticObjectMethod(env, classID,
									valueOfMethodID, enumName);
			break;

		default:

			(*env)->ThrowNew(env,
						(*env)->FindClass(env, "java/lang/IllegalArgumentException"),
						"Illegal value for flock type");
			goto ERROR_HANDLER;
	}

	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, enumName);

	/* Returns the enum value */
	return enumValue;


	/* Error handler */
	ERROR_HANDLER:

		return NULL;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    setType
 * Signature: (LjFUSE/lowlevel/FLock/Type;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FLock_setType
  (JNIEnv *env, jobject jFLock, jobject jtype)
{
	/* Declarations */
	struct flock *flock = NULL;
	jclass 		  typeClass = NULL;
	jfieldID	  valueFieldID = NULL;
	jint		  value = -1;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class id of the jtype */
	typeClass = (*env)->GetObjectClass(env, jtype);

	if(typeClass == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the enum value of the type */
	valueFieldID = (*env)->GetFieldID(env, typeClass, "_value", "I");

	if(valueFieldID == NULL)
	{
		goto ERROR_HANDLER;
	}

	value = (*env)->GetIntField(env, jtype, valueFieldID);

	if((*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Checks the validity of the value */
	if(value != F_RDLCK && value != F_WRLCK && value != F_UNLCK)
	{
		(*env)->ThrowNew(env,
				(*env)->FindClass(env,"java/lang/IllegalArgumentException"),
				"Illegal value for Type enum");
		goto ERROR_HANDLER;
	}

	/* Sets the value */
	flock->l_type = value;

	return;


	/* Error handler */
	ERROR_HANDLER:

		return;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    getWhence
 * Signature: ()LjFUSE/lowlevel/FLock/Whence;
 */
JNIEXPORT jobject JNICALL Java_jFUSE_lowlevel_FLock_getWhence
  (JNIEnv *env, jobject jFLock)
{
	/* Declarations */
	struct flock *flock = NULL;
	jclass classID 		= NULL;
	jmethodID valueOfMethodID = NULL;
	jstring  enumName = NULL;
	jobject  enumValue = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class of the enum type */
	classID = (*env)->FindClass(env, "jFUSE/lowlevel/FLock$Whence");

	if(classID == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the static method for valueOf */
	valueOfMethodID = (*env)->GetStaticMethodID(env, classID, "valueOf",
				"(Ljava/lang/String;)LjFUSE/lowlevel/FLock$Whence;");

	if(valueOfMethodID == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the enum value based on the whence specified */
	switch(flock->l_whence)
	{
		case SEEK_SET:
			enumName = (*env)->NewStringUTF(env, "SEEK_SET");
			enumValue = (*env)->CallStaticObjectMethod(env, classID,
												valueOfMethodID, enumName);
			break;

		case SEEK_CUR:
			enumName = (*env)->NewStringUTF(env, "SEEK_CUR");
			enumValue = (*env)->CallStaticObjectMethod(env, classID,
												valueOfMethodID, enumName);
			break;

		case SEEK_END:
			enumName = (*env)->NewStringUTF(env, "SEEK_END");
			enumValue = (*env)->CallStaticObjectMethod(env, classID,
												valueOfMethodID, enumName);
			break;

		default:

			(*env)->ThrowNew(env,
							(*env)->FindClass(env, "java/lang/IllegalArgumentException"),
							"Illegal value for flock whence");
			goto ERROR_HANDLER;
	}


	/* Frees the local reference to the fuse request */
	(*env)->DeleteLocalRef(env, enumName);

	/* Returns the enum value */
	return enumValue;


	/* Error handler */
	ERROR_HANDLER:

		return NULL;


}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    setWhence
 * Signature: (LjFUSE/lowlevel/FLock/Whence;)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FLock_setWhence
  (JNIEnv *env, jobject jFLock, jobject jWhence)
{
	/* Declarations */
	struct flock *flock = NULL;
	jclass 		  whenceClass = NULL;
	jfieldID	  valueFieldID = NULL;
	jint		  value = -1;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the class id of the jWhence*/
	whenceClass = (*env)->GetObjectClass(env, jWhence);

	if(whenceClass == NULL)
	{
		goto ERROR_HANDLER;
	}

	/* Gets the enum value of the jWhence */
	valueFieldID = (*env)->GetFieldID(env, whenceClass, "_value", "I");

	if(valueFieldID == NULL)
	{
		goto ERROR_HANDLER;
	}

	value = (*env)->GetIntField(env, jWhence, valueFieldID);

	if((*env)->ExceptionCheck(env))
	{
		goto ERROR_HANDLER;
	}

	/* Checks the validity of the value */
	if(value != SEEK_CUR && value != SEEK_SET && value != SEEK_END)
	{
		(*env)->ThrowNew(env,
					(*env)->FindClass(env, "java/lang/IllegalArgumentException"),
					"Illegal value for Whence enum");
		goto ERROR_HANDLER;
	}

	/* Sets the value */
	flock->l_whence = value;

	return;


	/* Error handler */
	ERROR_HANDLER:

		return;



}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    getStart
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FLock_getStart
  (JNIEnv *env, jobject jFLock)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		return -1;
	}

	return flock->l_start;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    setStart
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FLock_setStart
  (JNIEnv *env, jobject jFLock, jlong start)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		return;
	}

	flock->l_start = (off_t)start;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    getLen
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FLock_getLen
  (JNIEnv *env, jobject jFLock)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		return -1;
	}

	return flock->l_len;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    setLen
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FLock_setLen
  (JNIEnv *env, jobject jFLock, jlong length)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		return;
	}

	flock->l_len = (off_t)length;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    getPid
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_FLock_getPid
  (JNIEnv *env, jobject jFLock)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		return -1;
	}

	return flock->l_pid;
}

/*
 * Class:     jFUSE_lowlevel_FLock
 * Method:    setPid
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_jFUSE_lowlevel_FLock_setPid
  (JNIEnv *env, jobject jFLock, jlong pid)
{
	/* Declarations */
	struct flock *flock = NULL;


	/* Gets the pointer to the flock */
	flock = get_flock(env, jFLock);

	if(flock == NULL)
	{
		return;
	}

	flock->l_pid = (pid_t)pid;
}


