#pragma once
#include "ParcelEvents.h"

// Інтерфейс підписника патерну Observer
class IParcelObserver
{
public:
    virtual ~IParcelObserver() = default;
    virtual void onParcelEvent(const ParcelEvent& event) = 0;
};
