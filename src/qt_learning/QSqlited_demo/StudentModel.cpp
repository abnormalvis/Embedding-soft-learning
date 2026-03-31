#include "StudentModel.h"

#include <QHash>

StudentModel::StudentModel(QObject *parent)
    : QAbstractListModel(parent) {
    if (!m_database.initialize()) {
        setStatusMessage("Failed to initialize database. Check QSQLITE driver availability.");
        return;
    }

    reload();
}

int StudentModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_students.size());
}

QVariant StudentModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_students.size()) {
        return QVariant();
    }

    const Student &student = m_students.at(index.row());
    switch (role) {
    case IdRole:
        return student.id;
    case NameRole:
        return student.name;
    case PhoneRole:
        return student.phone;
    case CityRole:
        return student.city;
    case AgeRole:
        return student.age;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> StudentModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[PhoneRole] = "phone";
    roles[CityRole] = "city";
    roles[AgeRole] = "age";
    return roles;
}

QString StudentModel::statusMessage() const {
    return m_statusMessage;
}

bool StudentModel::reload() {
    QString error;
    QVector<Student> students = m_database.fetchAll(&error);
    if (!error.isEmpty()) {
        setStatusMessage("Load failed: " + error);
        return false;
    }

    beginResetModel();
    m_students = students;
    endResetModel();

    setStatusMessage("Loaded " + QString::number(m_students.size()) + " rows.");
    return true;
}

bool StudentModel::addStudent(const QString &name,
                              const QString &phone,
                              const QString &city,
                              int age) {
    QString validationError;
    if (!validateInput(name, phone, city, age, &validationError)) {
        setStatusMessage(validationError);
        return false;
    }

    QString error;
    if (!m_database.addStudent(name.trimmed(), phone.trimmed(), city.trimmed(), age, &error)) {
        setStatusMessage("Add failed: " + error);
        return false;
    }

    reload();
    setStatusMessage("Student added successfully.");
    return true;
}

bool StudentModel::updateStudent(int id,
                                 const QString &name,
                                 const QString &phone,
                                 const QString &city,
                                 int age) {
    if (id <= 0) {
        setStatusMessage("Update failed: select a valid row first.");
        return false;
    }

    QString validationError;
    if (!validateInput(name, phone, city, age, &validationError)) {
        setStatusMessage(validationError);
        return false;
    }

    QString error;
    if (!m_database.updateStudent(id, name.trimmed(), phone.trimmed(), city.trimmed(), age, &error)) {
        setStatusMessage("Update failed: " + error);
        return false;
    }

    reload();
    setStatusMessage("Student updated successfully.");
    return true;
}

bool StudentModel::removeStudent(int id) {
    if (id <= 0) {
        setStatusMessage("Delete failed: select a valid row first.");
        return false;
    }

    QString error;
    if (!m_database.deleteStudent(id, &error)) {
        setStatusMessage("Delete failed: " + error);
        return false;
    }

    reload();
    setStatusMessage("Student deleted successfully.");
    return true;
}

bool StudentModel::validateInput(const QString &name,
                                 const QString &phone,
                                 const QString &city,
                                 int age,
                                 QString *errorMessage) const {
    if (name.trimmed().isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Validation failed: name cannot be empty.";
        }
        return false;
    }

    if (age < 1 || age > 130) {
        if (errorMessage != nullptr) {
            *errorMessage = "Validation failed: age should be in [1, 130].";
        }
        return false;
    }

    Q_UNUSED(phone);
    Q_UNUSED(city);
    return true;
}

void StudentModel::setStatusMessage(const QString &message) {
    if (m_statusMessage == message) {
        return;
    }

    m_statusMessage = message;
    emit statusMessageChanged();
}
