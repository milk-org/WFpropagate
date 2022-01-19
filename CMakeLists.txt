set(LIBNAME "milkWFpropagate")

# list source files (.c) other than modulename.c
set(SOURCEFILES
	Fresnel_propagate.c
	Fresnel_propagate_cube.c
)

# list include files (.h) that should be installed on system
set(INCLUDEFILES
	)

# list scripts that should be installed on system
set(SCRIPTFILES
	)





# DEFAULT SETTINGS
# Do not change unless needed
# =====================================================================

# SRCNAME is current directory (last subdir)
set(NAME0 "${CMAKE_CURRENT_SOURCE_DIR}")
string(REPLACE "/" " " NAME1 ${NAME0})
string(REGEX MATCH "[a-zA-Z0-9_]+$" SRCNAME "${NAME0}")

message("")
message(" SRCNAME = ${SRCNAME} -> LIBNAME = ${LIBNAME}")

project(lib_${LIBNAME}_project)

add_library(${LIBNAME} SHARED ${SRCNAME}.c ${SOURCEFILES})

target_include_directories(${LIBNAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)
target_link_libraries(${LIBNAME} PUBLIC CLIcore)


install(TARGETS ${LIBNAME} DESTINATION lib)
install(FILES ${SRCNAME}.h ${INCLUDEFILES} DESTINATION include/${SRCNAME})

install(PROGRAMS
	${SCRIPTFILES}
	DESTINATION bin)
