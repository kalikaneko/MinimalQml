import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Window {
    signal submitTextField(string text)

    // this function is our QML slot
    function setTextFieldUI(text){
        console.log("setTextField: " + text)
        textField1.text = text
    }

    visible: true
    width: 360
    height: 360

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    TextField {
        id: textField1
        x: 31
        y: 169
        placeholderText: qsTr("Enter some text...")
    }

    Button {
        id: onBtn
        x: 50
        y: 200
        text: qsTr("on")

        onClicked:
            // emit the submitTextField signal
            submitTextField(textField1.text)
    }

    Button {
        id: offBtn
        x: 150
        y: 200
        text: qsTr("off")

        onClicked:
            // emit the submitTextField signal
            submitTextField(textField1.text)
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
            delegate: EditorDelegate {
                text: styleData.value
                onTextChanged: {
                    /* 
                       this calls setData in the model,
                       which in turn emits dataChanged signal
                    */
                    model.key = text
                }
            }
        }

        TableViewColumn {
            id: valueCol
            title: "value"
            role: "value"
            movable: true
            width: treeView.viewport.width - keyCol.width
            delegate: EditorDelegate {
                text: styleData.value
                onTextChanged: {
                    /* 
                       this calls setData in the model,
                       which in turn emits dataChanged signal
                    */
                    model.value = text
                }
            }
        }
    }  
}
