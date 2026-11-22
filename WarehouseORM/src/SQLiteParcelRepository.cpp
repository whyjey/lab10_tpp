#include "SQLiteParcelRepository.h"
#include <stdexcept>

SQLiteParcelRepository::SQLiteParcelRepository(const std::string& databaseFile)
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

int SQLiteParcelRepository::add(const Parcel& parcel)
{
    return storage_.insert(parcel);
}

std::optional<Parcel> SQLiteParcelRepository::findById(int id)
{
    return storage_.get_optional<Parcel>(id);
}

std::vector<Parcel> SQLiteParcelRepository::findAll()
{
    return storage_.get_all<Parcel>();
}

std::vector<Parcel> SQLiteParcelRepository::findByDirectionId(int directionId)
{
    using namespace sqlite_orm;
    return storage_.get_all<Parcel>(where(c(&Parcel::directionId) == directionId));
}

bool SQLiteParcelRepository::update(const Parcel& parcel)
{
    auto existing = storage_.get_optional<Parcel>(parcel.id);
    if (!existing)
    {
        return false;
    }
    storage_.update(parcel);
    return true;
}

bool SQLiteParcelRepository::remove(int id)
{
    auto existing = storage_.get_optional<Parcel>(id);
    if (!existing)
    {
        return false;
    }
    storage_.remove<Parcel>(id);
    return true;
}

void SQLiteParcelRepository::updateStatusWithHistory(
    Parcel& parcel,
    const std::string& oldStatus,
    const std::string& newStatus,
    const std::string& changedAt,
    bool simulateHistoryError)
{
    storage_.transaction([&]()
    {
        auto existing = storage_.get_optional<Parcel>(parcel.id);
        if (!existing)
        {
            throw std::runtime_error("Parcel not found for status update");
        }

        parcel.status = newStatus;
        storage_.update(parcel);

        if (simulateHistoryError)
        {
            // Імітація збою під час запису в таблицю історії (порушення FK parcelId = -1)
            storage_.insert(ParcelStatusHistory{0, -1, oldStatus, newStatus, changedAt});
        }
        else
        {
            storage_.insert(ParcelStatusHistory{0, parcel.id, oldStatus, newStatus, changedAt});
        }
        return true;
    });
}

std::vector<ParcelStatusHistory> SQLiteParcelRepository::getHistoryForParcel(int parcelId)
{
    using namespace sqlite_orm;
    return storage_.get_all<ParcelStatusHistory>(
        where(c(&ParcelStatusHistory::parcelId) == parcelId),
        order_by(&ParcelStatusHistory::id).asc()
    );
}
