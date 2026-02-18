cmake_minimum_required(VERSION 3.16)

project(my_chess_game VERSION 0.1 LANGUAGES CXX)
if(CMAKE_BUILD_TYPE MATCHES "Release")
    add_compile_options(-O3 -march=native)
endif()
set(BOOST_ROOT "C:/Libraries/boost_1_89_0")
include_directories(${BOOST_ROOT})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# logic sources for tests and for the game
set(LOGIC_SOURCES
    config.cpp config.h
    rook.cpp rook.h
    pawn.cpp pawn.h
    bishop.cpp bishop.h
    queen.cpp queen.h
    knight.cpp knight.h
    king.cpp king.h
    board.cpp board.h
)

# GUI sources only for the game
set(GUI_SOURCES
    main.cpp
    main_window.cpp main_window.h main_window.ui
    resources.qrc
)

# creation a static logic library
add_library(ChessLogic STATIC ${LOGIC_SOURCES})

# link Qt to logic
target_link_libraries(ChessLogic PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(ChessLogic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(my_chess_game MANUAL_FINALIZATION ${GUI_SOURCES})
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET my_chess_game APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    if(ANDROID)
        add_library(my_chess_game SHARED ${GUI_SOURCES})
    else()
        add_executable(my_chess_game ${GUI_SOURCES})
    endif()
endif()

target_link_libraries(my_chess_game PRIVATE ChessLogic Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.my_chess_game)
endif()
set_target_properties(my_chess_game PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

enable_testing()

add_executable(chess_tests tests/test_main.cpp)

target_link_libraries(chess_tests PRIVATE ChessLogic GTest::gtest_main Qt${QT_VERSION_MAJOR}::Widgets)

include(GoogleTest)
gtest_discover_tests(chess_tests DISCOVERY_MODE PRE_TEST)

include(GNUInstallDirs)
install(TARGETS my_chess_game
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(my_chess_game)
endif()
