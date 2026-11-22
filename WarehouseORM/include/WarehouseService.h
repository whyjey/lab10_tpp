#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IDeliveryDirectionRepository.h"
#include "IParcelObserver.h"
#include "IParcelRepository.h"

// Сервісний шар (Subject у патерні Observer + координатор State та Repository)
class WarehouseService
{
public:
    WarehouseService(
        IDeliveryDirectionRepository& directionRepo,
        IParcelRepository& parcelRepo
    );

    // Керування підписниками (Observer)
    void subscribe(const std::shared_ptr<IParcelObserver>& observer);
    void unsubscribe(const std::shared_ptr<IParcelObserver>& observer);

    // Реєстрація посилки (стан Registered + збереження в БД + сповіщення ParcelRegistered)
    int registerParcel(int directionId, const std::string& trackingCode, const std::string& recipient, double weightKg);

    // Керування переходами станів (State + атомарна транзакція в БД + сповіщення ParcelStatusChanged)
    void shipParcel(int parcelId, const std::string& timestamp = "2026-11-22 10:00:00");
    void deliverParcel(int parcelId, const std::string& timestamp = "2026-11-22 14:30:00");
    void returnParcel(int parcelId, const std::string& timestamp = "2026-11-22 16:00:00", bool simulateDbError = false);

    std::vector<ParcelStatusHistory> getParcelHistory(int parcelId);

private:
    enum class TransitionAction
    {
        Ship,
        Deliver,
        Return
    };

    void executeTransition(int parcelId, TransitionAction action, const std::string& timestamp, bool simulateDbError = false);
    void notifyObservers(const ParcelEvent& event);

    IDeliveryDirectionRepository& directionRepo_;
    IParcelRepository& parcelRepo_;
    std::vector<std::weak_ptr<IParcelObserver>> observers_;
};
