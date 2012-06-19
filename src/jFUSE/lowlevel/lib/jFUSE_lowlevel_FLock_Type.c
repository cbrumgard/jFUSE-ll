
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <jni.h>


/*
 * Class:     jFUSE_lowlevel_FLock_Type
 * Method:    assignValue
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jFUSE_lowlevel_FLock_00024Type_assignValue
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
	if(strcmp(valueName, "F_RDLCK") == 0)
	{
		value = F_RDLCK;
	}else if(strcmp(valueName, "F_WRLCK") == 0)
	{
		value = F_WRLCK;
	}else if(strcmp(valueName, "F_UNLCK") == 0)
	{
		value = F_UNLCK;
	}

	/* Frees the local reference */
	(*env)->ReleaseStringUTFChars(env, jValueName, valueName);

	/* Returns the name */
	return value;
}
