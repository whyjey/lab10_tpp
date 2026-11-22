#include "ParcelContext.h"
#include "ParcelStates.h"
#include <utility>

ParcelContext::ParcelContext(Parcel parcel)
    : parcel_(std::move(parcel)),
      state_(ParcelStateFactory::createFromStatus(parcel_.status))
{
}

void ParcelContext::ship()
{
    state_->ship(*this);
}

void ParcelContext::deliver()
{
    state_->deliver(*this);
}

void ParcelContext::returnParcel()
{
    state_->returnParcel(*this);
}

void ParcelContext::transitionTo(std::unique_ptr<IParcelState> nextState)
{
    lastPreviousStatus_ = parcel_.status;
    state_ = std::move(nextState);
    parcel_.status = state_->name();
}

std::string ParcelContext::currentStateName() const
{
    return state_->name();
}
