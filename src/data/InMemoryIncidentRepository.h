#pragma once
#include "data/IncidentRepository.h"
#include <unordered_map>

namespace data {

class InMemoryIncidentRepository final : public IncidentRepository {
public:
    int64_t add(domain::Incident incident) override;
    std::optional<domain::Incident> get(int64_t id) const override;
    std::vector<domain::Incident> list() const override;
    bool update(const domain::Incident& incident) override;
    bool remove(int64_t id) override;

private:
    int64_t nextId_{1};
    std::unordered_map<int64_t, domain::Incident> items_;
};

}