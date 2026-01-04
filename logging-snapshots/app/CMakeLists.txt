add_executable(logging-snapshots
    main.cpp
    )

target_link_libraries(logging-snapshots
    PRIVATE
    ${QTX}::Core
    zera-modulemanager-lib
    zera-modulemanager-testlib
)
