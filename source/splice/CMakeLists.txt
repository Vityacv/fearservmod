project(splice CXX)
set (CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
enable_language(ASM_NASM)
add_library(splice STATIC ${src} tramp.asm)
target_compile_features(splice PUBLIC cxx_noexcept)

if(MSVC)

    target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_INVALID_PARAMETER={} _NO_CRT_STDIO_INLINE _CRT_SECURE_NO_WARNINGS _UNICODE UNICODE)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} /EHs-c- /GS- /GR- /Zo- /Zc:sizedDealloc- /Zc:threadSafeInit- ${crt_type} /utf-8>)
    target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

elseif(CMAKE_COMPILER_IS_GNUCXX)

    target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE UNICODE __USE_MINGW_ANSI_STDIO=0)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} -static -static-libgcc -static-libstdc++ -fno-ident -s -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -nostdlib -fno-exceptions -fno-sized-deallocation -march=core2 -mfpmath=sse -static -static-libgcc -static-libstdc++  -Os -fomit-frame-pointer -fno-stack-protector -mno-stack-arg-probe -fno-threadsafe-statics -fno-rtti -nostdlib -Wno-pmf-conversions -Wno-narrowing>)
    target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

endif()
