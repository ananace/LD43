# 
# External dependencies
# 

if (WIN32)
    find_package(SFML 2 REQUIRED COMPONENTS audio graphics main system window)
    add_definitions(-D_USE_MATH_DEFINES)
else ()
    find_package(SFML 2 REQUIRED COMPONENTS audio graphics system window)
    find_package(FontConfig REQUIRED)
endif ()


# 
# Sources
# 

set(headers
    Application.hpp
    Logging.hpp
    ResourceManager.hpp

    IState.hpp

    States/IntroState.hpp
    States/MenuState.hpp
)

set(sources
    Application.cpp
    Logging.cpp
    ResourceManager.cpp

    States/IntroState.cpp
    States/MenuState.cpp

    main.cpp
)

# Group source files
set(header_group "Header Files")
set(source_group "Source Files")
source_group(${header_group} FILES ${headers})
source_group(${source_group} FILES ${sources})

# 
# Create executable
# 

# Build executable
add_executable(LD43
    MACOSX_BUNDLE
    ${sources}
    ${headers}
)


# 
# Project options
# 

set_target_properties(LD43
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER ""
)


# 
# Include directories
# 

target_include_directories(LD43
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FontConfig_INCLUDE_DIRS}
    ${SFML_INCLUDE_DIR}
)


# 
# Libraries
# 

target_link_libraries(LD43
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${FontConfig_LIBRARIES}
    ${SFML_LIBRARIES}
    ${SFML_DEPENDENCIES}
)


# 
# Compile definitions
# 

target_compile_definitions(LD43
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


# 
# Compile options
# 

target_compile_options(LD43
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


# 
# Linker options
# 

target_link_libraries(LD43
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)


# 
# Install information
# 

install(
    TARGETS
    LD43
    RUNTIME DESTINATION bin
)
