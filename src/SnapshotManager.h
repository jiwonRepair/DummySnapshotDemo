#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <functional>

class SnapshotManager : public QObject {
    Q_OBJECT
public:
    explicit SnapshotManager(QObject* parent = nullptr);

    Q_INVOKABLE void captureFromQml(const QString& resource, const QString& context, const QString& data);
    Q_INVOKABLE QStringList getSnapshotList() const;

    void registerSnapshotSource(std::function<QByteArray()> func);
    void registerLoadFunction(std::function<void(QByteArray)> func);

    void capture(const QString& resource, QObject* context, const QByteArray& data, const QByteArray& meta);
    QByteArray buildMeta(const QString& functionName, const QString& operation,
                         int expected, int actual, const QString& status);


    QByteArray restore() const;
    void restoreToTarget();

private:
    std::function<QByteArray()> m_snapshotSource;
    std::function<void(QByteArray)> m_loadFunction;
    QByteArray m_lastSnapshot;    

};

#endif // SNAPSHOTMANAGER_H

