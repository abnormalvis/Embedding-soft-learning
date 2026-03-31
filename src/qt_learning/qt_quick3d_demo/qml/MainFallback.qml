import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 800
    height: 480
    title: "Qt Quick 3D Demo (Fallback)"

    Rectangle {
        anchors.fill: parent
        color: "#f3f7fb"

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 14

            Label {
                text: "Qt Quick3D module is not installed"
                font.pixelSize: 26
                font.bold: true
            }

            Label {
                text: "Install Quick3D dev package and rebuild to run the 3D scene."
                font.pixelSize: 16
                color: "#35506b"
            }

            TextArea {
                readOnly: true
                text: "Ubuntu example:\n  sudo apt install qt6-quick3d-dev"
                Layout.preferredWidth: 520
                Layout.preferredHeight: 90
            }
        }
    }
}
