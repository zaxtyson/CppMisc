#include <mysql_connection.h>
#include <mysql_driver.h>
#include <memory>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <fmt/format.h>

using namespace std;
using namespace sql;

int main() {
//    const string host = "tcp://127.0.0.1:3306";
//    const string username = "cpp";
//    const string password = "cpptest";
//    const string db_name = "studb";

    ConnectOptionsMap options;
    options["hostName"] = "tcp://127.0.0.1";
    options["port"] = 3306;
    options["userName"] = "cpp";
    options["password"] = "cpptest";
    options["schema"] = "studb";
    options["OPT_CONNECT_TIMEOUT"] = 3;  // 3s
    options["OPT_RECONNECT"] = true;

    try {
        shared_ptr<Driver> driver(mysql::get_driver_instance());
        shared_ptr<Connection> conn(driver->connect(options));
//        conn->setSchema(db_name);

        // Query example
        SQLString sql = "SELECT CNO, CNAME, CREDIT FROM c";
        shared_ptr<Statement> stmt(conn->createStatement());
        ResultSet *res = stmt->executeQuery(sql);

        while (res->next()) {
            SQLString cno = res->getString("CNO");
            SQLString cname = res->getString("CNAME");
            int credit = res->getInt("CREDIT");
            fmt::print("课程号: {} \t 课程名: {} \t 学分: {}\n", cno.c_str(), cname.c_str(), credit);
        }

        // Update example
        sql = "INSERT INTO sc(SNO, CNO, SCORE) VALUES (?, ?, ?)";
        shared_ptr<PreparedStatement> pst(conn->prepareStatement(sql));
        pst->setString(1, "S4");
        pst->setString(2, "C7");
        pst->setInt(3, 100);

        int ret = pst->executeUpdate();
        fmt::print("Update result: {}\n", ret);
    } catch (SQLException &e) {
        fmt::print("error in {}, {}: {}\n", __FILE__, __LINE__, e.what());
    }
}
