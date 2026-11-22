#pragma once
#include <memory>
#include <string>
#include "IParcelState.h"

// 1. Зареєстрована -> допустимі переходи: Відправлена, Повернена
class RegisteredState : public IParcelState
{
public:
    void ship(ParcelContext& context) override;
    void deliver(ParcelContext& context) override;
    void returnParcel(ParcelContext& context) override;
    std::string name() const override { return "Registered"; }
};

// 2. Відправлена -> допустимі переходи: Доставлена, Повернена
class ShippedState : public IParcelState
{
public:
    void ship(ParcelContext& context) override;
    void deliver(ParcelContext& context) override;
    void returnParcel(ParcelContext& context) override;
    std::string name() const override { return "Shipped"; }
};

// 3. Доставлена -> фінальний успішний стан (подальші переходи заборонені)
class DeliveredState : public IParcelState
{
public:
    void ship(ParcelContext& context) override;
    void deliver(ParcelContext& context) override;
    void returnParcel(ParcelContext& context) override;
    std::string name() const override { return "Delivered"; }
};

// 4. Повернена -> окремий фінальний стан повернення
class ReturnedState : public IParcelState
{
public:
    void ship(ParcelContext& context) override;
    void deliver(ParcelContext& context) override;
    void returnParcel(ParcelContext& context) override;
    std::string name() const override { return "Returned"; }
};

// Додатковий патерн GoF: Factory Method (Фабричний метод)
// Відновлює об'єкт стану IParcelState за рядковим статусом із бази даних
class ParcelStateFactory
{
public:
    static std::unique_ptr<IParcelState> createFromStatus(const std::string& status);
};
