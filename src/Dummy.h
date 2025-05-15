#ifndef DUMMY_H
#define DUMMY_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include "SnapshotManager.h"
#include <QPointer>


class Dummy : public QObject {
    Q_OBJECT
public:
    explicit Dummy(SnapshotManager* sharedSnapshot, QObject* parent = nullptr);

    Q_INVOKABLE void setInputs(int a, int b);
    Q_INVOKABLE void performOperation();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void loadSnapshot(const QByteArray& data);
    Q_INVOKABLE QByteArray snapshot() const;

    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString resetReason READ resetReason NOTIFY resetReasonChanged)
    Q_PROPERTY(QString output READ output NOTIFY stateChanged)

    Q_PROPERTY(int a READ a NOTIFY stateChanged)
    Q_PROPERTY(int b READ b NOTIFY stateChanged)

    QString state() const;
    QString resetReason() const;
    QString output() const;
    int a() const { return a_; }
    int b() const { return b_; }

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

    Q_INVOKABLE void registerTextFields(QObject* inputA, QObject* inputB);

signals:
    void stateChanged();
    void resetInvoked(); // ✅ 리셋이 실행됐음을 알리는 신호
    void resetReasonChanged();

private:
    int a_ = 0;
    int b_ = 0;
    QString state_ = "Init";
    QString resetReason_ = "";
    QString output_ = "";

    SnapshotManager* sm = nullptr;  // 🔁 포인터로 변경

    QPointer<QObject> inputAField;
    QPointer<QObject> inputBField;
};

#endif // DUMMY_H
