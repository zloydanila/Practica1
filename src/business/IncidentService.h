#pragma once
#include <string>
#include <cstdint>
#include <optional>
#include <vector>
#include "data/IncidentRepository.h"

namespace business {

class IncidentService {
public:
    explicit IncidentService(data::IncidentRepository& repo);

    int64_t createIncident(std::string title, std::string location, int severity);
    std::vector<domain::Incident> listIncidents() const;
    bool assignIncident(int64_t id, std::string assignee);
    bool addAction(int64_t id, std::string description);
    bool markActionDone(int64_t incidentId, int64_t actionId);
    bool closeIncident(int64_t id);   

private:
    data::IncidentRepository& repo_;
    int64_t nextActionId_{1};
};

}