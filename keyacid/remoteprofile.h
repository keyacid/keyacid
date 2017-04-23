#ifndef REMOTEPROFILE_H
#define REMOTEPROFILE_H

#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <sodium.h>

class RemoteProfile {
public:
    QString name;
    QByteArray publicKey;
    bool isValidKey() const;
    QByteArray curve25519PublicKey() const;
    QVariantMap toVariantMap() const;
    static RemoteProfile fromVariantMap(const QVariantMap &variantMap);
};

#endif // REMOTEPROFILE_H
