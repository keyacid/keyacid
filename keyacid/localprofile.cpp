#include "localprofile.h"

bool LocalProfile::isValidKey() const {
    if (!((RemoteProfile*)this)->isValidKey()) {
        return false;
    }
    if (privateKey.length()!=crypto_sign_ed25519_SECRETKEYBYTES) {
        return false;
    }
    QByteArray tmpPublicKey(crypto_sign_ed25519_PUBLICKEYBYTES,0);
    crypto_sign_ed25519_sk_to_pk((unsigned char*)tmpPublicKey.data(),
                                 (unsigned char*)privateKey.data());
    return publicKey==tmpPublicKey;
}

void LocalProfile::generateKeyPair() {
    publicKey=QByteArray(crypto_sign_ed25519_PUBLICKEYBYTES,0);
    privateKey=QByteArray(crypto_sign_ed25519_SECRETKEYBYTES,0);
    crypto_sign_ed25519_keypair((unsigned char*)publicKey.data(),
                                (unsigned char*)privateKey.data());
}

bool LocalProfile::generatePublicKey() {
    if (privateKey.length()!=crypto_sign_ed25519_SECRETKEYBYTES) {
        return false;
    }
    publicKey=QByteArray(crypto_sign_ed25519_PUBLICKEYBYTES,0);
    crypto_sign_ed25519_sk_to_pk((unsigned char*)publicKey.data(),
                                 (unsigned char*)privateKey.data());
    return true;
}

QByteArray LocalProfile::curve25519PrivateKey() const {
    if (!isValidKey()) {
        return QByteArray();
    }
    QByteArray curve(crypto_scalarmult_curve25519_BYTES,0);
    if (!crypto_sign_ed25519_sk_to_curve25519((unsigned char*)curve.data(),
                                              (unsigned char*)privateKey.data()))
        return curve;
    return QByteArray();
}

QVariantMap LocalProfile::toVariantMap() const {
    QVariantMap ret;
    ret["name"]=name;
    ret["publicKey"]=publicKey;
    ret["privateKey"]=privateKey;
    return ret;
}

LocalProfile LocalProfile::fromVariantMap(const QVariantMap &variantMap) {
    LocalProfile ret;
    ret.name=variantMap["name"].toString();
    ret.publicKey=variantMap["publicKey"].toByteArray();
    ret.privateKey=variantMap["privateKey"].toByteArray();
    return ret;
}
