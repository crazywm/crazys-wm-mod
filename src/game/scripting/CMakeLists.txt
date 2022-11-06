set(SCRIPTING_SRC
        src/cEventMapping.cpp
        src/cLuaScript.cpp
        src/cLuaState.cpp
        src/cScriptManager.cpp
        src/sLuaGirl.cpp
        src/sLuaParameter.cpp
        src/sLuaEventResult.cpp
        )


add_library(WmScriptingObjects OBJECT ${SCRIPTING_SRC})
target_link_libraries(WmScriptingObjects PRIVATE WMEngineObjects lua::lua)
target_include_directories(WmScriptingObjects PUBLIC include)
target_include_directories(WmScriptingObjects PRIVATE include/scripting)
# TODO Fix includes!
target_include_directories(WmScriptingObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(WmScriptingObjects PROPERTIES CXX_STANDARD 14)

