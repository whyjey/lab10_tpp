#pragma once
#include <string>

struct DeliveryDirection
{
    int id = 0;
    std::string code;    // Код напрямку, наприклад "KYV-01"
    std::string region;  // Регіон доставки, наприклад "Kyiv"
    std::string status;  // Статус напрямку: "active", "paused", "closed"
};
