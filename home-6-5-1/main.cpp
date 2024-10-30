#include <iostream>
#include <pqxx/pqxx> 

class ClientManager {
private:
    pqxx::connection conn;

public:
    ClientManager(const std::string& connectionString) : conn(connectionString) {}

    void createTables() {
        pqxx::work createTable(conn);
        createTable.exec("CREATE TABLE IF NOT EXISTS clients ("
            "id SERIAL PRIMARY KEY, "
            "first_name VARCHAR(255) NOT NULL, "
            "last_name VARCHAR(255) NOT NULL, "
            "email VARCHAR(255) UNIQUE NOT NULL);");

        createTable.exec("CREATE TABLE IF NOT EXISTS phones ("
            "id SERIAL PRIMARY KEY, "
            "client_id INT NOT NULL REFERENCES clients(id), "
            "phone_number VARCHAR(20) NOT NULL);");
        createTable.commit();
    }

    void addClient(const std::string& first_name, const std::string& last_name, const std::string& email) {
        pqxx::work addClient(conn);
        addClient.exec("INSERT INTO clients (first_name, last_name, email) VALUES (" +
            addClient.quote(first_name) + ", " +
            addClient.quote(last_name) + ", " +
            addClient.quote(email) + ");");
        addClient.commit();
    }

    void addPhone(int client_id, const std::string& phone_number) {
        pqxx::work addPhone(conn);
        addPhone.exec0("INSERT INTO phones (client_id, phone_number) VALUES (" +
            addPhone.quote(client_id) + ", " +
            addPhone.quote(phone_number) + ");");
        addPhone.commit();
    }

    void updateClient(int client_id, const std::string& first_name, const std::string& last_name, const std::string& email) {
        pqxx::work updateClient(conn);
        updateClient.exec("UPDATE clients SET first_name = " + updateClient.quote(first_name) +
            ", last_name = " + updateClient.quote(last_name) +
            ", email = " + updateClient.quote(email) +
            " WHERE id = " + updateClient.quote(client_id) + ";");
        updateClient.commit();
    }

    void deletePhone(int client_id, const std::string& phone_number) {
        pqxx::work deletePhone(conn);
        deletePhone.exec0("DELETE FROM phones WHERE client_id = " +
            deletePhone.quote(client_id) + " AND phone_number = " +
            deletePhone.quote(phone_number) + ";");
        deletePhone.commit();
    }

    void deleteClient(int client_id) {
        pqxx::work deleteClient(conn);
        deleteClient.exec("DELETE FROM phones WHERE client_id = " + deleteClient.quote(client_id) + ";");
        deleteClient.exec("DELETE FROM clients WHERE id = " + deleteClient.quote(client_id) + ";");
        deleteClient.commit();
    }

    void findClient(const std::string& search_term) {
        pqxx::work findClient(conn);
        pqxx::result resultFind = findClient.exec("SELECT * FROM clients WHERE first_name ILIKE " + findClient.quote("%" + search_term + "%") +
            " OR last_name ILIKE " + findClient.quote("%" + search_term + "%") +
            " OR email ILIKE " + findClient.quote("%" + search_term + "%") +
            " OR id IN (SELECT client_id FROM phones WHERE phone_number ILIKE " + findClient.quote("%" + search_term + "%") + ");");

        for (const auto& row : resultFind) {
            std::cout << "ID: " << row[0].as<int>() << ", Name: " << row[1].c_str() << " " << row[2].c_str() << ", Email: " << row[3].c_str() << '\n';
        }
    }
};

int main() {
    ClientManager cm("host=192.168.26.23 dbname=clients user=vano password=123");

    cm.createTables();
    cm.addClient("Ivan", "Semenyuta", "vano50@mail.ru");
    cm.addPhone(1, "+123456789");
    cm.addPhone(1, "+987654321");

   
    cm.findClient("Ivan");

    cm.updateClient(1, "Ivan", "Semenyuta", "vano50@mail.ru");

    cm.deletePhone(1, "+987654321");
    cm.deleteClient(1);

    return 0;
}