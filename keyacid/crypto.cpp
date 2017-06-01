#include "crypto.h"

QByteArray Crypto::salt("keyacidkeyacidke");

QByteArray Crypto::key(crypto_secretbox_KEYBYTES,0);

void Crypto::setPassword(const QByteArray &password) {
    if (crypto_pwhash((unsigned char*)key.data(),
                      key.length(),
                      password.data(),
                      password.length(),
                      (unsigned char*)salt.data(),
                      crypto_pwhash_OPSLIMIT_INTERACTIVE,
                      crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT)!=0) {
        exit(1);
    }
}

QByteArray Crypto::secretEncrypt(const QByteArray &data) {
    QByteArray nonce(crypto_secretbox_NONCEBYTES,0);
    randombytes_buf(nonce.data(),nonce.length());
    QByteArray ret(crypto_secretbox_MACBYTES+data.length(),0);
    if (!crypto_secretbox_easy((unsigned char*)ret.data(),
                               (unsigned char*)data.data(),
                               data.length(),
                               (unsigned char*)nonce.data(),
                               (unsigned char*)key.data())) {
        return nonce+ret;
    }
    return QByteArray();
}

QByteArray Crypto::secretDecrypt(const QByteArray &data) {
    if (data.length()<int(crypto_secretbox_NONCEBYTES+crypto_secretbox_MACBYTES)) {
        return QByteArray();
    }
    QByteArray ret(data.length()-crypto_secretbox_NONCEBYTES-crypto_secretbox_MACBYTES,0);
    if (!crypto_secretbox_open_easy((unsigned char*)ret.data(),
                                    (unsigned char*)data.mid(crypto_secretbox_NONCEBYTES).data(),
                                    data.length()-crypto_secretbox_NONCEBYTES,
                                    (unsigned char*)data.data(),
                                    (unsigned char*)key.data())) {
        return ret;
    }
    return QByteArray();
}

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

QByteArray Crypto::sealedEncrypt(const QByteArray &data,const RemoteProfile &to) {
    if (!to.isValidKey()) {
        return QByteArray();
    }
    QByteArray ret(crypto_box_SEALBYTES+data.length(),0);
    if (!crypto_box_seal((unsigned char*)ret.data(),
                         (unsigned char*)data.data(),
                         data.length(),
                         (unsigned char*)to.curve25519PublicKey().data())) {
        return ret;
    }
    return QByteArray();
}

QByteArray Crypto::sealedDecrypt(const QByteArray &data,const LocalProfile &to) {
    if (!to.isValidKey()) {
        return QByteArray();
    }
    if (data.length()<int(crypto_box_SEALBYTES)) {
        return QByteArray();
    }
    QByteArray ret(data.length()-crypto_box_SEALBYTES,0);
    if (!crypto_box_seal_open((unsigned char*)ret.data(),
                              (unsigned char*)data.data(),
                              data.length(),
                              (unsigned char*)to.curve25519PublicKey().data(),
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
