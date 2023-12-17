#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

bool isUserExists(const string& username, sqlite3* db) {
    const char* selectQuery = "SELECT * FROM LoginInfo WHERE username = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return result == SQLITE_ROW;
}

bool checkUser(sqlite3* db) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS LoginInfo ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL);";

    if (sqlite3_exec(db, createTableQuery, nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Error creating table: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    const char* selectQuery = "SELECT * FROM LoginInfo WHERE username = ? AND password = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error preparing statement" << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return result == SQLITE_ROW;
}

void saveLoginInfo(const string& username, const string& password, sqlite3* db) {
    if (isUserExists(username, db)) {
        cout << "User with the same username already exists. Please choose a different username." << endl;
        return;
    }

    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS LoginInfo ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL);";

    if (sqlite3_exec(db, createTableQuery, nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "Error creating table: " << sqlite3_errmsg(db) << endl;
        return;
    }

    const char* insertQuery = "INSERT INTO LoginInfo (username, password) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error preparing statement" << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Error executing statement" << endl;
    }
    else {
        cout << "User information saved to the database." << endl;
    }

    sqlite3_finalize(stmt);
}

int main() {
    sqlite3* db;

    if (sqlite3_open("example.db", &db) != SQLITE_OK) {
        cerr << "Error opening database" << endl;
    }
    string choice;
    cout << "Do you want to sign up (1) or log in (2)? ";
    cin >> choice;

    if (choice == "1") {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        saveLoginInfo(username, password, db);
    }
    else if (choice == "2") {
        if (checkUser(db)) {
            cout << "Login successful!" << endl;
        }
        else {
            cout << "Login failed. Invalid username or password." << endl;
        }
    }
    else {
        cout << "Invalid choice." << endl;
    }

    sqlite3_close(db);
}
