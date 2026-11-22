#include "TestData.h"
#include "Database.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

void prepareTestData(const std::string& databaseFile)
{
    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    storage.on_open = [](sqlite3* db)
    {
        if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error("Cannot enable foreign keys");
        }
    };
    storage.open_forever();
    storage.sync_schema();

    std::vector<DeliveryDirection> directions = {
        {0, "KYV-01", "Kyiv",    "active"},
        {0, "LWO-02", "Lviv",    "active"},
        {0, "ODS-03", "Odesa",   "paused"},
        {0, "KNP-04", "Dnipro",  "active"},
        {0, "HRK-05", "Kharkiv", "closed"}
    };

    std::vector<int> directionIds;
    directionIds.reserve(directions.size());
    for (const auto& dir : directions)
    {
        directionIds.push_back(storage.insert(dir));
    }

    std::vector<Parcel> parcels = {
        {0, directionIds[0], "TRK-1001", "Olena Koval",          "delivered",  2.5},
        {0, directionIds[0], "TRK-1002", "Maksym Bondar",        "in_transit", 12.0},
        {0, directionIds[0], "TRK-1003", "Iryna Tkachenko",      "delivered",  4.8},
        {0, directionIds[1], "TRK-2001", "Andrii Melnyk",        "in_transit", 8.4},
        {0, directionIds[1], "TRK-2002", "Sofiia Shevchuk",      "delivered",  1.2},
        {0, directionIds[1], "TRK-2003", "Bohdan Krawchuk",      "delayed",    15.5},
        {0, directionIds[2], "TRK-3001", "Daria Moroz",          "delayed",    6.0},
        {0, directionIds[2], "TRK-3002", "Yaroslav Savchuk",     "in_transit", 3.3},
        {0, directionIds[2], "TRK-3003", "Kateryna Lysenko",     "delivered",  9.7},
        {0, directionIds[3], "TRK-4001", "Taras Pavlenko",       "delivered",  5.1},
        {0, directionIds[3], "TRK-4002", "Viktoriia Polishchuk", "in_transit", 18.2},
        {0, directionIds[3], "TRK-4003", "Denys Marchenko",      "delivered",  2.9},
        {0, directionIds[4], "TRK-5001", "Anastasiia Rudenko",   "delayed",    7.4},
        {0, directionIds[4], "TRK-5002", "Roman Kravets",        "delivered",  11.0},
        {0, directionIds[4], "TRK-5003", "Yuliia Honchar",       "in_transit", 4.2}
    };

    for (const auto& parcel : parcels)
    {
        storage.insert(parcel);
    }

    std::cout << "Test database '" << databaseFile << "' prepared.\n";
    std::cout << "Directions count: " << storage.count<DeliveryDirection>() << '\n';
    std::cout << "Parcels count: " << storage.count<Parcel>() << '\n';
}
