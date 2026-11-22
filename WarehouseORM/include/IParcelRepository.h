#pragma once
#include <optional>
#include <string>
#include <vector>
#include "Parcel.h"
#include "ParcelStatusHistory.h"

class IParcelRepository
{
public:
    virtual ~IParcelRepository() = default;

    virtual int add(const Parcel& parcel) = 0;
    virtual std::optional<Parcel> findById(int id) = 0;
    virtual std::vector<Parcel> findAll() = 0;
    virtual std::vector<Parcel> findByDirectionId(int directionId) = 0;
    virtual bool update(const Parcel& parcel) = 0;
    virtual bool remove(int id) = 0;

    // Атомарне оновлення статусу посилки та збереження запису в історію (транзакція SQLite)
    virtual void updateStatusWithHistory(
        Parcel& parcel,
        const std::string& oldStatus,
        const std::string& newStatus,
        const std::string& changedAt,
        bool simulateHistoryError = false
    ) = 0;

    virtual std::vector<ParcelStatusHistory> getHistoryForParcel(int parcelId) = 0;
};
