#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QString>
#include <QVector>

struct Student {
    int id;
    QString name;
    QString phone;
    QString city;
    int age;
};

class DatabaseService {
public:
    DatabaseService();

    bool initialize();
    QString lastError() const;
    QVector<Student> fetchAll(QString *errorMessage = nullptr) const;

    bool addStudent(const QString &name,
                    const QString &phone,
                    const QString &city,
                    int age,
                    QString *errorMessage = nullptr);

    bool updateStudent(int id,
                       const QString &name,
                       const QString &phone,
                       const QString &city,
                       int age,
                       QString *errorMessage = nullptr);

    bool deleteStudent(int id, QString *errorMessage = nullptr);

private:
    bool createTable(QString *errorMessage = nullptr);
    bool seedIfEmpty(QString *errorMessage = nullptr);
    QString databasePath() const;
    void setError(QString *errorMessage, const QString &message) const;

    mutable QString m_lastError;
};

#endif

