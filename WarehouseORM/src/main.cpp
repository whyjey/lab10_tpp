#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "DeliveryTimeStrategies.h"
#include "ParcelObservers.h"
#include "SQLiteDeliveryDirectionRepository.h"
#include "SQLiteParcelRepository.h"
#include "Tests.h"
#include "WarehouseService.h"

namespace
{
    void runBehavioralPatternsDemo()
    {
        const std::string dbFile = "warehouse_lab10_demo.db";
        std::filesystem::remove(dbFile);

        SQLiteDeliveryDirectionRepository directionRepo(dbFile);
        SQLiteParcelRepository parcelRepo(dbFile);
        WarehouseService warehouseService(directionRepo, parcelRepo);

        // Підключення підписників (Observer)
        auto logger = std::make_shared<DeliveryLogger>(true);
        auto statistics = std::make_shared<DirectionStatistics>();
        warehouseService.subscribe(logger);
        warehouseService.subscribe(statistics);

        // Створення напрямків доставки (сутність 1 із ЛР №8)
        int kyivDirId = directionRepo.add({0, "KYV-01", "Kyiv", "active"});
        int lvivDirId = directionRepo.add({0, "LWO-02", "Lviv", "active"});

        std::cout << "=== 1. OBSERVER + STATE + FACTORY METHOD DEMO ===\n";
        std::cout << "\n--- Registering parcels (Event: ParcelRegistered) ---\n";
        int p1 = warehouseService.registerParcel(kyivDirId, "TRK-2501", "Olena Koval", 4.5);
        int p2 = warehouseService.registerParcel(lvivDirId, "TRK-2502", "Andrii Melnyk", 9.2);

        std::cout << "\n--- Valid transitions for TRK-2501: Registered -> Shipped -> Delivered ---\n";
        warehouseService.shipParcel(p1, "2026-11-22 09:15:00");
        warehouseService.deliverParcel(p1, "2026-11-22 15:40:00");

        std::cout << "\n--- Invalid transition test (Delivered -> Returned) ---\n";
        try
        {
            warehouseService.returnParcel(p1, "2026-11-22 16:00:00");
        }
        catch (const std::exception& ex)
        {
            std::cout << "  Blocked invalid transition: " << ex.what() << '\n';
        }

        std::cout << "\n--- Branch transition for TRK-2502: Registered -> Returned ---\n";
        warehouseService.returnParcel(p2, "2026-11-22 11:20:00");

        std::cout << "\n--- Unsubscribing DeliveryLogger before registering TRK-2503 ---\n";
        warehouseService.unsubscribe(logger);
        int p3 = warehouseService.registerParcel(kyivDirId, "TRK-2503", "Iryna Tkachenko", 2.8);
        warehouseService.shipParcel(p3, "2026-11-22 12:05:00");
        std::cout << "  (DeliveryLogger stayed silent; DirectionStatistics updated)\n";

        std::cout << "\n--- Status transition history from SQLite for TRK-2501 ---\n";
        for (const auto& h : warehouseService.getParcelHistory(p1))
        {
            std::cout << "  History #" << h.id << ": parcelId=" << h.parcelId
                      << " | " << h.oldStatus << " -> " << h.newStatus
                      << " at " << h.changedAt << '\n';
        }

        std::cout << "\n--- Accumulated DirectionStatistics ---\n";
        statistics->printSummary();

        std::cout << "\n=== 2. STRATEGY DEMO (Delivery Time Estimation) ===\n";
        DeliveryEstimateInput routeInput{540.0, 60.0, 2.5}; // 540 км, 60 км/год, 2.5 год обробки
        std::cout << "Input: distance=" << routeInput.distanceKm << " km, speed="
                  << routeInput.normativeSpeedKmh << " km/h, processing="
                  << routeInput.processingHours << " h\n\n";

        DeliveryEstimator estimator(std::make_unique<DistanceTimeStrategy>());
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  1) " << estimator.currentStrategyName()
                  << " -> " << estimator.calculate(routeInput) << " hours\n";

        estimator.setStrategy(std::make_unique<TransportPlusProcessingStrategy>());
        std::cout << "  2) " << estimator.currentStrategyName()
                  << " -> " << estimator.calculate(routeInput) << " hours\n";

        estimator.setStrategy(std::make_unique<WeightedDeliveryTimeStrategy>(1.15, 0.85));
        std::cout << "  3) " << estimator.currentStrategyName()
                  << " -> " << estimator.calculate(routeInput) << " hours\n";
    }
}

int main()
{
    try
    {
        runAllUnitTests();
        runBehavioralPatternsDemo();
        std::cout << "\nLab 10 (Variant 25) completed successfully.\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Application error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}