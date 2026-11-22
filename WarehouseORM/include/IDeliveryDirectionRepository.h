#pragma once
#include <optional>
#include <vector>
#include "DeliveryDirection.h"

class IDeliveryDirectionRepository
{
public:
    virtual ~IDeliveryDirectionRepository() = default;

    virtual int add(const DeliveryDirection& direction) = 0;
    virtual std::optional<DeliveryDirection> findById(int id) = 0;
    virtual std::vector<DeliveryDirection> findAll() = 0;
    virtual bool update(const DeliveryDirection& direction) = 0;
    virtual bool remove(int id) = 0;
};
