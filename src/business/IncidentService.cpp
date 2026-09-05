#include "business/IncidentService.h"

namespace business {

IncidentService::IncidentService(data::IncidentRepository& repo) : repo_(repo) {}

int64_t IncidentService::createIncident(std::string title, std::string location, int severity) {
    if (severity < 1) severity = 1;
    if (severity > 5) severity = 5;

    domain::Incident inc;
    inc.title  = std::move(title);
    inc.location = std::move(location);
    inc.severity = severity;
    inc.status = domain::IncidentStatus::New;

    return repo_.add(std::move(inc));
}

std::vector<domain::Incident> IncidentService::listIncidents() const {
    return repo_.list();
}

bool IncidentService::assignIncident(int64_t id, std::string assignee) {
    auto incOpt = repo_.get(id);
    if (!incOpt) return false;

    auto inc = *incOpt;
    inc.assignedTo = std::move(assignee);
    if (inc.status == domain::IncidentStatus::New)
        inc.status = domain::IncidentStatus::InProgress;

    return repo_.update(inc);
}





bool IncidentService::addAction(int64_t id, std::string description) {
    auto incOpt = repo_.get(id);
    if (!incOpt) return false;

    auto inc = *incOpt;
    domain::CorrectiveAction a;
    a.id          = nextActionId_++;
    a.description = std::move(description);
    a.done        = false;

    inc.actions.push_back(std::move(a));
    return repo_.update(inc);
}

bool IncidentService::markActionDone(int64_t incidentId, int64_t actionId) {
    auto incOpt = repo_.get(incidentId);
    if (!incOpt) return false;

    auto inc = *incOpt;
    for (auto& a : inc.actions) {
        if (a.id == actionId) {
            a.done = true;
            return repo_.update(inc);
        }
    }
    return false;
}

bool IncidentService::closeIncident(int64_t id) {
    auto incOpt = repo_.get(id);
    if (!incOpt) return false;

    auto inc = *incOpt;
    for (const auto& a : inc.actions) {
        if (!a.done) return false;
    }

    inc.status = domain::IncidentStatus::Closed;
    return repo_.update(inc);
}

} // namespace business
