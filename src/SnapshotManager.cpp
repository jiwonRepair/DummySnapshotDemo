#include "SnapshotManager.h"
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QDebug>

SnapshotManager::SnapshotManager(QObject* parent) : QObject(parent) {}

void SnapshotManager::registerSnapshotSource(std::function<QByteArray()> func) {
    m_snapshotSource = std::move(func);
}

void SnapshotManager::registerLoadFunction(std::function<void(QByteArray)> func) {
    m_loadFunction = std::move(func);
}

QByteArray SnapshotManager::buildMeta(const QString& functionName,
                                      const QString& operation,
                                      int expected,
                                      int actual,
                                      const QString& status)
{
    QJsonObject metaJson;
    metaJson["status"] = status;
    metaJson["operation"] = operation;
    metaJson["expected"] = expected;
    metaJson["actual"] = actual;
    metaJson["reason"] = QString("Computation failed: expected %1 but got %2").arg(expected).arg(actual);
    metaJson["function"] = functionName;

    return QJsonDocument(metaJson).toJson(QJsonDocument::Compact);
}

void SnapshotManager::capture(const QString& resource, QObject* context, const QByteArray& data, const QByteArray& meta){
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject())
        m_lastSnapshot = doc.object();  // ✅ 변환하여 저장

    captureAsync(resource, context, data, meta);
}


QFuture<void> SnapshotManager::captureAsync(const QString& resource, QObject* context,
                                            const QByteArray& data, const QByteArray& meta)
{
    QJsonObject snap;
    snap["resource"] = resource;
    snap["context"] = context ? context->objectName() : "";
    snap["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // meta 병합
    QJsonDocument metaDoc = QJsonDocument::fromJson(meta);
    if (metaDoc.isObject()) {
        const QJsonObject metaObj = metaDoc.object();
        const QStringList keys = metaObj.keys(); // detach 방지
        for (const QString& key : keys) {
            snap.insert(key, QJsonValue::fromVariant(metaObj.value(key).toVariant()));
        }
    }

    // data 파싱
    QJsonDocument inputDoc = QJsonDocument::fromJson(data);
    if (inputDoc.isObject()) {
        snap["data"] = inputDoc.object();
    } else {
        snap["data"] = QString::fromUtf8(data);
    }

    QDir().mkpath("snapshots");
    const QString filename = QString("snapshots/%1_%2.json")
                                 .arg(resource, QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QJsonDocument finalDoc(snap);
    return saveSnapshotAsync(finalDoc, filename);
}

QFuture<void> SnapshotManager::saveSnapshotAsync(const QJsonDocument& doc, const QString& filename)
{
    return QtConcurrent::run([doc, filename]() {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
            qDebug() << "[Snapshot] Saved to" << filename;
        } else {
            qWarning() << "[Snapshot] Failed to write:" << filename;
        }
    });
}


void SnapshotManager::restoreLastSnapshot() {
    restoreFromObject(m_lastSnapshot);
}

void SnapshotManager::restoreFromObject(const QJsonObject &obj) {
    if (m_loadFunction) {
        qDebug() << "[SnapshotManager] 복원 시도";
        QJsonDocument doc(obj);
        m_loadFunction(doc.toJson(QJsonDocument::Compact));
    } else {
        qWarning() << "[SnapshotManager] m_loadFunction이 등록되지 않음";
    }
}

void SnapshotManager::restoreByName(const QString &name) {
    QString path = "snapshots/" + ensureJsonSuffix(name);  // ✅ 안전하게 확장자 처리
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[SnapshotManager] Failed to open snapshot:" << path;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    restoreFromObject(doc.object());
}


void SnapshotManager::captureFromQml(const QString& resource, const QString& context, const QString& data) {
    QJsonObject snap;
    snap["resource"] = resource;
    snap["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    snap["context"] = context;

    QJsonObject result;
    result["state"] = data;
    snap["result"] = result;

    QDir().mkpath("snapshots");
    QString filename = "snapshots/" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".json";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(snap).toJson());
        file.close();
        qDebug() << "[Snapshot/QML] Saved to" << filename;
    }
}

QStringList SnapshotManager::getSnapshotList() const {
    QStringList list;
    QDir dir("snapshots");
    if (!dir.exists()) return list;
    const auto files = dir.entryInfoList(QStringList() << "*.json", QDir::Files, QDir::Time);
    for (const auto& fi : files)
        list << fi.fileName();
    return list;
}

QString SnapshotManager::ensureJsonSuffix(const QString &name) {
    return name.endsWith(".json") ? name : name + ".json";
}
