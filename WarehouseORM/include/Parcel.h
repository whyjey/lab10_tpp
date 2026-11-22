#pragma once
#include <string>

struct Parcel
{
    int id = 0;
    int directionId = 0;      // Зовнішній ключ -> DeliveryDirection.id
    std::string trackingCode; // Трек-номер посилки
    std::string recipient;    // Отримувач
    std::string status;       // Статус: "Registered", "Shipped", "Delivered", "Returned"
    double weightKg = 0.0;    // Вага у кг
};
