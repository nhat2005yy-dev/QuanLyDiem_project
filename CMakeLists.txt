cmake_minimum_required(VERSION 3.16)

project(QuanLyDiem_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QuanLyDiem_project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        danhsachmonhoc.h danhsachmonhoc.cpp

        cautrucdulieu.h cautrucdulieu.cpp




        danhsachloptinchi.h danhsachloptinchi.cpp

        quanlyloptinchi.h quanlyloptinchi.cpp quanlyloptinchi.ui
        themloptinchi.h themloptinchi.cpp themloptinchi.ui


        danhsachdangky.h danhsachdangky.cpp danhsachdangky.ui



        DS_Lop.h DS_Lop.cpp DS_Lop.ui
        DS_SinhVien.h DS_SinhVien.cpp DS_SinhVien.ui
        danhsachmonhoc.h danhsachmonhoc.cpp danhsachmonhoc.ui
        indssvloptinchi.h indssvloptinchi.cpp indssvloptinchi.ui
        diem_input.cpp diem_input.h diem_input.ui diem_monhoc.cpp diem_monhoc.h diem_monhoc.ui diem_tongket.cpp diem_tongket.h diem_tongket.ui diem_trungbinh.cpp diem_trungbinh.h diem_trungbinh.ui quanlydiem.cpp quanlydiem.h quanlydiem.ui






    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QuanLyDiem_project APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(QuanLyDiem_project SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QuanLyDiem_project
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(QuanLyDiem_project PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QuanLyDiem_project)
endif()
set_target_properties(QuanLyDiem_project PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QuanLyDiem_project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QuanLyDiem_project)
endif()
