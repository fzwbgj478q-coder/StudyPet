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
    src/animation/AnimationPlayer.cpp
    src/animation/AnimationPlayer.h
    src/core/PetController.cpp
    src/core/PetController.h
    src/core/PetStateMachine.cpp
    src/core/PetStateMachine.h
    src/core/AppSettings.h
    src/core/PetStats.cpp
    src/core/PetStats.h
    src/service/ReminderService.cpp
    src/service/ReminderService.h
    src/service/StorageService.cpp
    src/service/StorageService.h
    src/service/StudyService.cpp
    src/service/StudyService.h
    src/system/AutoStartManager.cpp
    src/system/AutoStartManager.h
    src/window/PetWindow.cpp
    src/window/PetWindow.h
    src/window/SettingsWindow.cpp
    src/window/SettingsWindow.h
    src/window/StudyWindow.cpp
    src/window/StudyWindow.h
    resources/resources.qrc
)

target_link_libraries(StudyPet PRIVATE Qt6::Widgets)
target_include_directories(StudyPet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(StudyPet PRIVATE STUDYPET_VERSION=\"${PROJECT_VERSION}\")

set_target_properties(StudyPet PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(CTest)

if(BUILD_TESTING)
    find_package(Qt6 6.4 REQUIRED COMPONENTS Test)

    qt_add_executable(StudyPetTests
        tests/TestStage2.cpp
        src/animation/AnimationPlayer.cpp
        src/animation/AnimationPlayer.h
        src/core/PetStateMachine.cpp
        src/core/PetStateMachine.h
        src/core/AppSettings.h
        src/core/PetStats.cpp
        src/core/PetStats.h
        src/service/StorageService.cpp
        src/service/StorageService.h
        src/system/AutoStartManager.cpp
        src/system/AutoStartManager.h
        src/system/AutoStartManager.cpp
        src/system/AutoStartManager.h
        src/service/StudyService.cpp
        src/service/StudyService.h
        resources/resources.qrc
    )
    target_link_libraries(StudyPetTests PRIVATE Qt6::Test Qt6::Widgets)
    target_include_directories(StudyPetTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_test(NAME StudyPetTests COMMAND StudyPetTests)

    qt_add_executable(StudyPetStage3Tests
        tests/TestStage3.cpp
        src/core/PetStats.cpp
        src/core/PetStats.h
        src/service/StudyService.cpp
        src/service/StudyService.h
        src/service/StorageService.cpp
        src/service/StorageService.h
        src/system/AutoStartManager.cpp
        src/system/AutoStartManager.h
    )
    target_link_libraries(StudyPetStage3Tests PRIVATE Qt6::Test Qt6::Core)
    target_include_directories(StudyPetStage3Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_test(NAME StudyPetStage3Tests COMMAND StudyPetStage3Tests)
endif()

install(TARGETS StudyPet RUNTIME DESTINATION bin)

