#include "DeliveryTimeStrategies.h"
#include <stdexcept>
#include <utility>

namespace
{
    void validateInput(const DeliveryEstimateInput& input)
    {
        if (input.distanceKm < 0.0 || input.processingHours < 0.0 || input.normativeSpeedKmh <= 0.0)
        {
            throw std::invalid_argument("Invalid delivery estimation parameters");
        }
    }
}

double DistanceTimeStrategy::estimateHours(const DeliveryEstimateInput& input) const
{
    validateInput(input);
    return input.distanceKm / input.normativeSpeedKmh;
}

double TransportPlusProcessingStrategy::estimateHours(const DeliveryEstimateInput& input) const
{
    validateInput(input);
    return (input.distanceKm / input.normativeSpeedKmh) + input.processingHours;
}

WeightedDeliveryTimeStrategy::WeightedDeliveryTimeStrategy(double transportWeight, double processingWeight)
    : transportWeight_(transportWeight),
      processingWeight_(processingWeight)
{
    if (transportWeight_ <= 0.0 || processingWeight_ < 0.0)
    {
        throw std::invalid_argument("Invalid strategy weights");
    }
}

double WeightedDeliveryTimeStrategy::estimateHours(const DeliveryEstimateInput& input) const
{
    validateInput(input);
    double transportHours = input.distanceKm / input.normativeSpeedKmh;
    return transportWeight_ * transportHours + processingWeight_ * input.processingHours;
}

DeliveryEstimator::DeliveryEstimator(std::unique_ptr<IDeliveryTimeStrategy> strategy)
    : strategy_(std::move(strategy))
{
    if (!strategy_)
    {
        throw std::invalid_argument("Strategy cannot be null");
    }
}

void DeliveryEstimator::setStrategy(std::unique_ptr<IDeliveryTimeStrategy> newStrategy)
{
    if (!newStrategy)
    {
        throw std::invalid_argument("New strategy cannot be null");
    }
    strategy_ = std::move(newStrategy);
}

double DeliveryEstimator::calculate(const DeliveryEstimateInput& input) const
{
    return strategy_->estimateHours(input);
}

std::string DeliveryEstimator::currentStrategyName() const
{
    return strategy_->name();
}
