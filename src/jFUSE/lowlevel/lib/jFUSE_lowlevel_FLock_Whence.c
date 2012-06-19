
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <jni.h>

/*
 * Class:     jFUSE_lowlevel_FLock_Whence
 * Method:    assignValue
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_FLock_00024Whence_assignValue
	(JNIEnv *env, jclass jFlockType, jstring jValueName)
{
	/* Declarations */
	char *valueName = NULL;
	jint  value     = -1;


	/* Gets the value's name as a cstring */
	valueName = (char *)(*env)->GetStringUTFChars(env, jValueName, NULL);

	if(valueName == NULL)
	{
		return value;
	}

	/* Gets the constant value for the value name */
	if(strcmp(valueName, "SEEK_SET") == 0)
	{
		value = SEEK_SET;
	}else if(strcmp(valueName, "SEEK_CUR") == 0)
	{
		value = SEEK_CUR;
	}else if(strcmp(valueName, "SEEK_END") == 0)
	{
		value = SEEK_END;
	}

	/* Frees the local reference */
	(*env)->ReleaseStringUTFChars(env, jValueName, valueName);

	/* Returns the name */
	return value;
}
