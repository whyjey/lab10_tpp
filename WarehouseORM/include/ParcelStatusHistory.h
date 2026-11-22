#pragma once
#include <string>

struct ParcelStatusHistory
{
    int id = 0;
    int parcelId = 0;         // Зовнішній ключ -> Parcel.id
    std::string oldStatus;    // Попередній стан
    std::string newStatus;    // Новий стан
    std::string changedAt;    // Мітка часу переходу (YYYY-MM-DD HH:MM:SS)
};
