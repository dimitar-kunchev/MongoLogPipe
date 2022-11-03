project(MongoLogPipe)
find_package(PkgConfig REQUIRED)

cmake_minimum_required(VERSION 3.13)

add_executable(mongoLogPipe mongoLogPipe.c)

pkg_check_modules(MONGOC REQUIRED libmongoc-1.0)
target_link_libraries(mongoLogPipe ${MONGOC_LIBRARIES}) 
target_include_directories(mongoLogPipe PUBLIC ${MONGOC_INCLUDE_DIRS})
target_compile_options(mongoLogPipe PUBLIC ${MONGOC_CFLAGS_OTHER})

pkg_check_modules(LIBCONFIG REQUIRED libconfig)
target_link_libraries(mongoLogPipe ${LIBCONFIG_LIBRARIES}) 
target_include_directories(mongoLogPipe PUBLIC ${LIBCONFIG_INCLUDE_DIRS})
target_compile_options(mongoLogPipe PUBLIC ${LIBCONFIG_CFLAGS_OTHER})
