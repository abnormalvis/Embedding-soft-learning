import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 960
    height: 600
    visible: true
    title: "QML + QSqlite CRUD Demo"

    property int selectedId: -1

    function clearForm() {
        selectedId = -1
        nameInput.text = ""
        phoneInput.text = ""
        cityInput.text = ""
        ageInput.text = ""
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: "Students"
            font.pixelSize: 26
            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 10
            border.color: "#d2d8df"
            color: "#f7f9fc"

            ListView {
                id: listView
                anchors.fill: parent
                anchors.margins: 8
                clip: true
                model: studentModel
                spacing: 6

                delegate: Rectangle {
                    required property int id
                    required property string name
                    required property string phone
                    required property string city
                    required property int age

                    width: listView.width
                    height: 54
                    radius: 8
                    color: root.selectedId === id ? "#cde7ff" : "white"
                    border.color: "#c7d4e2"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Label { text: "#" + id; Layout.preferredWidth: 80 }
                        Label { text: name; Layout.preferredWidth: 180; elide: Label.ElideRight }
                        Label { text: phone; Layout.preferredWidth: 180; elide: Label.ElideRight }
                        Label { text: city; Layout.preferredWidth: 180; elide: Label.ElideRight }
                        Label { text: "Age: " + age; Layout.preferredWidth: 100 }

                        Item { Layout.fillWidth: true }

                        Button {
                            text: "Select"
                            onClicked: {
                                root.selectedId = id
                                nameInput.text = name
                                phoneInput.text = phone
                                cityInput.text = city
                                ageInput.text = age.toString()
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            color: "#ffffff"
            border.color: "#d2d8df"
            radius: 10

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Label {
                    text: root.selectedId > 0 ? "Edit Student (id=" + root.selectedId + ")" : "Create Student"
                    font.pixelSize: 18
                    font.bold: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    TextField { id: nameInput; Layout.fillWidth: true; placeholderText: "Name" }
                    TextField { id: phoneInput; Layout.fillWidth: true; placeholderText: "Phone" }
                    TextField { id: cityInput; Layout.fillWidth: true; placeholderText: "City" }
                    TextField {
                        id: ageInput
                        Layout.preferredWidth: 120
                        placeholderText: "Age"
                        validator: IntValidator { bottom: 1; top: 130 }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Button {
                        text: "Add"
                        onClicked: {
                            const parsedAge = Number(ageInput.text)
                            if (studentModel.addStudent(nameInput.text, phoneInput.text, cityInput.text, parsedAge)) {
                                clearForm()
                            }
                        }
                    }

                    Button {
                        text: "Update"
                        enabled: root.selectedId > 0
                        onClicked: {
                            const parsedAge = Number(ageInput.text)
                            if (studentModel.updateStudent(root.selectedId, nameInput.text, phoneInput.text, cityInput.text, parsedAge)) {
                                clearForm()
                            }
                        }
                    }

                    Button {
                        text: "Delete"
                        enabled: root.selectedId > 0
                        onClicked: {
                            if (studentModel.removeStudent(root.selectedId)) {
                                clearForm()
                            }
                        }
                    }

                    Button {
                        text: "Reload"
                        onClicked: studentModel.reload()
                    }

                    Button {
                        text: "Clear"
                        onClicked: clearForm()
                    }

                    Item { Layout.fillWidth: true }
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Label.Wrap
                    color: "#0f4d92"
                    text: studentModel.statusMessage
                }
            }
        }
    }
}
