#ifndef LOCALPROFILE_H
#define LOCALPROFILE_H

#include "remoteprofile.h"

class LocalProfile : public RemoteProfile {
public:
    QByteArray privateKey;
    void generateKeyPair();
    bool generatePublicKey();
    bool isValidKey() const;
    QByteArray curve25519PrivateKey() const;
    QVariantMap toVariantMap() const;
    static LocalProfile fromVariantMap(const QVariantMap &variantMap);
};

#endif // LOCALPROFILE_H
