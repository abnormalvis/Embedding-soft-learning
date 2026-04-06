#include "DatabaseService.h"

#include <QProcessEnvironment>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {
const char *kConnectionName = "qt6widget_ui_demo_mysql_connection";

QString readEnvOrDefault(const QString &key, const QString &fallback) {
    const QString value = QProcessEnvironment::systemEnvironment().value(key).trimmed();
    return value.isEmpty() ? fallback : value;
}
}

DatabaseService::DatabaseService() {}

bool DatabaseService::initialize() {
    m_lastError.clear();

    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        setError(nullptr, "QMYSQL driver is unavailable. Install libqt6sql6-mysql.");
        return false;
    }

    QSqlDatabase db;
    if (QSqlDatabase::contains(kConnectionName)) {
        db = QSqlDatabase::database(kConnectionName);
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", kConnectionName);
    }

    db.setHostName(readEnvOrDefault("DB_HOST", "localhost"));

    bool ok = false;
    const int port = readEnvOrDefault("DB_PORT", "3306").toInt(&ok);
    if (!ok) {
        setError(nullptr, "Invalid DB_PORT value. Expected an integer, e.g. 3306.");
        return false;
    }
    db.setPort(port);

    db.setDatabaseName(databasePath());
    db.setUserName(readEnvOrDefault("DB_USER", "root"));
    db.setPassword(readEnvOrDefault("DB_PASSWORD", ""));

    const QString socketPath = readEnvOrDefault("DB_SOCKET", "");
    if (!socketPath.isEmpty()) {
        db.setConnectOptions("UNIX_SOCKET=" + socketPath);
    }

    if (!db.open()) {
        setError(nullptr, "MySQL open failed: " + db.lastError().text());
        return false;
    }

    QString error;
    if (!createTable(&error)) {
        setError(nullptr, "Create table failed: " + error);
        return false;
    }

    if (!seedIfEmpty(&error)) {
        setError(nullptr, "Seed data failed: " + error);
        return false;
    }

    return true;
}

QString DatabaseService::lastError() const {
    return m_lastError;
}

QVector<Student> DatabaseService::fetchAll(QString *errorMessage) const {
    QVector<Student> students;
    const QSqlDatabase db = QSqlDatabase::database(kConnectionName);
    if (!db.isOpen()) {
        setError(errorMessage, "Database is not open.");
        return students;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT id, name, phone, city, age FROM students ORDER BY id ASC")) {
        setError(errorMessage, query.lastError().text());
        return students;
    }

    while (query.next()) {
        Student s;
        s.id = query.value(0).toInt();
        s.name = query.value(1).toString();
        s.phone = query.value(2).toString();
        s.city = query.value(3).toString();
        s.age = query.value(4).toInt();
        students.append(s);
    }

    return students;
}

bool DatabaseService::addStudent(const QString &name,
                                 const QString &phone,
                                 const QString &city,
                                 int age,
                                 QString *errorMessage) {
    const QSqlDatabase db = QSqlDatabase::database(kConnectionName);
    if (!db.isOpen()) {
        setError(errorMessage, "Database is not open.");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO students(name, phone, city, age) VALUES(:name, :phone, :city, :age)");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":city", city);
    query.bindValue(":age", age);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseService::updateStudent(int id,
                                    const QString &name,
                                    const QString &phone,
                                    const QString &city,
                                    int age,
                                    QString *errorMessage) {
    const QSqlDatabase db = QSqlDatabase::database(kConnectionName);
    if (!db.isOpen()) {
        setError(errorMessage, "Database is not open.");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE students SET name=:name, phone=:phone, city=:city, age=:age WHERE id=:id");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":city", city);
    query.bindValue(":age", age);
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() == 0) {
        setError(errorMessage, "No row updated. The id may not exist.");
        return false;
    }

    return true;
}

bool DatabaseService::deleteStudent(int id, QString *errorMessage) {
    const QSqlDatabase db = QSqlDatabase::database(kConnectionName);
    if (!db.isOpen()) {
        setError(errorMessage, "Database is not open.");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM students WHERE id=:id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (query.numRowsAffected() == 0) {
        setError(errorMessage, "No row deleted. The id may not exist.");
        return false;
    }

    return true;
}

bool DatabaseService::createTable(QString *errorMessage) {
    const QSqlDatabase db = QSqlDatabase::database(kConnectionName);
    QSqlQuery query(db);

    const QString createSql =
        "CREATE TABLE IF NOT EXISTS students ("
        "id INT PRIMARY KEY AUTO_INCREMENT,"
        "name VARCHAR(128) NOT NULL,"
        "phone VARCHAR(64),"
        "city VARCHAR(128),"
        "age INT NOT NULL"
        ")";

    if (!query.exec(createSql)) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseService::seedIfEmpty(QString *errorMessage) {
    const QSqlDatabase db = QSqlDatabase::database(kConnectionName);
    QSqlQuery query(db);
    if (!query.exec("SELECT COUNT(*) FROM students")) {
        setError(errorMessage, query.lastError().text());
        return false;
    }

    if (!query.next()) {
        setError(errorMessage, "Failed to read students count.");
        return false;
    }

    if (query.value(0).toInt() > 0) {
        return true;
    }

    QSqlQuery insertQuery(db);
    if (!insertQuery.exec("INSERT INTO students(name, phone, city, age) VALUES('Alice', '13500000001', 'Hangzhou', 21)")) {
        setError(errorMessage, insertQuery.lastError().text());
        return false;
    }
    if (!insertQuery.exec("INSERT INTO students(name, phone, city, age) VALUES('Bob', '13600000002', 'Shenzhen', 23)")) {
        setError(errorMessage, insertQuery.lastError().text());
        return false;
    }

    return true;
}

QString DatabaseService::databasePath() const {
    return readEnvOrDefault("DB_NAME", "qt_demo");
}

void DatabaseService::setError(QString *errorMessage, const QString &message) const {
    m_lastError = message;
    if (errorMessage != nullptr) {
        *errorMessage = message;
    }
}

