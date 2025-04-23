project(fearservmod)
set (CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(entry "entry")
    if(MSVC)
        set(MINOSVERSIONMAJOR 5)
        set(MINOSVERSIONMINOR 2)
    elseif(CMAKE_COMPILER_IS_GNUCXX)
    endif()
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(entry "entry")
    if(MSVC)
        set(MINOSVERSIONMAJOR 5)
        set(MINOSVERSIONMINOR 1)
    elseif(CMAKE_COMPILER_IS_GNUCXX)
    endif()
endif()
enable_language(ASM_NASM)
set(entry "DllMain@12")
get_filename_component(LIB_EXE ${CMAKE_LINKER} DIRECTORY)
set(LIB_EXE ${LIB_EXE}/lib)

if(MSVC)

add_library(${PROJECT_NAME} SHARED ${src} impl.asm ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
# set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "StringEditRuntime")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "version")
# add_custom_command(
#     TARGET ${PROJECT_NAME}
#     PRE_LINK
#     COMMAND ${LIB_EXE} -MACHINE:X86 -NOLOGO -DEF:${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def
#     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def
#     )

target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_INVALID_PARAMETER={} _NO_CRT_STDIO_INLINE _CRT_SECURE_NO_WARNINGS _UNICODE UNICODE _STARTUP_CONSOLE)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} /EHs-c- /fp:fast /GS- /GR- /Zo- /Zc:threadSafeInit- ${crt_type} /utf-8>)

target_link_options(${PROJECT_NAME} PUBLIC ${optimization_linker_flags} /NODEFAULTLIB /SAFESEH:NO /RELEASE /NOCOFFGRPINFO /DYNAMICBASE /STACK:0x1000,0x1000 /HEAP:0x10000,0 /SUBSYSTEM:CONSOLE,${MINOSVERSIONMAJOR}.${MINOSVERSIONMINOR} /MANIFEST:NO /ENTRY:${entry}) #${CMAKE_CURRENT_BINARY_DIR}/stringeditruntime_.lib)

target_link_libraries(${PROJECT_NAME} PUBLIC ${crt_lib} ${crt_lib_obj} splice sharedcode kernel32 user32 ws2_32 ntdll)
target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)

elseif(CMAKE_COMPILER_IS_GNUCXX)

add_library(${PROJECT_NAME} SHARED ${src} impl.asm ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def) #${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "winmm")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE UNICODE __USE_MINGW_ANSI_STDIO=0)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "winmm")
    find_program(DLLTOOL dlltool CMAKE_FIND_ROOT_PATH_BOTH)
    if (NOT DLLTOOL)
        message(FATAL_ERROR "Could not find dlltool command")
    endif()
    # add_custom_command(
    #     TARGET ${PROJECT_NAME}
    #     PRE_LINK
    #     COMMAND ${DLLTOOL} -d ${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def -l ${CMAKE_CURRENT_BINARY_DIR}/stringeditruntime_.lib
    #     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def
    #     )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/winmm.lib
        COMMAND ${DLLTOOL} -d ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def -l ${CMAKE_CURRENT_BINARY_DIR}/winmm.lib
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def
        )
    add_custom_target(export_lib ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/winmm.lib
    )
    add_dependencies(${PROJECT_NAME} export_lib)
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE UNICODE __USE_MINGW_ANSI_STDIO=0)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} -municode -mconsole -s -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -mno-stack-arg-probe -static -static-libgcc -static-libstdc++ -fno-ident -s -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -fno-exceptions -fno-sized-deallocation -march=core2 -mfpmath=sse -Os -fomit-frame-pointer -fno-stack-protector -mno-stack-arg-probe -fno-threadsafe-statics -fno-rtti -fcf-protection=none -Wl,--gc-sections,--strip-all${optimization_linker_flags} -Wall>)
    target_link_options(${PROJECT_NAME} PUBLIC -static -static-libgcc -static-libstdc++ -municode -mwindows -s -fno-ident -fno-asynchronous-unwind-tables -mno-stack-arg-probe -fno-exceptions  -Wl,-dll,-pic-executable,-disable-runtime-pseudo-reloc,--major-os-version,5,--minor-os-version,1,--major-image-version,0,--minor-image-version,0,--major-subsystem-version,5,--minor-subsystem-version,1,-image-base=0x10000000,-dynamicbase,-nxcompat)
    target_link_libraries(${PROJECT_NAME} PUBLIC sharedcode ntdll kernel32 user32 ws2_32 splice msvcrt ${CMAKE_CURRENT_BINARY_DIR}/winmm.lib)
    target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
else()

add_library(${PROJECT_NAME} SHARED ${src} impl.asm ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def) #${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def)
# add_library(${PROJECT_NAME} SHARED start.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "winmm")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE UNICODE __USE_MINGW_ANSI_STDIO=0)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "winmm")
    find_program(DLLTOOL dlltool CMAKE_FIND_ROOT_PATH_BOTH)
    if (NOT DLLTOOL)
        message(FATAL_ERROR "Could not find dlltool command")
    endif()
    # add_custom_command(
    #     TARGET ${PROJECT_NAME}
    #     PRE_LINK
    #     COMMAND ${DLLTOOL} -d ${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def -l ${CMAKE_CURRENT_BINARY_DIR}/stringeditruntime_.lib
    #     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/stringeditruntime_.def
    #     )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/winmm.lib
        COMMAND ${DLLTOOL} -d ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def -l ${CMAKE_CURRENT_BINARY_DIR}/winmm.lib
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/winmm.def
        )
    add_custom_target(export_lib ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/winmm.lib
    )
    add_dependencies(${PROJECT_NAME} export_lib)
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _UNICODE UNICODE __USE_MINGW_ANSI_STDIO=0)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${optimization_compiler_flags} -municode -mconsole -s -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -mno-stack-arg-probe -static -s -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -fno-exceptions -fno-sized-deallocation -march=core2 -mfpmath=sse -Os -fomit-frame-pointer -fno-stack-protector -mno-stack-arg-probe -fno-threadsafe-statics -fno-rtti -fcf-protection=none -Wl,--gc-sections,--strip-all${optimization_linker_flags} -Wall>)
    target_link_options(${PROJECT_NAME} PUBLIC -static -municode -mwindows -s -fno-asynchronous-unwind-tables -mno-stack-arg-probe -fno-exceptions  -Wl,-dll,-pic-executable,-disable-runtime-pseudo-reloc,--major-os-version,5,--minor-os-version,1,--major-image-version,0,--minor-image-version,0,--major-subsystem-version,5,--minor-subsystem-version,1,-image-base=0x10000000,-dynamicbase,-nxcompat)
    # target_link_libraries(${PROJECT_NAME} PUBLIC sharedcode ntdll user32 ws2_32 kernel32 splice)
    target_link_libraries(${PROJECT_NAME} PUBLIC sharedcode ntdll kernel32 user32 ws2_32 splice ${CMAKE_CURRENT_BINARY_DIR}/winmm.lib)
    target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
endif()
