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
#include <stdio.h>

#include "jni.h"


#define LIBPATH   "../../JFUSE/src/native"
#define CLASSPATH "./:../../JFUSE/src/fuse/jfuse.jar:../deps/lsync-0.0.1-SNAPSHOT.jar"

int main(int argc, char *argv[])
{
	/* Declarations */
	JavaVM 		   *jvm		= NULL;
	JNIEnv		   *env		= NULL;
	JavaVMOption   options[2];
	JavaVMInitArgs vm_args;
	jclass 		   mainClass = NULL;
	jmethodID 	   mainMethod = NULL;
	jobjectArray   args       = NULL;


	/* Builds the options */
	options[0].optionString = "-Djava.library.path=" LIBPATH;
	options[1].optionString = "-Djava.class.path=" CLASSPATH;

	/* Builds the args */
	vm_args.version = 0x00010002;
	vm_args.options = options;
	vm_args.nOptions = 2;
	vm_args.ignoreUnrecognized = JNI_TRUE;


	/* Creates a java jvm */
	if(JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args) != 0)
	{
		printf("HERE1\n");
		goto ERROR_HANDLER;
	}

	/* Gets the main class and method*/
	if(((mainClass = (*env)->FindClass(env, "bastardfs/BastardFS")) == NULL) ||
	   ((mainMethod = (*env)->GetStaticMethodID(env, mainClass, "main",
	                                     "([Ljava/lang/String;)V")) == NULL))
	{
		printf("HERE2\n");
		goto ERROR_HANDLER;
	}


	/* Builds the arg list to the program */
	args = (*env)->NewObjectArray(env, 6, (*env)->FindClass(env, "java/lang/String"),
			(*env)->NewStringUTF(env, "--threads"));

	if(args == NULL)
	{
		printf("HERE3\n");
		goto ERROR_HANDLER;
	}

	/* Sets the elements of the args */
	(*env)->SetObjectArrayElement(env, args, 0,
			(*env)->NewStringUTF(env, "--threads"));
	(*env)->SetObjectArrayElement(env, args, 1,
			(*env)->NewStringUTF(env, "10"));
	(*env)->SetObjectArrayElement(env, args, 2,
			(*env)->NewStringUTF(env, "--configfile"));
	(*env)->SetObjectArrayElement(env, args, 3,
			(*env)->NewStringUTF(env, "bastardfs.properties"));
	(*env)->SetObjectArrayElement(env, args, 4,
			(*env)->NewStringUTF(env, "chris-brumgard-desktop.accre.vanderbilt.edu"));
	(*env)->SetObjectArrayElement(env, args, 5,
			(*env)->NewStringUTF(env, "tmp"));



	/* Calls the main method for the class */
	(*env)->CallStaticVoidMethod(env, mainClass, mainMethod, args);


	/* Frees the jvm */
	(*jvm)->DestroyJavaVM(jvm);

	/* Return success */
	return EXIT_SUCCESS;


	/* Error handler */
	ERROR_HANDLER:

		if(jvm != NULL)
		{
			if(env != NULL && (*env)->ExceptionOccurred(env))
			{
				(*env)->ExceptionOccurred(env);
			}

			(*jvm)->DestroyJavaVM(jvm);
		}

		/* Error code */
		return -1;
}
