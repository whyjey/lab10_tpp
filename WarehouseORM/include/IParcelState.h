#pragma once

#include <string>

class ParcelContext;

class IParcelState
{
public:
    virtual ~IParcelState() = default;

    virtual std::string name() const = 0;
    virtual void ship(ParcelContext& context) = 0;
    virtual void deliver(ParcelContext& context) = 0;
    virtual void returnParcel(ParcelContext& context) = 0;
};