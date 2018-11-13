import QtQuick 2.7
import QtQuick.Controls 2.2
import "../js/Utils.js" as Utils

Rectangle{
    id: root
    visible: true
    z: 11
    width: parent.width / 3
    height: parent.height / 3
    radius: 8
    anchors.centerIn: parent
    color: "white"

    signal confirmed()
    signal cancelled()

    Keys.onPressed: {
        if(event.key === Utils.KEY_CONFIRM){
            confirmed()
        }
        else if(event.key === Utils.KEY_BACK){
            cancelled()
        }
    }

    Column{
        spacing: 8
        topPadding: 16
        Item{
            width: root.width
            height: 48

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("关机")
            }
        }

        Item{
            width: parent.width
            height: 60

            Row{
                anchors.centerIn: parent
                spacing: 16
                leftPadding: 16
                rightPadding: 16
                Rectangle{
                    id: confirmBtn
                    width: 120
                    height: 48
                    radius: 4
                    color: "lightgray"
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("确定")
                    }
                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            confirmBtn.color = "gray"
                        }
                        onReleased: {
                            confirmBtn.color = "lightgray"
                        }

                        onClicked: {
                            confirmed()
                        }
                    }

                }


            }
        }




    }
}// end poweroff confirm Dialog
