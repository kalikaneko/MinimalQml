import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Window {
    signal switchOn()
    signal switchOff()
    signal unblock()

    visible: true
    width: 360
    height: 360

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Button {
        id: onBtn
        x: 50
        y: 200
        text: qsTr("on")

        onClicked: switchOn()
    }

    Button {
        id: offBtn
        x: 150
        y: 200
        text: qsTr("off")

        onClicked: switchOff()
    }

    TreeView {
        id: treeView
        model: jsonModel
        anchors.left: parent.left
        anchors.right: parent.right

        TableViewColumn {
            id: keyCol
            title: "key"
            role: "key"
        }

        TableViewColumn {
            id: valueCol
            title: "value"
            role: "value"
            movable: true
            width: treeView.viewport.width - keyCol.width
        }
    }  
}
