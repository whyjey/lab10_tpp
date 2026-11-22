#include "WarehouseService.h"
#include "ParcelContext.h"
#include <algorithm>
#include <stdexcept>

WarehouseService::WarehouseService(
    IDeliveryDirectionRepository& directionRepo,
    IParcelRepository& parcelRepo)
    : directionRepo_(directionRepo),
      parcelRepo_(parcelRepo)
{
}

void WarehouseService::subscribe(const std::shared_ptr<IParcelObserver>& observer)
{
    if (!observer)
    {
        return;
    }
    observers_.push_back(observer);
}

void WarehouseService::unsubscribe(const std::shared_ptr<IParcelObserver>& observer)
{
    std::erase_if(observers_, [&](const std::weak_ptr<IParcelObserver>& item)
    {
        auto locked = item.lock();
        return !locked || locked == observer;
    });
}

int WarehouseService::registerParcel(
    int directionId,
    const std::string& trackingCode,
    const std::string& recipient,
    double weightKg)
{
    if (!directionRepo_.findById(directionId))
    {
        throw std::invalid_argument("Delivery direction does not exist");
    }

    Parcel parcel{0, directionId, trackingCode, recipient, "Registered", weightKg};
    int newId = parcelRepo_.add(parcel);
    parcel.id = newId;

    notifyObservers(ParcelEvent{
        ParcelEventType::ParcelRegistered,
        parcel.id,
        parcel.directionId,
        parcel.trackingCode,
        "",
        parcel.status,
        parcel.weightKg
    });

    return newId;
}

void WarehouseService::shipParcel(int parcelId, const std::string& timestamp)
{
    executeTransition(parcelId, TransitionAction::Ship, timestamp, false);
}

void WarehouseService::deliverParcel(int parcelId, const std::string& timestamp)
{
    executeTransition(parcelId, TransitionAction::Deliver, timestamp, false);
}

void WarehouseService::returnParcel(int parcelId, const std::string& timestamp, bool simulateDbError)
{
    executeTransition(parcelId, TransitionAction::Return, timestamp, simulateDbError);
}

std::vector<ParcelStatusHistory> WarehouseService::getParcelHistory(int parcelId)
{
    return parcelRepo_.getHistoryForParcel(parcelId);
}

void WarehouseService::executeTransition(
    int parcelId,
    TransitionAction action,
    const std::string& timestamp,
    bool simulateDbError)
{
    auto existing = parcelRepo_.findById(parcelId);
    if (!existing)
    {
        throw std::runtime_error("Parcel not found");
    }

    // 1. Відновлюємо стан через Factory Method і виконуємо перехід через патерн State
    ParcelContext context(*existing);
    if (action == TransitionAction::Ship)
    {
        context.ship();
    }
    else if (action == TransitionAction::Deliver)
    {
        context.deliver();
    }
    else
    {
        context.returnParcel();
    }

    // 2. Атомарно оновлюємо статус посилки та записуємо подію в ParcelStatusHistory (транзакція SQLite)
    Parcel updatedParcel = context.getParcel();
    const std::string oldStatus = context.lastPreviousStatus();
    const std::string newStatus = context.currentStateName();

    parcelRepo_.updateStatusWithHistory(updatedParcel, oldStatus, newStatus, timestamp, simulateDbError);

    // 3. Сповіщаємо активних підписників (Observer)
    notifyObservers(ParcelEvent{
        ParcelEventType::ParcelStatusChanged,
        updatedParcel.id,
        updatedParcel.directionId,
        updatedParcel.trackingCode,
        oldStatus,
        newStatus,
        updatedParcel.weightKg
    });
}

void WarehouseService::notifyObservers(const ParcelEvent& event)
{
    std::erase_if(observers_, [](const std::weak_ptr<IParcelObserver>& item)
    {
        return item.expired();
    });

    for (const auto& weakObs : observers_)
    {
        if (auto obs = weakObs.lock())
        {
            obs->onParcelEvent(event);
        }
    }
}
