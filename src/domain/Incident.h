#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace domain {

enum class IncidentStatus { 
    New, InProgress, Closed
};

struct CorrectiveAction {
    int64_t id{};
    std::string description;
    bool done{false};
};

struct Incident {
    int64_t id{};
    std::string title;
    std::string location;
    int severity{1};
    IncidentStatus status{IncidentStatus::New};
    std::string assignedTo;
    std::vector<CorrectiveAction> actions;
};

}