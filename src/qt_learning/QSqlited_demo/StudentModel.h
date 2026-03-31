#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>

#include "DatabaseService.h"

class StudentModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    enum StudentRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PhoneRole,
        CityRole,
        AgeRole
    };

    explicit StudentModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString statusMessage() const;

    Q_INVOKABLE bool reload();
    Q_INVOKABLE bool addStudent(const QString &name,
                                const QString &phone,
                                const QString &city,
                                int age);
    Q_INVOKABLE bool updateStudent(int id,
                                   const QString &name,
                                   const QString &phone,
                                   const QString &city,
                                   int age);
    Q_INVOKABLE bool removeStudent(int id);

signals:
    void statusMessageChanged();

private:
    bool validateInput(const QString &name,
                       const QString &phone,
                       const QString &city,
                       int age,
                       QString *errorMessage) const;
    void setStatusMessage(const QString &message);

    DatabaseService m_database;
    QVector<Student> m_students;
    QString m_statusMessage;
};

#endif
