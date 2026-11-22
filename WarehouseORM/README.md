# Лабораторна робота №10 — Поведінкові патерни проєктування (Варіант 25)

**Предметна область:** Система обліку складських посилок (розширення проєкту з ЛР №8)
- **Основна сутність (1):** Напрямок доставки (`DeliveryDirection`)
- **Залежна сутність (N):** Посилка (`Parcel`) + історія переходів станів (`ParcelStatusHistory`)

## Реалізовані патерни GoF
1. **State (`IParcelState`):** життєвий цикл посилки `Registered` (`Зареєстрована`) -> `Shipped` (`Відправлена`) -> `Delivered` (`Доставлена`), а також окремий стан `Returned` (`Повернена`). Переходи та запис історії в БД виконуються атомарно в транзакції SQLite.
2. **Strategy (`IDeliveryTimeStrategy`):** три взаємозамінні алгоритми оцінки терміну доставки за параметрами `(distanceKm, normativeSpeedKmh, processingHours)`:
   - `DistanceTimeStrategy` — розрахунок часу за відстанню ($T = d / v$);
   - `TransportPlusProcessingStrategy` — сума часу транспортування та обробки ($T = d / v + t_{proc}$);
   - `WeightedDeliveryTimeStrategy` — зважений розрахунок часу з урахуванням вагових коефіцієнтів.
3. **Observer (`IParcelObserver`):** сповіщення підписників про події `ParcelRegistered` та `ParcelStatusChanged` із підтримкою динамічної підписки/відписки (`std::weak_ptr`). Реалізовано два незалежні підписники: `DeliveryLogger` (журнал доставки) та `DirectionStatistics` (статистика напрямку).
4. **Factory Method (`ParcelStateFactory` — додатковий патерн GoF):** фабричний метод створення об'єкта стану `std::unique_ptr<IParcelState>` на основі рядкового представлення статусу з бази даних.

## Збирання та запуск
```powershell
cmake --preset default
cmake --build --preset default
.\build\WarehouseORM.exe
```
