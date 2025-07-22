add_executable(logging-snapshots
    main.cpp
    )

target_link_libraries(logging-snapshots
    PRIVATE
    Qt5::Core
    zera-modulemanager-lib
    zera-modulemanager-testlib
)
