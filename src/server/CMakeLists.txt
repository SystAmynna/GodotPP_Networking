add_executable(GodotPPServer
        src/main.cpp
        src/ecs_declaration.h
        src/ecs_implementation.cpp
)

target_link_libraries(GodotPPServer PRIVATE
        snl
        EnTT::EnTT)