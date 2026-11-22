#pragma once
#include <string>

enum class ParcelEventType
{
    ParcelRegistered,   // Подія 1: реєстрація посилки
    ParcelStatusChanged // Подія 2: зміна її стану
};

struct ParcelEvent
{
    ParcelEventType type;
    int parcelId = 0;
    int directionId = 0;
    std::string trackingCode;
    std::string oldStatus;
    std::string newStatus;
    double weightKg = 0.0;
};
