cmake_minimum_required (VERSION 3.16)

project( android_uuid C)

set( SOURCES
    copy.c
    gen_uuid.c
    isnull.c
    pack.c
    parse.c
    unpack.c
    unparse.c
    uuid_time.c
)

add_library( android_uuid STATIC ${SOURCES})

set_target_properties(android_uuid PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(android_uuid PUBLIC .)

install( TARGETS android_uuid DESTINATION "lib")
