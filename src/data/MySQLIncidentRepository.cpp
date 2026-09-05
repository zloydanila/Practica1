#include "data/MySQLIncidentRepository.h"
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <stdexcept>
#include <sstream>

namespace data {

MySQLIncidentRepository::MySQLIncidentRepository(
    const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& database)
{
    driver_ = sql::mysql::get_mysql_driver_instance();
    conn_.reset(driver_->connect(host, user, password));
    conn_->setSchema(database);
    initSchema();
}

void MySQLIncidentRepository::initSchema() {
    std::unique_ptr<sql::Statement> stmt(conn_->createStatement());

    stmt->execute(R"(
        CREATE TABLE IF NOT EXISTS incidents (
            id         BIGINT AUTO_INCREMENT PRIMARY KEY,
            title      VARCHAR(255) NOT NULL,
            location   VARCHAR(255) NOT NULL,
            severity   INT NOT NULL DEFAULT 1,
            status     VARCHAR(32) NOT NULL DEFAULT 'New',
            assignedTo VARCHAR(255) NOT NULL DEFAULT ''
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )");

    stmt->execute(R"(
        CREATE TABLE IF NOT EXISTS corrective_actions (
            id          BIGINT AUTO_INCREMENT PRIMARY KEY,
            incident_id BIGINT NOT NULL,
            description TEXT NOT NULL,
            done        TINYINT(1) NOT NULL DEFAULT 0,
            FOREIGN KEY (incident_id) REFERENCES incidents(id) ON DELETE CASCADE
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )");
}

std::string MySQLIncidentRepository::statusToString(domain::IncidentStatus s) {
    switch (s) {
        case domain::IncidentStatus::New:        return "New";
        case domain::IncidentStatus::InProgress: return "InProgress";
        case domain::IncidentStatus::Closed:     return "Closed";
    }
    return "New";
}

domain::IncidentStatus MySQLIncidentRepository::statusFromString(const std::string& s) {
    if (s == "InProgress") return domain::IncidentStatus::InProgress;
    if (s == "Closed")     return domain::IncidentStatus::Closed;
    return domain::IncidentStatus::New;
}

int64_t MySQLIncidentRepository::add(domain::Incident incident) {
    std::unique_ptr<sql::PreparedStatement> ps(conn_->prepareStatement(
        "INSERT INTO incidents (title, location, severity, status, assignedTo) "
        "VALUES (?, ?, ?, ?, ?)"
    ));
    ps->setString(1, incident.title);
    ps->setString(2, incident.location);
    ps->setInt(3, incident.severity);
    ps->setString(4, statusToString(incident.status));
    ps->setString(5, incident.assignedTo);
    ps->executeUpdate();

    std::unique_ptr<sql::Statement> s(conn_->createStatement());
    std::unique_ptr<sql::ResultSet> rs(s->executeQuery("SELECT LAST_INSERT_ID()"));
    rs->next();
    return static_cast<int64_t>(rs->getInt64(1));
}

std::optional<domain::Incident> MySQLIncidentRepository::get(int64_t id) const {
    std::unique_ptr<sql::PreparedStatement> ps(conn_->prepareStatement(
        "SELECT id,title,location,severity,status,assignedTo FROM incidents WHERE id=?"
    ));
    ps->setInt64(1, id);
    std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
    if (!rs->next()) return std::nullopt;

    domain::Incident inc;
    inc.id         = rs->getInt64("id");
    inc.title      = rs->getString("title");
    inc.location   = rs->getString("location");
    inc.severity   = rs->getInt("severity");
    inc.status     = statusFromString(rs->getString("status"));
    inc.assignedTo = rs->getString("assignedTo");

    std::unique_ptr<sql::PreparedStatement> pa(conn_->prepareStatement(
        "SELECT id,description,done FROM corrective_actions WHERE incident_id=? ORDER BY id"
    ));
    pa->setInt64(1, inc.id);
    std::unique_ptr<sql::ResultSet> ra(pa->executeQuery());
    while (ra->next()) {
        domain::CorrectiveAction a;
        a.id          = ra->getInt64("id");
        a.description = ra->getString("description");
        a.done        = ra->getBoolean("done");
        inc.actions.push_back(std::move(a));
    }
    return inc;
}

std::vector<domain::Incident> MySQLIncidentRepository::list() const {
    std::unique_ptr<sql::Statement> s(conn_->createStatement());
    std::unique_ptr<sql::ResultSet> rs(s->executeQuery(
        "SELECT id FROM incidents ORDER BY id"
    ));
    std::vector<domain::Incident> out;
    while (rs->next()) {
        auto inc = get(rs->getInt64("id"));
        if (inc) out.push_back(*inc);
    }
    return out;
}

bool MySQLIncidentRepository::update(const domain::Incident& incident) {
    std::unique_ptr<sql::PreparedStatement> ps(conn_->prepareStatement(
        "UPDATE incidents SET title=?, location=?, severity=?, status=?, assignedTo=? WHERE id=?"
    ));
    ps->setString(1, incident.title);
    ps->setString(2, incident.location);
    ps->setInt(3, incident.severity);
    ps->setString(4, statusToString(incident.status));
    ps->setString(5, incident.assignedTo);
    ps->setInt64(6, incident.id);
    int affected = ps->executeUpdate();
    if (affected == 0) return false;

    std::unique_ptr<sql::PreparedStatement> pd(conn_->prepareStatement(
        "DELETE FROM corrective_actions WHERE incident_id=?"
    ));
    pd->setInt64(1, incident.id);
    pd->executeUpdate();

    for (const auto& a : incident.actions) {
        std::unique_ptr<sql::PreparedStatement> pi(conn_->prepareStatement(
            "INSERT INTO corrective_actions (incident_id, description, done) VALUES (?,?,?)"
        ));
        pi->setInt64(1, incident.id);
        pi->setString(2, a.description);
        pi->setBoolean(3, a.done);
        pi->executeUpdate();
    }
    return true;
}

bool MySQLIncidentRepository::remove(int64_t id) {
    std::unique_ptr<sql::PreparedStatement> ps(conn_->prepareStatement(
        "DELETE FROM incidents WHERE id=?"
    ));
    ps->setInt64(1, id);
    return ps->executeUpdate() > 0;
}

}