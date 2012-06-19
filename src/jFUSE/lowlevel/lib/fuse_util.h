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

#ifndef FUSE_UTIL_H_
#define FUSE_UTIL_H_


#include "jni.h"


#define FUSE_USE_VERSION 28



void throw_FuseException(JNIEnv *env, const char *msg);
fuse_req_t getFuseRequest(JNIEnv *env, jobject fuseRequest);
int convertFuseStat(JNIEnv *env, jobject fuseStat, struct stat *statbuf);
jobject new_fuse_request(JNIEnv *env, fuse_req_t req);
jobject new_fuse_stat(JNIEnv *env, struct stat *statbuf);
int convert_to_native_fuse_param(JNIEnv *env, jobject fuseEntryParam,
								struct fuse_entry_param *param);
jobject new_fuse_file_info(JNIEnv *env, struct fuse_file_info *fi);
struct fuse_file_info *convert_to_native_fuse_file_info(
			JNIEnv *env, jobject fuseInfo);
jobject new_fuse_flock(JNIEnv *env, struct flock *flock);
struct flock *convert_to_native_flock(JNIEnv *env, jobject flock);
jobject make_byte_array_for_buffer(JNIEnv *env, void *buf,  jlong size);


#endif /* FUSE_UTIL_H_ */
