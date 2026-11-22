cmake_minimum_required(VERSION 3.21)
project(WarehouseORM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(WarehouseORM
    src/main.cpp
    src/SQLiteDeliveryDirectionRepository.cpp
    src/SQLiteParcelRepository.cpp
    src/ParcelStates.cpp
    src/ParcelContext.cpp
    src/DeliveryTimeStrategies.cpp
    src/ParcelObservers.cpp
    src/WarehouseService.cpp
    src/Tests.cpp
)

target_include_directories(WarehouseORM
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(WarehouseORM
    PRIVATE
        sqlite_orm::sqlite_orm
        unofficial::sqlite3::sqlite3
)
