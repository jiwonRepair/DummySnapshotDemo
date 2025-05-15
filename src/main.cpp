#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QDebug>

#include "Dummy.h"
#include "SnapshotManager.h"
#include "ResetManager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SnapshotManager snapshot;
    Dummy dummy(&snapshot);  // ✅ 공유 인스턴스 전달
    ResetManager resetMgr;

    // 연결: Dummy를 Resettable로 등록
    resetMgr.registerResettable(&dummy);

    // snapshot 연결
    snapshot.registerSnapshotSource([&dummy]() {
        return dummy.snapshot();
    });
    snapshot.registerLoadFunction([&dummy](QByteArray data) {
        dummy.loadSnapshot(data);
    });

    // QML에 객체 바인딩
    engine.rootContext()->setContextProperty("dummy", &dummy);
    engine.rootContext()->setContextProperty("snapshot", &snapshot);

    // QML 로드
    QString qmlPath = QDir::currentPath() + "/qml/Main.qml";
    engine.load(QUrl::fromLocalFile(qmlPath));

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "❌ QML 로드 실패:" << qmlPath;
        return -1;
    }

    return app.exec();
}
