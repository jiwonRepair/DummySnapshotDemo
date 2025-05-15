#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <functional>
#include <QtConcurrent/QtConcurrentRun>
#include <QJsonDocument>
#include <QJsonObject>

class SnapshotManager : public QObject {
    Q_OBJECT
public:
    explicit SnapshotManager(QObject* parent = nullptr);

    Q_INVOKABLE void captureFromQml(const QString& resource, const QString& context, const QString& data);
    Q_INVOKABLE QStringList getSnapshotList() const;

    void registerSnapshotSource(std::function<QByteArray()> func);
    void registerLoadFunction(std::function<void(QByteArray)> func);

    Q_INVOKABLE void capture(const QString& resource, QObject* context, const QByteArray& data, const QByteArray& meta);

    QFuture<void> captureAsync(const QString& resource, QObject* context,
                               const QByteArray& data, const QByteArray& meta);

    QFuture<void> saveSnapshotAsync(const QJsonDocument& doc, const QString& filename);

    QByteArray buildMeta(const QString& functionName, const QString& operation,
                         int expected, int actual, const QString& status);


    Q_INVOKABLE void restoreByName(const QString &name);
    void restoreLastSnapshot();                      // ⬅️ 무정지 리셋에 사용
    void restoreFromObject(const QJsonObject &obj);  // ⬅️ 클릭 복원에 사용
    QString ensureJsonSuffix(const QString &name);


private:
    std::function<QByteArray()> m_snapshotSource;
    std::function<void(QByteArray)> m_loadFunction;
    //QByteArray m_lastSnapshot;

    QJsonObject m_lastSnapshot;
};

#endif // SNAPSHOTMANAGER_H

