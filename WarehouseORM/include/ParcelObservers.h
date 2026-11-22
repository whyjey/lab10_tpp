#pragma once
#include <map>
#include <string>
#include <vector>
#include "IParcelObserver.h"

// Підписник 1: Журнал доставки (DeliveryLogger)
class DeliveryLogger : public IParcelObserver
{
public:
    explicit DeliveryLogger(bool echoToConsole = true);

    void onParcelEvent(const ParcelEvent& event) override;
    const std::vector<std::string>& entries() const { return entries_; }

private:
    bool echoToConsole_;
    std::vector<std::string> entries_;
};

// Підписник 2: Статистика напрямку (DirectionStatistics)
struct DirectionStatRecord
{
    int registeredCount = 0;
    int shippedCount = 0;
    int deliveredCount = 0;
    int returnedCount = 0;
    double totalRegisteredWeightKg = 0.0;
};

class DirectionStatistics : public IParcelObserver
{
public:
    void onParcelEvent(const ParcelEvent& event) override;

    DirectionStatRecord getStatsForDirection(int directionId) const;
    void printSummary() const;

private:
    std::map<int, DirectionStatRecord> statsByDirection_;
};
