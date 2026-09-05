#pragma once

#include "business/IncidentService.h"

namespace presentation {

class ConsoleMenu {
public:
    explicit ConsoleMenu(business::IncidentService& service);
    void run();

private:
    business::IncidentService& service_;
};

}
