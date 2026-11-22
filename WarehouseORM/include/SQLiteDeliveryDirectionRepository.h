#pragma once
#include <string>
#include "IDeliveryDirectionRepository.h"
#include "Database.h"

class SQLiteDeliveryDirectionRepository : public IDeliveryDirectionRepository
{
public:
    explicit SQLiteDeliveryDirectionRepository(const std::string& databaseFile);

    int add(const DeliveryDirection& direction) override;
    std::optional<DeliveryDirection> findById(int id) override;
    std::vector<DeliveryDirection> findAll() override;
    bool update(const DeliveryDirection& direction) override;
    bool remove(int id) override;

private:
    using Storage = decltype(createStorage(""));
    Storage storage_;
};
