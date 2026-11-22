#pragma once
#include <string>

struct DeliveryEstimateInput
{
    double distanceKm = 0.0;        // Відстань доставки (км)
    double normativeSpeedKmh = 0.0; // Нормативна швидкість доставки (км/год)
    double processingHours = 0.0;   // Час складської обробки (год)
};

// Інтерфейс патерну Strategy для оцінки терміну доставки (Варіант 25)
class IDeliveryTimeStrategy
{
public:
    virtual ~IDeliveryTimeStrategy() = default;

    virtual double estimateHours(const DeliveryEstimateInput& input) const = 0;
    virtual std::string name() const = 0;
};
