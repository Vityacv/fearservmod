project(sharedcode CXX)
set (CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE asm_src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.asm)
list(REMOVE_ITEM src "msvc_vars.cpp")

if(MSVC)

    enable_language(ASM_NASM)
    add_library(${PROJECT_NAME}_msvc_vars OBJECT msvc_vars.cpp)
    add_library(${PROJECT_NAME} STATIC ${src} ${asm_src} $<TARGET_OBJECTS:${PROJECT_NAME}_msvc_vars>)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_INVALID_PARAMETER={} _NO_CRT_STDIO_INLINE _CRT_SECURE_NO_WARNINGS _UNICODE UNICODE)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} /EHs-c- /GS- /GR- /Zo- /Zc:sizedDealloc- /Zc:threadSafeInit- ${crt_type} /utf-8>)
    target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

elseif(CMAKE_COMPILER_IS_GNUCXX)

    set_property(SOURCE dll_dependency.S PROPERTY LANGUAGE C)
    add_library(${PROJECT_NAME}_dll_dependency OBJECT dll_dependency.S)
    add_library(${PROJECT_NAME} STATIC ${src} $<TARGET_OBJECTS:${PROJECT_NAME}_dll_dependency>)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE UNICODE __USE_MINGW_ANSI_STDIO=0)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} -static -static-libgcc -static-libstdc++ -fno-ident -s -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -nostdlib -fno-exceptions -fno-sized-deallocation -march=core2 -mfpmath=sse -static -static-libgcc -static-libstdc++ -Os -fomit-frame-pointer -fno-stack-protector -mno-stack-arg-probe -fno-threadsafe-statics -fno-rtti -nostdlib -Wno-pmf-conversions -Wno-narrowing>)
    target_precompile_headers(${PROJECT_NAME} PUBLIC pch.cpp)

endif()

