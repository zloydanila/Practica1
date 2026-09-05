#pragma once
#include "data/IncidentRepository.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include <string>

namespace data {

class MySQLIncidentRepository final : public IncidentRepository {
public:

    MySQLIncidentRepository(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

    int64_t add(domain::Incident incident) override;
    std::optional<domain::Incident> get(int64_t id) const override;
    std::vector<domain::Incident> list() const override;
    
    bool update(const domain::Incident& incident) override;
    bool remove(int64_t id) override;

private:
    void initSchema();
    static domain::IncidentStatus statusFromString(const std::string& s);
    static std::string statusToString(domain::IncidentStatus s);

    sql::mysql::MySQL_Driver* driver_;
    std::unique_ptr<sql::Connection> conn_;
};

}