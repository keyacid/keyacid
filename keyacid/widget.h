#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QStringListModel>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QVariantList>
#include <QJsonDocument>
#include "crypto.h"

namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private:
    Ui::Widget *ui;
    QList<RemoteProfile>remoteProfiles;
    QList<LocalProfile>localProfiles;
    QStringListModel remoteProfilesModel,localProfilesModel;
    void saveProfiles();
    void loadProfiles();
    bool checkSelectedRemoteProfile();
    bool checkSelectedLocalProfile();
    RemoteProfile getSelectedRemoteProfile();
    LocalProfile getSelectedLocalProfile();
    void encryptMessage();
    void encryptFile();
    void anonymouslyEncryptMessage();
    void anonymouslyEncryptFile();
    void decryptMessage();
    void decryptFile();
    void anonymouslyDecryptMessage();
    void anonymouslyDecryptFile();
    void signMessage();
    void signFile();
    void verifyMessage();
    void verifyFile();
    bool loadFile(QByteArray &data);
    bool saveFile(const QByteArray &data);
    QByteArray getPlainText();
    QByteArray getCipherText();
    void setPlainText(const QByteArray &data);
    void setCipherText(const QByteArray &data);
private slots:
    void encryptClicked();
    void decryptClicked();
    void signClicked();
    void verifyClicked();
    void fileClicked();
    void operateClicked();
    void newRemoteProfileClicked();
    void newLocalProfileClicked();
    void remoteProfilesDoubleClicked(const QModelIndex &index);
    void localProfilesDoubleClicked(const QModelIndex &index);
    void changePasswordClicked();
};

#endif // WIDGET_H
