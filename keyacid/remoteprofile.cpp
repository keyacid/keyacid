#include "remoteprofile.h"

bool RemoteProfile::isValidKey() const {
    return publicKey.length()==crypto_sign_ed25519_PUBLICKEYBYTES;
}

QByteArray RemoteProfile::curve25519PublicKey() const {
    if (!isValidKey()) {
        return QByteArray();
    }
    QByteArray curve(crypto_scalarmult_curve25519_BYTES,0);
    if (!crypto_sign_ed25519_pk_to_curve25519((unsigned char*)curve.data(),
                                              (unsigned char*)publicKey.data())) {
        return curve;
    }
    return QByteArray();
}

QVariantMap RemoteProfile::toVariantMap() const {
    QVariantMap ret;
    ret["name"]=name;
    ret["publicKey"]=publicKey.toBase64();
    return ret;
}

RemoteProfile RemoteProfile::fromVariantMap(const QVariantMap &variantMap) {
    RemoteProfile ret;
    ret.name=variantMap["name"].toString();
    ret.publicKey=QByteArray::fromBase64(variantMap["publicKey"].toByteArray());
    return ret;
}
