#pragma once
#include <memory>
#include <string>
#include "Parcel.h"
#include "IParcelState.h"

// Контекст патерну State: інкапсулює поточний стан посилки та керує переходами
class ParcelContext
{
public:
    explicit ParcelContext(Parcel parcel);

    void ship();
    void deliver();
    void returnParcel();

    void transitionTo(std::unique_ptr<IParcelState> nextState);

    const Parcel& getParcel() const { return parcel_; }
    Parcel& getParcel() { return parcel_; }
    std::string currentStateName() const;
    const std::string& lastPreviousStatus() const { return lastPreviousStatus_; }

private:
    Parcel parcel_;
    std::unique_ptr<IParcelState> state_;
    std::string lastPreviousStatus_;
};
