#include "ParcelObservers.h"
#include <iostream>
#include <sstream>

DeliveryLogger::DeliveryLogger(bool echoToConsole)
    : echoToConsole_(echoToConsole)
{
}

void DeliveryLogger::onParcelEvent(const ParcelEvent& event)
{
    std::ostringstream oss;
    if (event.type == ParcelEventType::ParcelRegistered)
    {
        oss << "[LOG][ParcelRegistered] ID=" << event.parcelId
            << " (" << event.trackingCode << "), directionId=" << event.directionId
            << ", status=" << event.newStatus << ", weight=" << event.weightKg << " kg";
    }
    else
    {
        oss << "[LOG][ParcelStatusChanged] ID=" << event.parcelId
            << " (" << event.trackingCode << "): "
            << event.oldStatus << " -> " << event.newStatus;
    }

    std::string message = oss.str();
    entries_.push_back(message);

    if (echoToConsole_)
    {
        std::cout << "  " << message << '\n';
    }
}

void DirectionStatistics::onParcelEvent(const ParcelEvent& event)
{
    auto& rec = statsByDirection_[event.directionId];

    if (event.type == ParcelEventType::ParcelRegistered)
    {
        rec.registeredCount++;
        rec.totalRegisteredWeightKg += event.weightKg;
    }
    else if (event.type == ParcelEventType::ParcelStatusChanged)
    {
        if (event.newStatus == "Shipped")
        {
            rec.shippedCount++;
        }
        else if (event.newStatus == "Delivered")
        {
            rec.deliveredCount++;
        }
        else if (event.newStatus == "Returned")
        {
            rec.returnedCount++;
        }
    }
}

DirectionStatRecord DirectionStatistics::getStatsForDirection(int directionId) const
{
    auto it = statsByDirection_.find(directionId);
    if (it == statsByDirection_.end())
    {
        return {};
    }
    return it->second;
}

void DirectionStatistics::printSummary() const
{
    std::cout << "  [DirectionStatistics Summary]\n";
    for (const auto& [dirId, st] : statsByDirection_)
    {
        std::cout << "    directionId=" << dirId
                  << " | registered=" << st.registeredCount
                  << ", shipped=" << st.shippedCount
                  << ", delivered=" << st.deliveredCount
                  << ", returned=" << st.returnedCount
                  << ", totalWeight=" << st.totalRegisteredWeightKg << " kg\n";
    }
}