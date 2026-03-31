#include "DatabaseService.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

namespace {
const char *kConnectionName = "qsqlited_demo_connection";
}

DatabaseService::DatabaseService() {}

bool DatabaseService::initialize() {
    QString error;

    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        return false;
    }

    QSqlDatabase db;
    if (QSqlDatabase::contains(kConnectionName)) {
        db = QSqlDatabase::database(kConnectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", kConnectionName);
    }

    db.setDatabaseName(databasePath());
    if (!db.open()) {
        return false;
    }

    if (!createTable(&error)) {
        return false;
    }

    if (!seedIfEmpty(&error)) {
        return false;
    }

    return true;
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
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone TEXT,"
        "city TEXT,"
        "age INTEGER NOT NULL"
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
    const QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString dataDirPath = appDataDir + "/data";
    QDir dataDir(dataDirPath);
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    return dataDirPath + "/demo.db";
}

void DatabaseService::setError(QString *errorMessage, const QString &message) const {
    if (errorMessage != nullptr) {
        *errorMessage = message;
    }
}
