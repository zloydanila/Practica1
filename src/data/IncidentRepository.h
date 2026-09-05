#pragma once
#include <optional>
#include <vector>
#include <cstdint>
#include "domain/Incident.h"

namespace data {

class IncidentRepository {
public:
    virtual ~IncidentRepository() = default;

    virtual int64_t add(domain::Incident incident) = 0;
    virtual std::optional<domain::Incident> get(int64_t id) const = 0;
    virtual std::vector<domain::Incident> list() const = 0;
    virtual bool update(const domain::Incident& incident) = 0;
    virtual bool remove(int64_t id) = 0;  
};

}