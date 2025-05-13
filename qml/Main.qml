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
            text: dummy ? ("상태: " + dummy.state + " / 리셋 이유: " + dummy.resetReason) : "상태 정보 없음"
            font.pointSize: 14
        }

        Connections {
            target: dummy
            function onResetInvoked() {
                console.log("리셋됨")
                console.log("상태:", dummy.state)
                console.log("리셋 이유:", dummy.resetReason) // ✅ 이제 항상 최신값
            }
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
            TextField {
                id: output
                readOnly: true          // 🔹 키보드 입력 차단
                focus: false        // 🔹 포커스 자체 제거
                cursorVisible: false    // 🔹 커서 숨김
                background: null        // 🔹 (선택) 배경 제거
                placeholderText: "a+b 값"                
                width: 80
            }
            Button {
                text: "연산 실행"
                onClicked: {
                    dummy.setInputs(parseInt(inputA.text), parseInt(inputB.text))
                    dummy.performOperationGeneralized(
                        { "a": parseInt(inputA.text), "b": parseInt(inputB.text) },  // QJsonObject equivalent
                        "a + b",             // operation
                        2,                   // expected
                        parseInt(inputA.text) + parseInt(inputB.text)            // actual
                    )

                    output.text = (Number(inputA.text) + Number(inputB.text)).toString()

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
