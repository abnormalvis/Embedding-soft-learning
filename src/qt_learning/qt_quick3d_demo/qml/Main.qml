import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D

ApplicationWindow {
    id: root
    visible: true
    width: 1000
    height: 640
    title: "Qt Quick 3D Teaching Demo"

    property real xRotation: 15
    property real yRotation: 25
    property real zRotation: 0
    property real scaleValue: 1.0

    RowLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#0b1220"

            View3D {
                anchors.fill: parent
                environment: SceneEnvironment {
                    clearColor: "#101a2e"
                    backgroundMode: SceneEnvironment.Color
                }

                PerspectiveCamera {
                    id: camera
                    position: Qt.vector3d(0, 120, 300)
                    eulerRotation.x: -20
                    clipFar: 2000
                }

                DirectionalLight {
                    eulerRotation.x: -40
                    eulerRotation.y: 20
                    brightness: 1.2
                }

                DirectionalLight {
                    eulerRotation.x: 20
                    eulerRotation.y: -30
                    brightness: 0.5
                }

                Model {
                    id: cube
                    source: "#Cube"
                    scale: Qt.vector3d(root.scaleValue, root.scaleValue, root.scaleValue)
                    eulerRotation: Qt.vector3d(root.xRotation, root.yRotation, root.zRotation)
                    materials: [
                        PrincipledMaterial {
                            baseColor: "#35b6ff"
                            metalness: 0.25
                            roughness: 0.18
                        }
                    ]

                    NumberAnimation on eulerRotation.y {
                        from: 0
                        to: 360
                        duration: 6000
                        loops: Animation.Infinite
                        running: autoRotateCheck.checked
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: "#f4f7fb"
            border.color: "#d8e2ef"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 10

                Label {
                    text: "Quick 3D Controls"
                    font.pixelSize: 22
                    font.bold: true
                }

                CheckBox {
                    id: autoRotateCheck
                    text: "Auto rotate"
                    checked: true
                }

                Label { text: "X Rotation" }
                Slider {
                    from: -180
                    to: 180
                    value: root.xRotation
                    onValueChanged: root.xRotation = value
                }

                Label { text: "Y Rotation" }
                Slider {
                    from: -180
                    to: 180
                    value: root.yRotation
                    onValueChanged: root.yRotation = value
                }

                Label { text: "Z Rotation" }
                Slider {
                    from: -180
                    to: 180
                    value: root.zRotation
                    onValueChanged: root.zRotation = value
                }

                Label { text: "Scale" }
                Slider {
                    from: 0.3
                    to: 2.0
                    value: root.scaleValue
                    onValueChanged: root.scaleValue = value
                }

                Item { Layout.fillHeight: true }

                Label {
                    text: "Teaching points:\n1. View3D + Camera + Light\n2. Model transform\n3. Material and animation"
                    wrapMode: Label.Wrap
                    color: "#38506e"
                }
            }
        }
    }
}
