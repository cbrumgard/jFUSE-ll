
cmake_minimum_required(VERSION 2.6)
project(jFUSE Java)


add_subdirectory(lib)


SET(JAVA_SRC fuselib.java JFuseFS.java FuseRequest.java FuseEntryParam.java 
    Mode.java FuseStat.java FileInfo.java FuseCTX.java ConnInfo.java 
    FLock.java FuseException.java FuseErrno.java FuseArgs.java FuseOpt.java
    FuseBuffer.java FuseBufferCopyFlags.java Splice.java)

include_directories(../..)

add_library(jFUSE ${JAVA_SRC})
