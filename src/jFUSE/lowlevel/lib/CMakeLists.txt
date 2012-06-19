

cmake_minimum_required(VERSION 2.6)
project(JFUSE C)

#LIST(APPEND CMAKE_MODULE_PATH "native/cmake_modules")




FIND_PACKAGE(JNI REQUIRED)
FIND_PACKAGE(FUSE REQUIRED)



MESSAGE(STATUS "${FUSE_CFLAGS}")


include_directories(
    native
    ${FUSE_INCLUDE_DIR}
    /usr/lib/jvm/java-6-openjdk/include/
    /Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/JavaVM.framework/Versions/1.6.0/Headers/
	${JAVA_INCLUDE_PATH}
	${JAVA_INCLUDE_PATH2}
)

link_directories()

message(STATUS ${FUSE_LIBRARIES})
message(STATUS ${FUSE_DEFINITIONS})
message(STATUS ${FUSE_INCLUDE_DIR})
message(STATUS ${JAVA_AWT_LIBRARY})

set(FUSE_VERSION_MAJOR, ${FUSE_VERSION_MAJOR})
set(FUSE_VERSION_MINOR, ${FUSE_VERSION_MINOR})


#SET(CMAKE_C_LDFLAGS "-L${FUSE_LINK_DIR} ${FUSE_LDFLAGS}")
#SET(CMAKE_C_FLAGS  "-g -Wall -D__FreeBSD__=10  -D_FILE_OFFSET_BITS=64 ${FUSE_CFLAGS}")
#SET(CMAKE_C_FLAGS  "-g -Wall  -D_FILE_OFFSET_BITS=64 ${FUSE_CFLAGS}")

SET(CMAKE_C_FLAGS  "-g -Wall ${CMAKE_C_FLAGS}")

add_library(jfuselib SHARED 
            jfuse.c 
            fuse_FileInfo.c 
            fuse_Mode.c 
            fuse_FuseRequest.c 
            jFUSE_lowlevel_ConnInfo.c 
            jFUSE_lowlevel_fuselib.c
            jFUSE_lowlevel_FuseArgs.c 
            fuse_FuseCTX.c 
            jFUSE_lowlevel_FLock.c
            jFUSE_lowlevel_FLock_Type.c 
            jFUSE_lowlevel_FLock_Whence.c
            fuse_util.c 
            fuse_errno.c 
            jFUSE_lowlevel_Splice.c 
            jFUSE_lowlevel_FuseBufferCopyFlags.c)
            
target_link_libraries(jfuselib ${FUSE_LIBRARIES})

message(${CMAKE_SYSTEM_NAME})

set(__LINUX__ "0")
set(__DARWIN__ "0")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-D__FreeBSD__=10  -D_FILE_OFFSET_BITS=64 ${FUSE_DEFINITIONS})
    set(__DARWIN__ "1")
else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(${FUSE_DEFINITIONS})
    
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(__LINUX__ "1")
    ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux") 

endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#add_executable(jfuse_mounter jfuse_mounter.c)
#target_link_libraries(jfuse_mounter ${JNI_LIBRARIES})


configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)
