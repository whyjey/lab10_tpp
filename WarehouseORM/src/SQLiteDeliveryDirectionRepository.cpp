#include "SQLiteDeliveryDirectionRepository.h"
#include <stdexcept>

SQLiteDeliveryDirectionRepository::SQLiteDeliveryDirectionRepository(const std::string& databaseFile)
    : storage_(createStorage(databaseFile))
{
    storage_.on_open = [](sqlite3* db)
    {
        if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error("Cannot enable foreign key constraints");
        }
    };
    storage_.open_forever();
    storage_.sync_schema();
}

int SQLiteDeliveryDirectionRepository::add(const DeliveryDirection& direction)
{
    return storage_.insert(direction);
}

std::optional<DeliveryDirection> SQLiteDeliveryDirectionRepository::findById(int id)
{
    return storage_.get_optional<DeliveryDirection>(id);
}

std::vector<DeliveryDirection> SQLiteDeliveryDirectionRepository::findAll()
{
    return storage_.get_all<DeliveryDirection>();
}

bool SQLiteDeliveryDirectionRepository::update(const DeliveryDirection& direction)
{
    auto existing = storage_.get_optional<DeliveryDirection>(direction.id);
    if (!existing)
    {
        return false;
    }
    storage_.update(direction);
    return true;
}

bool SQLiteDeliveryDirectionRepository::remove(int id)
{
    auto existing = storage_.get_optional<DeliveryDirection>(id);
    if (!existing)
    {
        return false;
    }
    storage_.remove<DeliveryDirection>(id);
    return true;
}
