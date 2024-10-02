# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt
function(SETUP_QTESTS_SCPIDOC)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            scpi-doc-lib
            zera-modulemanager-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_SCPIDOC (
    test_sessionnamesmappingjson
    )

