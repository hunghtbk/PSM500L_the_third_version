#include "qextserialenumerator.h"
#include "qextserialenumerator_p.h"
#include <QtCore/QDebug>

void QextSerialEnumeratorPrivate::init_sys()
{
}

void QextSerialEnumeratorPrivate::destroy_sys()
{
}

QList<QextPortInfo> QextSerialEnumeratorPrivate::getPorts_sys()
{
    QList<QextPortInfo> infoList;
    QESP_WARNING("Enumeration for POSIX systems (except Linux) is not implemented yet.");
    return infoList;
}

bool QextSerialEnumeratorPrivate::setUpNotifications_sys(bool setup)
{
    Q_UNUSED(setup)
    QESP_WARNING("Notifications for *Nix/FreeBSD are not implemented yet");
    return false;
}
