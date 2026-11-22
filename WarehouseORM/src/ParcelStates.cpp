#include "ParcelStates.h"
#include "ParcelContext.h"
#include <stdexcept>

// --- RegisteredState ---
void RegisteredState::ship(ParcelContext& context)
{
    context.transitionTo(std::make_unique<ShippedState>());
}

void RegisteredState::deliver(ParcelContext&)
{
    throw std::logic_error("Invalid transition: cannot deliver a parcel that is still Registered (must be Shipped first)");
}

void RegisteredState::returnParcel(ParcelContext& context)
{
    context.transitionTo(std::make_unique<ReturnedState>());
}

// --- ShippedState ---
void ShippedState::ship(ParcelContext&)
{
    throw std::logic_error("Invalid transition: parcel is already Shipped");
}

void ShippedState::deliver(ParcelContext& context)
{
    context.transitionTo(std::make_unique<DeliveredState>());
}

void ShippedState::returnParcel(ParcelContext& context)
{
    context.transitionTo(std::make_unique<ReturnedState>());
}

// --- DeliveredState ---
void DeliveredState::ship(ParcelContext&)
{
    throw std::logic_error("Invalid transition: parcel is already Delivered");
}

void DeliveredState::deliver(ParcelContext&)
{
    throw std::logic_error("Invalid transition: parcel is already Delivered");
}

void DeliveredState::returnParcel(ParcelContext&)
{
    throw std::logic_error("Invalid transition: cannot return an already Delivered parcel");
}

// --- ReturnedState ---
void ReturnedState::ship(ParcelContext&)
{
    throw std::logic_error("Invalid transition: parcel has been Returned");
}

void ReturnedState::deliver(ParcelContext&)
{
    throw std::logic_error("Invalid transition: parcel has been Returned");
}

void ReturnedState::returnParcel(ParcelContext&)
{
    throw std::logic_error("Invalid transition: parcel is already Returned");
}

// --- Factory Method ---
std::unique_ptr<IParcelState> ParcelStateFactory::createFromStatus(const std::string& status)
{
    if (status == "Registered")
    {
        return std::make_unique<RegisteredState>();
    }
    if (status == "Shipped")
    {
        return std::make_unique<ShippedState>();
    }
    if (status == "Delivered")
    {
        return std::make_unique<DeliveredState>();
    }
    if (status == "Returned")
    {
        return std::make_unique<ReturnedState>();
    }
    throw std::invalid_argument("Unknown parcel status: " + status);
}
