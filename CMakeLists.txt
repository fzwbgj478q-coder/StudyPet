cmake_minimum_required(VERSION 3.21)

project(StudyPet VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(StudyPet
    src/main.cpp
    src/core/PetController.cpp
    src/core/PetController.h
    src/window/PetWindow.cpp
    src/window/PetWindow.h
    resources/resources.qrc
)

target_link_libraries(StudyPet PRIVATE Qt6::Widgets)

target_compile_definitions(StudyPet PRIVATE
    QT_NO_KEYWORDS
    STUDYPET_VERSION=\"${PROJECT_VERSION}\"
)

set_target_properties(StudyPet PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(CTest)

install(TARGETS StudyPet RUNTIME DESTINATION bin)

