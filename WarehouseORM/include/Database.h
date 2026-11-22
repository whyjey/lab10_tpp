#pragma once
#include <string>
#include <sqlite_orm/sqlite_orm.h>
#include "DeliveryDirection.h"
#include "Parcel.h"
#include "ParcelStatusHistory.h"

inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;

    return make_storage(
        filename,
        make_table(
            "DeliveryDirections",
            make_column("id", &DeliveryDirection::id, primary_key().autoincrement()),
            make_column("code", &DeliveryDirection::code, not_null()),
            make_column("region", &DeliveryDirection::region, not_null()),
            make_column("status", &DeliveryDirection::status, not_null())
        ),
        make_table(
            "Parcels",
            make_column("id", &Parcel::id, primary_key().autoincrement()),
            make_column("directionId", &Parcel::directionId, not_null()),
            make_column("trackingCode", &Parcel::trackingCode, not_null()),
            make_column("recipient", &Parcel::recipient, not_null()),
            make_column("status", &Parcel::status, not_null()),
            make_column("weightKg", &Parcel::weightKg, not_null()),
            foreign_key(&Parcel::directionId).references(&DeliveryDirection::id)
        ),
        make_table(
            "ParcelStatusHistory",
            make_column("id", &ParcelStatusHistory::id, primary_key().autoincrement()),
            make_column("parcelId", &ParcelStatusHistory::parcelId, not_null()),
            make_column("oldStatus", &ParcelStatusHistory::oldStatus, not_null()),
            make_column("newStatus", &ParcelStatusHistory::newStatus, not_null()),
            make_column("changedAt", &ParcelStatusHistory::changedAt, not_null()),
            foreign_key(&ParcelStatusHistory::parcelId).references(&Parcel::id)
        )
    );
}
