SET(REMOTE_DAEMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

## platform or product definition
IF(CMAKE_BUILD_PLATFORM STREQUAL "linux")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DLINUX)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "linux")

IF(CMAKE_BUILD_PLATFORM STREQUAL "mac")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DAPPLE)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "mac")

IF(CMAKE_BUILD_PLATFORM STREQUAL "soundkit")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DLINUX)
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DSOUNDKIT)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "soundkit")

IF(CMAKE_BUILD_PLATFORM STREQUAL "ffos")
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DFFOS)
    # SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DMATCHSTICK)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "ffos")

## remote-daemon-include
SET(remote-daemon-include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${FLINT}/src
    ${LIBS}/rapidjson/include
    ${LIBS}/boost
    ${LIBS}/websocketpp
)

## remote-daemon-src
SET(remote-daemon-src
	${REMOTE_DAEMON_SRC}/main.cpp
	${REMOTE_DAEMON_SRC}/RemoteDaemon.cpp
	${REMOTE_DAEMON_SRC}/InputEventHandler.cpp
	${REMOTE_DAEMON_SRC}/TcpSessionWrapper.cpp
)

## remote-daemon-libs
SET(remote-daemon-libs
    flint_static
)

IF(ANDROID)
    SET(remote-daemon-libs
        ${remote-daemon-libs}
        log
    )
ENDIF(ANDROID)

IF(CMAKE_BUILD_PLATFORM STREQUAL "linux")
	SET(remote-daemon-libs
		${remote-daemon-libs}
		pthread
		)
ENDIF(CMAKE_BUILD_PLATFORM STREQUAL "linux")

IF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")
    SET(remote-daemon-libs
        ${remote-daemon-libs}
        rt
    )
ENDIF(CMAKE_BUILD_PRODUCT STREQUAL "soundkit")

IF(UXIX AND NOT APPLE)
    SET(remote-daemon-libs
        ${remote-daemon-libs}
        rt
    )
ENDIF(UXIX AND NOT APPLE)

IF (APPLE)
    ADD_DEFINITIONS (-DNDEBUG -Wall)
    SET(CMAKE_MACOSX_RPATH 1)
ENDIF(APPLE)

## remote-daemon-cflags
IF (UNIX)
    SET(remote-daemon-cflags
        ${remote-daemon-cflags}
        -Wall
        -frtti
        -fexceptions
        -fPIC
        -D_FILE_OFFSET_BITS=64
        -DBOOST_NO_CXX11_NOEXCEPT
        -DENABLE_CPP11=FALSE
    )

    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        SET(remote-daemon-cflags
            ${remote-daemon-cflags}
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wl,-s
            -Wl,-X
        )
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")
ENDIF(UNIX)


STRING(REPLACE ";" " " remote-daemon-cflags "${remote-daemon-cflags}")

INCLUDE_DIRECTORIES(${remote-daemon-include})

LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

## executable
ADD_EXECUTABLE(remote-daemon ${remote-daemon-src})
TARGET_LINK_LIBRARIES(remote-daemon ${remote-daemon-libs})
SET_TARGET_PROPERTIES(remote-daemon PROPERTIES COMPILE_FLAGS "${remote-daemon-cflags}")