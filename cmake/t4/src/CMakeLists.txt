ADD_EXECUTABLE(main main.c)
INCLUDE_DIRECTORIES(/usr/local/include/hello)
TARGET_LINK_LIBRARIES(main /usr/local/lib/libhello.dylib)