#ifndef _QEXTSERIALENUMERATOR_H_
#define _QEXTSERIALENUMERATOR_H_

#include <QtCore/QList>
#include <QtCore/QObject>
#include "qextserialport_global.h"

struct QextPortInfo {
    QString portName;   ///< Port name.
    QString physName;   ///< Physical name.
    QString friendName; ///< Friendly name.
    QString enumName;   ///< Enumerator name.
    int vendorID;       ///< Vendor ID.
    int productID;      ///< Product ID
};

class QextSerialEnumeratorPrivate;
class QEXTSERIALPORT_EXPORT QextSerialEnumerator : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QextSerialEnumerator)
public:
    QextSerialEnumerator(QObject *parent=0);
    ~QextSerialEnumerator();

    static QList<QextPortInfo> getPorts();
    void setUpNotifications();

Q_SIGNALS:
    void deviceDiscovered(const QextPortInfo &info);
    void deviceRemoved(const QextPortInfo &info);

private:
    Q_DISABLE_COPY(QextSerialEnumerator)
#if defined(Q_OS_LINUX) && !defined(QESP_NO_UDEV)
    Q_PRIVATE_SLOT(d_func(), void _q_deviceEvent())
#endif
    QextSerialEnumeratorPrivate *d_ptr;
};

#endif /*_QEXTSERIALENUMERATOR_H_*/
