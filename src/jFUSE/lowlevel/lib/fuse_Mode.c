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
#include <string.h>
#include <sys/stat.h>

#include "jni.h"

JNIEXPORT jlong JNICALL Java_jFUSE_lowlevel_Mode_getModeConstants
  (JNIEnv *env, jclass Mode, jstring constantName)
{
	/* Declarations */
	char *cstring = NULL;
	jlong val     = 0;


	/* Gets the cstring */
	cstring = (char *)(*env)->GetStringUTFChars(env, constantName, NULL);

	if(cstring == NULL)
	{
		return -1;
	}

	if(strcmp(cstring, "S_IFMT") == 0)
	{
		val = S_IFMT;
	}else if(strcmp(cstring, "S_IFSOCK") == 0)
	{
		val = S_IFSOCK;
	}else if(strcmp(cstring, "S_IFLNK") == 0)
	{
		val = S_IFLNK;
	}else if(strcmp(cstring, "S_IFREG") == 0)
	{
		val = S_IFREG;
	}else if(strcmp(cstring, "S_IFBLK") == 0)
	{
		val = S_IFBLK;
	}else if(strcmp(cstring, "S_IFDIR") == 0)
	{
		val = S_IFDIR;
	}else if(strcmp(cstring, "S_IFCHR") == 0)
	{
		val = S_IFCHR;
	}else if(strcmp(cstring, "S_IFIFO") == 0)
	{
		val = S_IFIFO;
	}else if(strcmp(cstring, "S_ISUID") == 0)
	{
		val = S_ISUID;
	}else if(strcmp(cstring, "S_ISGID") == 0)
	{
		val = S_ISGID;
	}else if(strcmp(cstring, "S_ISVTX") == 0)
	{
		val = S_ISVTX;
	}else if(strcmp(cstring, "S_IRWXU") == 0)
	{
		val = S_IRWXU;
	}else if(strcmp(cstring, "S_IRUSR") == 0)
	{
		val = S_IRUSR;
	}else if(strcmp(cstring, "S_IWUSR") == 0)
	{
		val = S_IWUSR;
	}else if(strcmp(cstring, "S_IXUSR") == 0)
	{
		val = S_IXUSR;
	}else if(strcmp(cstring, "S_IRWXG") == 0)
	{
		val = S_IRWXG;
	}else if(strcmp(cstring, "S_IRGRP") == 0)
	{
		val = S_IRGRP;
	}else if(strcmp(cstring, "S_IWGRP") == 0)
	{
		val = S_IWGRP;
	}else if(strcmp(cstring, "S_IXGRP") == 0)
	{
		val = S_IXGRP;
	}else if(strcmp(cstring, "S_IRWXO") == 0)
	{
		val = S_IRWXO;
	}else if(strcmp(cstring, "S_IROTH") == 0)
	{
		val = S_IROTH;
	}else if(strcmp(cstring, "S_IWOTH") == 0)
	{
		val = S_IWOTH;
	}else if(strcmp(cstring, "S_IXOTH") == 0)
	{
		val = S_IXOTH;

	}

	/* Frees the local reference */
	(*env)->ReleaseStringUTFChars(env, constantName, cstring);

	/* Returns the value */
	return val;
}
