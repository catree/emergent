#####################################
##	MAKETA suport for cmake

SET(MAKETA_FLAGS -hx -css -cpp=\"g++ -E\"
-I${PROJECT_SOURCE_DIR}/src/temt/taiqtso
-I${PROJECT_SOURCE_DIR}/src/temt/ta/ios-g++-3.1
-I${PROJECT_SOURCE_DIR}/src/temt/ta
-I${PROJECT_SOURCE_DIR}/src/temt/css
-I${PROJECT_SOURCE_DIR}/src/emergent/network
-I${PROJECT_SOURCE_DIR}/src/emergent/leabra
-I${PROJECT_SOURCE_DIR}/src/emergent/bp
)

MACRO (MAKETA_GET_INC_DIRS _MAKETA_INC_DIRS)
   SET(${_MAKETA_INC_DIRS})
   GET_DIRECTORY_PROPERTY(_inc_DIRS INCLUDE_DIRECTORIES)

   FOREACH(_current ${_inc_DIRS})
      SET(${_MAKETA_INC_DIRS} ${${_MAKETA_INC_DIRS}} "-I${_current}")
   ENDFOREACH(_current ${_inc_DIRS})
ENDMACRO(MAKETA_GET_INC_DIRS)

# this is critical for allowing dependencies to work out, and for compiling w/out extra load
MACRO (SET_TA_PROPS ta_name path)
  SET_SOURCE_FILES_PROPERTIES(${path}/${ta_name}_TA.cpp
    PROPERTIES COMPILE_FLAGS "-O0 -g0" GENERATED 1    
  )
ENDMACRO(SET_TA_PROPS)

MACRO (CREATE_MAKETA_COMMAND ta_name path maketa_headers)
  MAKETA_GET_INC_DIRS(maketa_includes)
 
  SET(pta ${path}/${ta_name})
  
  ADD_CUSTOM_COMMAND(
    OUTPUT ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
    WORKING_DIRECTORY ${path}
    COMMAND maketa ${MAKETA_FLAGS} ${maketa_includes} ${ta_name} ${maketa_headers}
    DEPENDS ${maketa_headers}
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${pta}_TA.cpp ${pta}_TA_type.h ${pta}_TA_inst.h 
    WORKING_DIRECTORY ${path}
    COMMAND cmp ${pta}_TA.ccx  ${pta}_TA.cpp || cp  ${pta}_TA.ccx  ${pta}_TA.cpp\; cmp ${pta}_TA_type.hx  ${pta}_TA_type.h || cp  ${pta}_TA_type.hx  ${pta}_TA_type.h\; cmp ${pta}_TA_inst.hx  ${pta}_TA_inst.h || cp  ${pta}_TA_inst.hx  ${pta}_TA_inst.h
    DEPENDS ${pta}_TA.ccx ${pta}_TA_type.hx ${pta}_TA_inst.hx
  )
#   ADD_CUSTOM_TARGET(force_ta maketa ${maketa_flags} -autohx ${maketa_includes} ${PROJECT_NAME} ${MAKETA_HEADERS}
# #     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#   )
  SET_TA_PROPS(${ta_name} ${path})
ENDMACRO (CREATE_MAKETA_COMMAND)

