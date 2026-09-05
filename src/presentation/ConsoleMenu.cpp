#include "presentation/ConsoleMenu.h"

#include "domain/Incident.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

namespace {

void clearLine() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string statusToString(domain::IncidentStatus s) {
    switch (s) {
        case domain::IncidentStatus::New: return "New";
        case domain::IncidentStatus::InProgress: return "InProgress";
        case domain::IncidentStatus::Closed: return "Closed";
    }
    return "Unknown";
}

void printIncident(const domain::Incident& inc) {
    cout << "id=" << inc.id
         << " | title: " << inc.title
         << " | location: " << inc.location
         << " | severity=" << inc.severity
         << " | assignedTo=" << (inc.assignedTo.empty() ? "-" : inc.assignedTo)
         << " | status=" << statusToString(inc.status)
         << " | actions=" << inc.actions.size()
         << "\n";

    for (const auto& a : inc.actions) {
        cout << "  - actionId=" << a.id
             << " | done=" << (a.done ? "yes" : "no")
             << " | descr: " << a.description
             << "\n";
    }
}

} 

namespace presentation {

ConsoleMenu::ConsoleMenu(business::IncidentService& service) : service_(service) {}

void ConsoleMenu::run() {
    while (true) {
        cout << "\nWelcome to the Safety Incident Log. Select an action:\n";
        cout << "1) Create incident\n";
        cout << "2) List incidents\n";
        cout << "3) Assign incident\n";
        cout << "4) Add action\n";
        cout << "5) Mark action done\n";
        cout << "6) Close incident\n";
        cout << "0) Exit\n";
        cout << "Choose: ";

        int choice = -1;
        if (!(cin >> choice)) {
            cin.clear();
            clearLine();
            cout << "Invalid input\n";
            continue;
        }
        clearLine();

        if (choice == 0) return;

        if (choice == 1) {
            string title, location;
            int severity = 1;

            cout << "Title: ";
            getline(cin, title);

            cout << "Location: ";
            getline(cin, location);

            cout << "Severity (1-5): ";
            if (!(cin >> severity)) {
                cin.clear();
                clearLine();
                cout << "Invalid severity\n";
                continue;
            }
            clearLine();

            auto id = service_.createIncident(title, location, severity);
            cout << "Created. id=" << id << "\n";
        }
        else if (choice == 2) {
            vector<domain::Incident> list = service_.listIncidents();
            if (list.empty()) {
                cout << "No incidents\n";
                continue;
            }
            for (const auto& inc : list) {
                printIncident(inc);
            }
        }
        else if (choice == 3) {
            int64_t id = 0;
            string assignee;

            cout << "Incident id: ";
            if (!(cin >> id)) {
                cin.clear();
                clearLine();
                cout << "Invalid id\n";
                continue;
            }
            clearLine();

            cout << "Assignee: ";
            getline(cin, assignee);

            bool ok = service_.assignIncident(id, assignee);
            cout << (ok ? "Assigned\n" : "Not found\n");
        }
        else if (choice == 4) {
            int64_t id = 0;
            string descr;

            cout << "Incident id: ";
            if (!(cin >> id)) {
                cin.clear();
                clearLine();
                cout << "Invalid id\n";
                continue;
            }
            clearLine();

            cout << "Action description: ";
            getline(cin, descr);

            bool ok = service_.addAction(id, descr);
            cout << (ok ? "Action added\n" : "Not found\n");
        }
        else if (choice == 5) {
            int64_t incId = 0;
            int64_t actId = 0;

            cout << "Incident id: ";
            if (!(cin >> incId)) {
                cin.clear();
                clearLine();
                cout << "Invalid id\n";
                continue;
            }
            clearLine();

            cout << "Action id: ";
            if (!(cin >> actId)) {
                cin.clear();
                clearLine();
                cout << "Invalid id\n";
                continue;
            }
            clearLine();

            bool ok = service_.markActionDone(incId, actId);
            cout << (ok ? "Action marked done\n" : "Not found\n");
        }
        else if (choice == 6) {
            int64_t id = 0;

            cout << "Incident id: ";
            if (!(cin >> id)) {
                cin.clear();
                clearLine();
                cout << "Invalid id\n";
                continue;
            }
            clearLine();

            bool ok = service_.closeIncident(id);
            cout << (ok ? "Closed\n" : "Cannot close (not found or actions not done)\n");
        }
        else {
            cout << "Unknown option\n";
        }
    }
}

} 
