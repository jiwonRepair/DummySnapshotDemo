import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 400
    title: "출력 복원 테스트"

    ListModel { id: snapshotListModel }

    Column {
        anchors.centerIn: parent
        spacing: 10

        TextField {
            id: inputA
            width: 100
            placeholderText: "숫자 A"
        }

        TextField {
            id: inputB
            width: 100
            placeholderText: "숫자 B"
        }

        TextField {
            id: output
            width: 100
            readOnly: true
            cursorVisible: false
            background: null
            placeholderText: "출력값"
            text: dummy ? dummy.output : "";
        }

        Button {
            text: "연산 실행"
            onClicked: {
                const a = parseInt(inputA.text)
                const b = parseInt(inputB.text)
                const result = a + b

                dummy.performOperationGeneralized(
                    { "a": a, "b": b },
                    "a + b",
                    2,
                    result
                )

                // 상태가 FAIL이면 snapshot 리스트 갱신
                if (dummy.state !== "OK") {
                    // 🔴 snapshot 저장이 완료되도록 약간 지연
                    Qt.callLater(function() {
                        snapshotListModel.clear()
                        snapshot.getSnapshotList().forEach(function(name) {
                            snapshotListModel.append({ name: name })
                        })
                    })
                }
            }
        }

        Text {
            font.pointSize: 14
            text: dummy ? ("상태: " + dummy.state + " / 리셋 이유: " + dummy.resetReason) : "상태 정보 없음"
        }

        ListView {
            width: 200
            height: 100
            model: snapshotListModel
            delegate: Text { text: name }
        }

        Connections {
            target: dummy
            function onResetInvoked() {
                console.log("✅ 리셋 감지됨 → 출력값:", dummy.output)
            }
        }
    }
}
