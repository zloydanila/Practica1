#include "data/InMemoryIncidentRepository.h"
#include "data/MySQLIncidentRepository.h"
#include "business/IncidentService.h"
#include "presentation/ConsoleMenu.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 6 && std::string(argv[1]) == "mysql") {
        try {
            data::MySQLIncidentRepository repo(argv[2], argv[3], argv[4], argv[5]);
            business::IncidentService service(repo);
            presentation::ConsoleMenu menu(service);
            menu.run();
        } catch (const std::exception& e) {
            std::cerr << "MySQL error: " << e.what() << "\n";
            return 1;
        }
    } else {
        data::InMemoryIncidentRepository repo;
        business::IncidentService service(repo);
        presentation::ConsoleMenu menu(service);
        menu.run();
    }
    return 0;
}