#ifndef CRYPTO_H
#define CRYPTO_H

#include "localprofile.h"

class Crypto {
public:
    static QByteArray salt,key;
    static void setPassword(const QByteArray &password);
    static QByteArray secretEncrypt(const QByteArray &data);
    static QByteArray secretDecrypt(const QByteArray &data);
    static QByteArray encrypt(const QByteArray &data,const LocalProfile &from,const RemoteProfile &to);
    static QByteArray decrypt(const QByteArray &data,const RemoteProfile &from,const LocalProfile &to);
    static QByteArray sealedEncrypt(const QByteArray &data,const RemoteProfile &to);
    static QByteArray sealedDecrypt(const QByteArray &data,const LocalProfile &to);
    static QByteArray sign(const QByteArray &data,const LocalProfile &from);
    static bool verify(const QByteArray &data,const QByteArray &signature,const RemoteProfile &from);
};

#endif // CRYPTO_H
