import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 500
    title: qsTr("Snapshot Reset Demo")

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "상태: " + dummy.state
            font.pointSize: 14
        }

        Row {
            spacing: 10
            TextField {
                id: inputA
                placeholderText: "a 값"
                width: 80
            }
            TextField {
                id: inputB
                placeholderText: "b 값"
                width: 80
            }
            Button {
                text: "연산 실행"
                onClicked: {
                    dummy.setInputs(parseInt(inputA.text), parseInt(inputB.text))
                    //dummy.performOperation()
                    dummy.performOperationGeneralized(
                        { "a": parseInt(inputA.text), "b": parseInt(inputB.text) },  // QJsonObject equivalent
                        "a + b",             // operation
                        2,                   // expected
                        parseInt(inputA.text) + parseInt(inputB.text)            // actual
                    )

                    if (dummy.state !== "OK") {
                        //snapshot.captureFromQml("Ui", "dummy", dummy.state)
                        snapshotListModel.clear()
                        snapshot.getSnapshotList().forEach(function(name) {
                            snapshotListModel.append({ name: name })
                        })
                    }
                }
            }

        }

        Text {
            text: "📄 최근 스냅샷"
            font.bold: true
        }

        ListView {
            id: snapshotListView
            width: parent ? parent.width - 40 : 360
            height: 140
            model: ListModel { id: snapshotListModel }

            delegate: Item {
                width: snapshotListView.width
                height: 30
                Rectangle {
                    width: snapshotListView.width
                    height: 28
                    color: "#eeeeee"
                    border.color: "#cccccc"
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: name
                        font.pointSize: 10
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: dummy.loadSnapshot(name)
                    }
                }
            }
        }
    }
}
