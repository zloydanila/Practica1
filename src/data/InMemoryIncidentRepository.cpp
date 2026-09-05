#include "data/InMemoryIncidentRepository.h"

namespace data {

int64_t InMemoryIncidentRepository::add(domain::Incident incident) {
    incident.id = nextId_++;
    items_[incident.id] = std::move(incident);
    return nextId_ - 1;
}

std::optional<domain::Incident> InMemoryIncidentRepository::get(int64_t id) const {
    auto it = items_.find(id);
    if (it == items_.end()) return std::nullopt;
    return it->second;
}

std::vector<domain::Incident> InMemoryIncidentRepository::list() const {
    std::vector<domain::Incident> out;
    out.reserve(items_.size());
    for (const auto& [id, inc] : items_) out.push_back(inc);
    return out;
}

bool InMemoryIncidentRepository::update(const domain::Incident& incident) {
    auto it = items_.find(incident.id);
    if (it == items_.end()) return false;
    it->second = incident;
    return true;
}

bool InMemoryIncidentRepository::remove(int64_t id) {
    return items_.erase(id) > 0;
}

}