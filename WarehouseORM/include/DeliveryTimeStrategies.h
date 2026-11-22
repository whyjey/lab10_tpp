#pragma once
#include <memory>
#include <string>
#include "IDeliveryTimeStrategy.h"

// Алгоритм 1: розрахунок часу виключно за відстанню (T = d / v)
class DistanceTimeStrategy : public IDeliveryTimeStrategy
{
public:
    double estimateHours(const DeliveryEstimateInput& input) const override;
    std::string name() const override { return "DistanceOnly (d / v)"; }
};

// Алгоритм 2: сума часу транспортування та складської обробки (T = d / v + t_proc)
class TransportPlusProcessingStrategy : public IDeliveryTimeStrategy
{
public:
    double estimateHours(const DeliveryEstimateInput& input) const override;
    std::string name() const override { return "TransportPlusProcessing (d / v + t_proc)"; }
};

// Алгоритм 3: зважений розрахунок часу з урахуванням вагових коефіцієнтів
class WeightedDeliveryTimeStrategy : public IDeliveryTimeStrategy
{
public:
    WeightedDeliveryTimeStrategy(double transportWeight = 1.15, double processingWeight = 0.85);

    double estimateHours(const DeliveryEstimateInput& input) const override;
    std::string name() const override { return "WeightedTime (w1 * d / v + w2 * t_proc)"; }

private:
    double transportWeight_;
    double processingWeight_;
};

// Контекст патерну Strategy
class DeliveryEstimator
{
public:
    explicit DeliveryEstimator(std::unique_ptr<IDeliveryTimeStrategy> strategy);

    void setStrategy(std::unique_ptr<IDeliveryTimeStrategy> newStrategy);
    double calculate(const DeliveryEstimateInput& input) const;
    std::string currentStrategyName() const;

private:
    std::unique_ptr<IDeliveryTimeStrategy> strategy_;
};
