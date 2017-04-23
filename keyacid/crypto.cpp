#include "crypto.h"

QByteArray Crypto::encrypt(const QByteArray &data,const LocalProfile &from,const RemoteProfile &to) {
    if (!from.isValidKey()||!to.isValidKey()) {
        return QByteArray();
    }
    QByteArray nonce(crypto_box_NONCEBYTES,0);
    randombytes_buf(nonce.data(),nonce.length());
    QByteArray ret(crypto_box_MACBYTES+data.length(),0);
    if (!crypto_box_easy((unsigned char*)ret.data(),
                         (unsigned char*)data.data(),
                         data.length(),
                         (unsigned char*)nonce.data(),
                         (unsigned char*)to.curve25519PublicKey().data(),
                         (unsigned char*)from.curve25519PrivateKey().data())) {
        return nonce+ret;
    }
    return QByteArray();
}

QByteArray Crypto::decrypt(const QByteArray &data,const RemoteProfile &from,const LocalProfile &to) {
    if (!from.isValidKey()||!to.isValidKey()) {
        return QByteArray();
    }
    if (data.length()<int(crypto_box_NONCEBYTES+crypto_box_MACBYTES)) {
        return QByteArray();
    }
    QByteArray ret(data.length()-crypto_box_NONCEBYTES-crypto_box_MACBYTES,0);
    if (!crypto_box_open_easy((unsigned char*)ret.data(),
                              (unsigned char*)data.mid(crypto_box_NONCEBYTES).data(),
                              data.length()-crypto_box_NONCEBYTES,
                              (unsigned char*)data.data(),
                              (unsigned char*)from.curve25519PublicKey().data(),
                              (unsigned char*)to.curve25519PrivateKey().data())) {
        return ret;
    }
    return QByteArray();
}

QByteArray Crypto::sign(const QByteArray &data,const LocalProfile &from) {
    if (!from.isValidKey()) {
        return QByteArray();
    }
    QByteArray ret(crypto_sign_BYTES,0);
    crypto_sign_detached((unsigned char*)ret.data(),
                         NULL,
                         (unsigned char*)data.data(),
                         data.length(),
                         (unsigned char*)from.privateKey.data());
    return ret;
}

bool Crypto::verify(const QByteArray &data,const QByteArray &signature,const RemoteProfile &from) {
    if (!from.isValidKey()) {
        return false;
    }
    if (signature.length()!=crypto_sign_BYTES) {
        return false;
    }
    if (!crypto_sign_verify_detached((unsigned char*)signature.data(),
                                     (unsigned char*)data.data(),
                                     data.length(),
                                     (unsigned char*)from.publicKey.data())) {
        return true;
    }
    return false;
}
