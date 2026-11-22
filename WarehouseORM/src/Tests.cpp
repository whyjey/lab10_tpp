#include "Tests.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "DeliveryTimeStrategies.h"
#include "ParcelContext.h"
#include "ParcelObservers.h"
#include "ParcelStates.h"
#include "SQLiteDeliveryDirectionRepository.h"
#include "SQLiteParcelRepository.h"
#include "WarehouseService.h"

namespace
{
    void require(bool condition, const std::string& message)
    {
        if (!condition)
        {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }

    template <typename Func>
    void requireThrows(Func&& fn, const std::string& message)
    {
        bool thrown = false;
        try
        {
            fn();
        }
        catch (const std::exception&)
        {
            thrown = true;
        }
        if (!thrown)
        {
            throw std::runtime_error("Expected exception was not thrown: " + message);
        }
    }

    void testStateTransitionsAndFactory()
    {
        Parcel p{1, 1, "TRK-TEST-1", "Test Client", "Registered", 3.5};
        ParcelContext ctx(p);
        require(ctx.currentStateName() == "Registered", "Initial state must be Registered");

        // Недопустимий перехід Registered -> Delivered
        requireThrows([&]() { ctx.deliver(); }, "Cannot deliver directly from Registered");
        require(ctx.currentStateName() == "Registered", "State must remain Registered after invalid transition");

        // Допустимий перехід Registered -> Shipped
        ctx.ship();
        require(ctx.currentStateName() == "Shipped", "State must become Shipped");

        // Повторне відправлення заборонено
        requireThrows([&]() { ctx.ship(); }, "Cannot ship already Shipped parcel");

        // Допустимий перехід Shipped -> Delivered
        ctx.deliver();
        require(ctx.currentStateName() == "Delivered", "State must become Delivered");

        // Повернення вже доставленої посилки заборонено
        requireThrows([&]() { ctx.returnParcel(); }, "Cannot return Delivered parcel");

        // Окрема гілка: Registered -> Returned
        Parcel p2{2, 1, "TRK-TEST-2", "Test Client 2", "Registered", 1.2};
        ParcelContext ctx2(p2);
        ctx2.returnParcel();
        require(ctx2.currentStateName() == "Returned", "State must become Returned");
        requireThrows([&]() { ctx2.ship(); }, "Cannot ship Returned parcel");
    }

    void testStatePersistenceAndAtomicRollback()
    {
        const std::string dbFile = "test_lab10_state.db";
        std::filesystem::remove(dbFile);

        SQLiteDeliveryDirectionRepository dirRepo(dbFile);
        SQLiteParcelRepository parcelRepo(dbFile);
        WarehouseService service(dirRepo, parcelRepo);

        int dirId = dirRepo.add({0, "TST-01", "Kyiv", "active"});
        int parcelId = service.registerParcel(dirId, "TRK-DB-1", "Olena", 4.0);

        service.shipParcel(parcelId, "2026-11-22 10:00:00");
        auto afterShip = parcelRepo.findById(parcelId);
        require(afterShip && afterShip->status == "Shipped", "DB status must be Shipped");

        auto history1 = service.getParcelHistory(parcelId);
        require(history1.size() == 1, "History must contain 1 transition record");
        require(history1[0].oldStatus == "Registered" && history1[0].newStatus == "Shipped", "History entry mismatch");

        // Перевірка атомарного ROLLBACK при помилці запису історії
        requireThrows([&]()
        {
            service.returnParcel(parcelId, "2026-11-22 11:00:00", true);
        }, "Simulated DB history error must trigger rollback");

        auto afterRollback = parcelRepo.findById(parcelId);
        require(afterRollback && afterRollback->status == "Shipped", "Status must roll back to Shipped");
        require(service.getParcelHistory(parcelId).size() == 1, "History count must remain 1 after rollback");
    }

    void testDeliveryTimeStrategies()
    {
        DeliveryEstimateInput input{480.0, 60.0, 3.0}; // 480 км, 60 км/год -> 8 год в дорозі + 3 год обробка

        DeliveryEstimator estimator(std::make_unique<DistanceTimeStrategy>());
        double t1 = estimator.calculate(input);
        require(std::abs(t1 - 8.0) < 1e-6, "DistanceOnly must equal 8.0 hours");

        estimator.setStrategy(std::make_unique<TransportPlusProcessingStrategy>());
        double t2 = estimator.calculate(input);
        require(std::abs(t2 - 11.0) < 1e-6, "TransportPlusProcessing must equal 11.0 hours");

        estimator.setStrategy(std::make_unique<WeightedDeliveryTimeStrategy>(1.25, 0.5));
        double t3 = estimator.calculate(input); // 1.25 * 8.0 + 0.5 * 3.0 = 11.5
        require(std::abs(t3 - 11.5) < 1e-6, "Weighted strategy must equal 11.5 hours");
    }

    void testObserverSubscribeAndUnsubscribe()
    {
        const std::string dbFile = "test_lab10_observer.db";
        std::filesystem::remove(dbFile);

        SQLiteDeliveryDirectionRepository dirRepo(dbFile);
        SQLiteParcelRepository parcelRepo(dbFile);
        WarehouseService service(dirRepo, parcelRepo);

        auto logger = std::make_shared<DeliveryLogger>(false);
        auto stats = std::make_shared<DirectionStatistics>();

        service.subscribe(logger);
        service.subscribe(stats);

        int dirId = dirRepo.add({0, "OBS-01", "Lviv", "active"});
        int pId = service.registerParcel(dirId, "TRK-OBS-1", "Taras", 5.5);
        service.shipParcel(pId);

        require(logger->entries().size() == 2, "Logger must receive 2 events");
        auto st1 = stats->getStatsForDirection(dirId);
        require(st1.registeredCount == 1 && st1.shippedCount == 1, "Stats must reflect 1 registered and 1 shipped");

        // Відписуємо stats, залишаємо тільки logger
        service.unsubscribe(stats);
        service.deliverParcel(pId);

        require(logger->entries().size() == 3, "Logger must receive 3rd event");
        auto st2 = stats->getStatsForDirection(dirId);
        require(st2.deliveredCount == 0, "Unsubscribed stats must not receive Delivered event");
    }
}

void runAllUnitTests()
{
    std::cout << "=== RUNNING UNIT TESTS ===\n";

    testStateTransitionsAndFactory();
    std::cout << "[PASS] testStateTransitionsAndFactory\n";

    testStatePersistenceAndAtomicRollback();
    std::cout << "[PASS] testStatePersistenceAndAtomicRollback\n";

    testDeliveryTimeStrategies();
    std::cout << "[PASS] testDeliveryTimeStrategies\n";

    testObserverSubscribeAndUnsubscribe();
    std::cout << "[PASS] testObserverSubscribeAndUnsubscribe\n";

    std::cout << "All 4 unit test suites passed successfully!\n\n";
}