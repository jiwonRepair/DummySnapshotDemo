#ifndef DUMMY_H
#define DUMMY_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include "SnapshotManager.h"
#include "ResetManager.h"

class Dummy : public QObject {
    Q_OBJECT
public:
    explicit Dummy(QObject* parent = nullptr);

    Q_INVOKABLE void setInputs(int a, int b);
    Q_INVOKABLE void performOperation();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void loadSnapshot(const QByteArray& data);
    Q_INVOKABLE QByteArray snapshot() const;

    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    QString state() const;

    /**
     * @brief 일반적인 연산 검사 후 실패 시 자동으로 스냅샷 및 리셋을 수행합니다.
     *
     * @param input 원본 입력 JSON
     * @param operation 수행된 연산 설명 (예: "a + b")
     * @param expected 기대값
     * @param actual 실제 계산 결과
     */
    Q_INVOKABLE void performOperationGeneralized(const QJsonObject& input,
                                     const QString& operation,
                                     const QVariant& expected,
                                     const QVariant& actual);

signals:
    void stateChanged();

private:
    int a_ = 0;
    int b_ = 0;
    QString state_ = "Init";

    SnapshotManager sm;
    ResetManager rm;
};

#endif // DUMMY_H
