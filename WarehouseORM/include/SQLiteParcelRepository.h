#pragma once
#include <string>
#include "IParcelRepository.h"
#include "Database.h"

class SQLiteParcelRepository : public IParcelRepository
{
public:
    explicit SQLiteParcelRepository(const std::string& databaseFile);

    int add(const Parcel& parcel) override;
    std::optional<Parcel> findById(int id) override;
    std::vector<Parcel> findAll() override;
    std::vector<Parcel> findByDirectionId(int directionId) override;
    bool update(const Parcel& parcel) override;
    bool remove(int id) override;

    void updateStatusWithHistory(
        Parcel& parcel,
        const std::string& oldStatus,
        const std::string& newStatus,
        const std::string& changedAt,
        bool simulateHistoryError = false
    ) override;

    std::vector<ParcelStatusHistory> getHistoryForParcel(int parcelId) override;

private:
    using Storage = decltype(createStorage(""));
    Storage storage_;
};
