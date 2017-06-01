#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent):QWidget(parent),ui(new Ui::Widget) {
    ui->setupUi(this);
    connect(ui->encrypt,SIGNAL(clicked(bool)),this,SLOT(encryptClicked()));
    connect(ui->decrypt,SIGNAL(clicked(bool)),this,SLOT(decryptClicked()));
    connect(ui->sign,SIGNAL(clicked(bool)),this,SLOT(signClicked()));
    connect(ui->verify,SIGNAL(clicked(bool)),this,SLOT(verifyClicked()));
    connect(ui->file,SIGNAL(clicked(bool)),this,SLOT(fileClicked()));
    connect(ui->operate,SIGNAL(clicked(bool)),this,SLOT(operateClicked()));
    connect(ui->newRemoteProfile,SIGNAL(clicked(bool)),this,SLOT(newRemoteProfileClicked()));
    connect(ui->newLocalProfile,SIGNAL(clicked(bool)),this,SLOT(newLocalProfileClicked()));
    connect(ui->remoteProfiles,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(remoteProfilesDoubleClicked(QModelIndex)));
    connect(ui->localProfiles,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(localProfilesDoubleClicked(QModelIndex)));
    connect(ui->changePassword,SIGNAL(clicked(bool)),this,SLOT(changePasswordClicked()));
    ui->cipherText->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->remoteProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->localProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->remoteProfiles->setModel(&remoteProfilesModel);
    ui->localProfiles->setModel(&localProfilesModel);
    fileClicked();
    bool ok;
    QString password=QInputDialog::getText(this,"Password","Please enter the password\nIf it's the first time that you run keyacid, please set one",QLineEdit::Password,"",&ok);
    if (!ok) {
        exit(0);
    }
    Crypto::setPassword(password.toLocal8Bit());
    loadProfiles();
}

void Widget::loadProfiles() {
    QSettings settings("yvbbrjdr","keyacid");
    if (settings.contains("keyacid/remoteProfiles")) {
        QByteArray decrypted=Crypto::secretDecrypt(QByteArray::fromBase64(settings.value("keyacid/remoteProfiles").toByteArray()));
        if (decrypted=="") {
            QMessageBox::critical(this,"Password","You entered a wrong password!");
            exit(0);
        }
        QVariantList remoteProfileList=QJsonDocument::fromJson(decrypted).toVariant().toList();
        foreach (QVariant remoteProfile,remoteProfileList) {
            RemoteProfile tmp=RemoteProfile::fromVariantMap(remoteProfile.toMap());
            remoteProfiles.append(tmp);
            QStringList list=remoteProfilesModel.stringList();
            list.append(tmp.name);
            remoteProfilesModel.setStringList(list);
        }
        decrypted=Crypto::secretDecrypt(QByteArray::fromBase64(settings.value("keyacid/localProfiles").toByteArray()));
        if (decrypted=="") {
            QMessageBox::critical(this,"Password","You entered a wrong password!");
            exit(0);
        }
        QVariantList localProfileList=QJsonDocument::fromJson(decrypted).toVariant().toList();
        foreach (QVariant localProfile,localProfileList) {
            LocalProfile tmp=LocalProfile::fromVariantMap(localProfile.toMap());
            localProfiles.append(tmp);
            QStringList list=localProfilesModel.stringList();
            list.append(tmp.name);
            localProfilesModel.setStringList(list);
        }
    }
}

void Widget::saveProfiles() {
    QSettings settings("yvbbrjdr","keyacid");
    QVariantList remoteProfileList,localProfileList;
    foreach (RemoteProfile remoteProfile,remoteProfiles) {
        remoteProfileList.append(remoteProfile.toVariantMap());
    }
    foreach (LocalProfile localProfile,localProfiles) {
        localProfileList.append(localProfile.toVariantMap());
    }
    settings.setValue("keyacid/remoteProfiles",Crypto::secretEncrypt(QJsonDocument::fromVariant(remoteProfileList).toJson()).toBase64());
    settings.setValue("keyacid/localProfiles",Crypto::secretEncrypt(QJsonDocument::fromVariant(localProfileList).toJson()).toBase64());
}

Widget::~Widget() {
    delete ui;
}

void Widget::encryptClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(true);
    } else {
        ui->plainText->setReadOnly(false);
        ui->cipherText->setReadOnly(true);
    }
    ui->anonymous->setEnabled(true);
}

void Widget::decryptClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(true);
    } else {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(false);
    }
    ui->anonymous->setEnabled(true);
}

void Widget::signClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(true);
    } else {
        ui->plainText->setReadOnly(false);
        ui->cipherText->setReadOnly(true);
    }
    ui->anonymous->setEnabled(false);
    ui->anonymous->setChecked(false);
}

void Widget::verifyClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(false);
    } else {
        ui->plainText->setReadOnly(false);
        ui->cipherText->setReadOnly(false);
    }
    ui->anonymous->setEnabled(false);
    ui->anonymous->setChecked(false);
}

void Widget::fileClicked() {
    if (ui->encrypt->isChecked()) {
        encryptClicked();
    } else if (ui->decrypt->isChecked()) {
        decryptClicked();
    } else if (ui->sign->isChecked()) {
        signClicked();
    } else if (ui->verify->isChecked()) {
        verifyClicked();
    }
    if (ui->file->isChecked()) {
        ui->encrypt->setText("Encrypt");
        ui->decrypt->setText("Decrypt");
        ui->sign->setText("Sign>>");
        ui->verify->setText("Verify<<");
    } else {
        ui->encrypt->setText(">>Encrypt>>");
        ui->decrypt->setText("<<Decrypt<<");
        ui->sign->setText(">>Sign>>");
        ui->verify->setText(">>Verify<<");
    }
}

bool Widget::checkSelectedRemoteProfile() {
    if (ui->remoteProfiles->selectionModel()->selectedIndexes().empty()) {
        QMessageBox::critical(this,"Error","You have to select a remote profile!");
        return false;
    }
    return true;
}

bool Widget::checkSelectedLocalProfile() {
    if (ui->localProfiles->selectionModel()->selectedIndexes().empty()) {
        QMessageBox::critical(this,"Error","You have to select a local profile!");
        return false;
    }
    return true;
}

RemoteProfile Widget::getSelectedRemoteProfile() {
    return remoteProfiles[ui->remoteProfiles->selectionModel()->selectedIndexes().front().row()];
}

LocalProfile Widget::getSelectedLocalProfile() {
    return localProfiles[ui->localProfiles->selectionModel()->selectedIndexes().front().row()];
}

void Widget::encryptMessage() {
    if (getPlainText()=="") {
        QMessageBox::critical(this,"Error","You have to encrypt something!");
        return;
    }
    QByteArray encrypted=Crypto::encrypt(getPlainText(),getSelectedLocalProfile(),getSelectedRemoteProfile());
    setCipherText(encrypted);
    if (encrypted=="") {
        QMessageBox::critical(this,"Error","Encrypt failed! Corrupted profile.");
    }
}

void Widget::encryptFile() {
    QByteArray unencrypted;
    if (!loadFile(unencrypted)) {
        return;
    }
    if (unencrypted=="") {
        QMessageBox::critical(this,"Error","Don't encrypt an empty file!");
        return;
    }
    QByteArray encrypted=Crypto::encrypt(unencrypted,getSelectedLocalProfile(),getSelectedRemoteProfile());
    if (encrypted=="") {
        QMessageBox::critical(this,"Error","Encrypt failed! Corrupted profile.");
        return;
    }
    saveFile(encrypted);
}

void Widget::anonymouslyEncryptMessage() {
    if (getPlainText()=="") {
        QMessageBox::critical(this,"Error","You have to encrypt something!");
        return;
    }
    QByteArray encrypted=Crypto::sealedEncrypt(getPlainText(),getSelectedRemoteProfile());
    setCipherText(encrypted);
    if (encrypted=="") {
        QMessageBox::critical(this,"Error","Encrypt failed! Corrupted profile.");
    }
}

void Widget::anonymouslyEncryptFile() {
    QByteArray unencrypted;
    if (!loadFile(unencrypted)) {
        return;
    }
    if (unencrypted=="") {
        QMessageBox::critical(this,"Error","Don't encrypt an empty file!");
        return;
    }
    QByteArray encrypted=Crypto::sealedEncrypt(unencrypted,getSelectedRemoteProfile());
    if (encrypted=="") {
        QMessageBox::critical(this,"Error","Encrypt failed! Corrupted profile.");
        return;
    }
    saveFile(encrypted);
}

void Widget::decryptMessage() {
    QByteArray decrypted=Crypto::decrypt(getCipherText(),getSelectedRemoteProfile(),getSelectedLocalProfile());
    setPlainText(decrypted);
    if (decrypted=="") {
        QMessageBox::critical(this,"Error","Decrypt failed! Wrong profile or tampered data.");
    }
}

void Widget::decryptFile() {
    QByteArray undecrypted;
    if (!loadFile(undecrypted)) {
        return;
    }
    QByteArray decrypted=Crypto::decrypt(undecrypted,getSelectedRemoteProfile(),getSelectedLocalProfile());
    if (decrypted=="") {
        QMessageBox::critical(this,"Error","Decrypt failed! Wrong profile or tampered data.");
        return;
    }
    saveFile(decrypted);
}

void Widget::anonymouslyDecryptMessage() {
    QByteArray decrypted=Crypto::sealedDecrypt(getCipherText(),getSelectedLocalProfile());
    setPlainText(decrypted);
    if (decrypted=="") {
        QMessageBox::critical(this,"Error","Decrypt failed! Wrong profile or tampered data.");
    }
}

void Widget::anonymouslyDecryptFile() {
    QByteArray undecrypted;
    if (!loadFile(undecrypted)) {
        return;
    }
    QByteArray decrypted=Crypto::sealedDecrypt(undecrypted,getSelectedLocalProfile());
    if (decrypted=="") {
        QMessageBox::critical(this,"Error","Decrypt failed! Wrong profile or tampered data.");
        return;
    }
    saveFile(decrypted);
}

void Widget::signMessage() {
    QByteArray signature=Crypto::sign(getPlainText(),getSelectedLocalProfile());
    setCipherText(signature);
    if (signature=="") {
        QMessageBox::critical(this,"Error","Sign failed! Corrupted profile.");
    }
}

void Widget::signFile() {
    QByteArray data;
    loadFile(data);
    QByteArray signature=Crypto::sign(data,getSelectedLocalProfile());
    setCipherText(signature);
    if (signature=="") {
        QMessageBox::critical(this,"Error","Sign failed! Corrupted profile.");
    }
}

void Widget::verifyMessage() {
    if (Crypto::verify(getPlainText(),getCipherText(),getSelectedRemoteProfile())) {
        QMessageBox::information(this,"Success","Message verified!\nThis message is signed by "+getSelectedRemoteProfile().name);
    } else {
        QMessageBox::critical(this,"Error","Verify failed! Wrong profile or tampered data.");
    }
}

void Widget::verifyFile() {
    QByteArray data;
    if (!loadFile(data)) {
        return;
    }
    if (Crypto::verify(data,getCipherText(),getSelectedRemoteProfile())) {
        QMessageBox::information(this,"Success","File verified!\nThis file is signed by "+getSelectedRemoteProfile().name);
    } else {
        QMessageBox::critical(this,"Error","Verify failed! Wrong profile or tampered data.");
    }
}

bool Widget::loadFile(QByteArray &data) {
    QString filename=QFileDialog::getOpenFileName(this,"Load File");
    if (filename.size()==0) {
        return false;
    }
    QFile inFile(filename);
    if (!inFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,"Error","Failed to open the file!");
        return false;
    }
    data=inFile.readAll();
    inFile.close();
    return true;
}

bool Widget::saveFile(const QByteArray &data) {
    QString filename=QFileDialog::getSaveFileName(this,"Save File");
    if (filename.size()==0) {
        return false;
    }
    QFile outFile(filename);
    if (!outFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error","Failed to open the file!");
        return false;
    }
    outFile.write(data);
    outFile.close();
    return true;
}

QByteArray Widget::getPlainText() {
    return ui->plainText->toPlainText().toUtf8();
}

QByteArray Widget::getCipherText() {
    return QByteArray::fromBase64(ui->cipherText->toPlainText().toLocal8Bit());
}

void Widget::setPlainText(const QByteArray &data) {
    ui->plainText->setPlainText(data);
}

void Widget::setCipherText(const QByteArray &data) {
    ui->cipherText->setPlainText(data.toBase64());
}

void Widget::operateClicked() {
    if (ui->encrypt->isChecked()) {
        if (ui->file->isChecked()) {
            if (ui->anonymous->isChecked()) {
                if (!checkSelectedRemoteProfile()) {
                    return;
                }
                anonymouslyEncryptFile();
            } else {
                if (!checkSelectedLocalProfile()||!checkSelectedRemoteProfile()) {
                    return;
                }
                encryptFile();
            }
        } else {
            if (ui->anonymous->isChecked()) {
                if (!checkSelectedRemoteProfile()) {
                    return;
                }
                anonymouslyEncryptMessage();
            } else {
                if (!checkSelectedLocalProfile()||!checkSelectedRemoteProfile()) {
                    return;
                }
                encryptMessage();
            }
        }
    } else if (ui->decrypt->isChecked()) {
        if (ui->file->isChecked()) {
            if (ui->anonymous->isChecked()) {
                if (!checkSelectedLocalProfile()) {
                    return;
                }
                anonymouslyDecryptFile();
            } else {
                if (!checkSelectedLocalProfile()||!checkSelectedRemoteProfile()) {
                    return;
                }
                decryptFile();
            }
        } else {
            if (ui->anonymous->isChecked()) {
                if (!checkSelectedLocalProfile()) {
                    return;
                }
                anonymouslyDecryptMessage();
            } else {
                if (!checkSelectedLocalProfile()||!checkSelectedRemoteProfile()) {
                    return;
                }
                decryptMessage();
            }
        }
    } else if (ui->sign->isChecked()) {
        if (!checkSelectedLocalProfile()) {
            return;
        }
        if (ui->file->isChecked()) {
            signFile();
        } else {
            signMessage();
        }
    } else if (ui->verify->isChecked()) {
        if (!checkSelectedRemoteProfile()) {
            return;
        }
        if (ui->file->isChecked()) {
            verifyFile();
        } else {
            verifyMessage();
        }
    }
}

void Widget::newRemoteProfileClicked() {
    bool ok;
    RemoteProfile tmp;
    do {
        tmp.name=QInputDialog::getText(this,"New Remote Profile","Please enter the name of this profile",QLineEdit::Normal,"",&ok);
        if (!ok) {
            return;
        }
        if (tmp.name.size()) {
            break;
        } else {
            QMessageBox::critical(this,"Error","An empty name is not acceptable!");
        }
    } while (1);
    do {
        QString publicKey=QInputDialog::getText(this,"New Remote Profile","Please enter the public key of this profile",QLineEdit::Normal,"",&ok);
        if (!ok) {
            return;
        }
        tmp.publicKey=QByteArray::fromBase64(publicKey.toLocal8Bit());
        if (tmp.isValidKey()) {
            break;
        } else {
            QMessageBox::critical(this,"Error","You entered an invalid public key!");
            tmp.publicKey="";
        }
    } while (1);
    remoteProfiles.append(tmp);
    QStringList list=remoteProfilesModel.stringList();
    list.append(tmp.name);
    remoteProfilesModel.setStringList(list);
    saveProfiles();
}

void Widget::newLocalProfileClicked() {
    bool ok;
    LocalProfile tmp;
    do {
        tmp.name=QInputDialog::getText(this,"New Local Profile","Please enter the name of this profile",QLineEdit::Normal,"",&ok);
        if (!ok) {
            return;
        }
        if (tmp.name.size()) {
            break;
        } else {
            QMessageBox::critical(this,"Error","An empty name is not acceptable!");
        }
    } while (1);
    do {
        QString privateKey=QInputDialog::getText(this,"New Local Profile","Please enter the private key of this profile\n(leave blank to generate a new key)\nKEEP IT EXTREMELY PRIVATE AND SECURE!",QLineEdit::Normal,tmp.privateKey.toBase64(),&ok);
        if (!ok) {
            return;
        }
        if (privateKey.size()) {
            tmp.privateKey=QByteArray::fromBase64(privateKey.toLocal8Bit());
            if (tmp.generatePublicKey()) {
                break;
            } else {
                QMessageBox::critical(this,"Error","You entered an invalid private key!");
                tmp.privateKey="";
            }
        } else {
            tmp.generateKeyPair();
        }
    } while (1);
    QInputDialog::getText(this,"New Local Profile","This is your public key.\nYOU CAN SEND IT TO ANYONE!",QLineEdit::Normal,tmp.publicKey.toBase64(),&ok);
    if (!ok) {
        return;
    }
    localProfiles.append(tmp);
    QStringList list=localProfilesModel.stringList();
    list.append(tmp.name);
    localProfilesModel.setStringList(list);
    saveProfiles();
}

void Widget::remoteProfilesDoubleClicked(const QModelIndex &index) {
    int row=index.row();
    RemoteProfile tmp=remoteProfiles[row];
    bool ok;
    do {
        tmp.name=QInputDialog::getText(this,"Edit Remote Profile","Please enter the name of this profile\n(leave blank to delete, NOT REVERSIBLE)",QLineEdit::Normal,tmp.name,&ok);
        if (!ok) {
            return;
        }
        if (tmp.name.size()) {
            break;
        } else {
            remoteProfiles.removeAt(row);
            QStringList list=remoteProfilesModel.stringList();
            list.removeAt(row);
            remoteProfilesModel.setStringList(list);
            saveProfiles();
            return;
        }
    } while (1);
    do {
        QString publicKey=QInputDialog::getText(this,"Edit Remote Profile","Please enter the public key of this profile",QLineEdit::Normal,tmp.publicKey.toBase64(),&ok);
        if (!ok) {
            return;
        }
        tmp.publicKey=QByteArray::fromBase64(publicKey.toLocal8Bit());
        if (tmp.isValidKey()) {
            break;
        } else {
            QMessageBox::critical(this,"Error","You entered an invalid public key!");
            tmp.publicKey="";
        }
    } while (1);
    remoteProfiles[row]=tmp;
    QStringList list=remoteProfilesModel.stringList();
    list[row]=tmp.name;
    remoteProfilesModel.setStringList(list);
    saveProfiles();
}

void Widget::localProfilesDoubleClicked(const QModelIndex &index) {
    int row=index.row();
    LocalProfile tmp=localProfiles[row];
    bool ok;
    do {
        tmp.name=QInputDialog::getText(this,"Edit Local Profile","Please enter the name of this profile\n(leave blank to delete, NOT REVERSIBLE)",QLineEdit::Normal,tmp.name,&ok);
        if (!ok) {
            return;
        }
        if (tmp.name.size()) {
            break;
        } else {
            localProfiles.removeAt(row);
            QStringList list=localProfilesModel.stringList();
            list.removeAt(row);
            localProfilesModel.setStringList(list);
            saveProfiles();
            return;
        }
    } while (1);
    do {
        QString privateKey=QInputDialog::getText(this,"Edit Local Profile","Please enter the private key of this profile\n(leave blank to generate a new key)\nKEEP IT EXTREMELY PRIVATE AND SECURE!",QLineEdit::Normal,tmp.privateKey.toBase64(),&ok);
        if (!ok) {
            return;
        }
        if (privateKey.size()) {
            tmp.privateKey=QByteArray::fromBase64(privateKey.toLocal8Bit());
            if (tmp.generatePublicKey()) {
                break;
            } else {
                QMessageBox::critical(this,"Error","You entered an invalid private key!");
                tmp.privateKey="";
            }
        } else {
            tmp.generateKeyPair();
        }
    } while (1);
    QInputDialog::getText(this,"Edit Local Profile","This is your public key.\nYOU CAN SEND IT TO ANYONE!",QLineEdit::Normal,tmp.publicKey.toBase64(),&ok);
    if (!ok) {
        return;
    }
    localProfiles[row]=tmp;
    QStringList list=localProfilesModel.stringList();
    list[row]=tmp.name;
    localProfilesModel.setStringList(list);
    saveProfiles();
}

void Widget::changePasswordClicked() {
    bool ok;
    QString password=QInputDialog::getText(this,"Password","Enter a new password",QLineEdit::Password,"",&ok);
    if (!ok) {
        return;
    }
    Crypto::setPassword(password.toLocal8Bit());
    saveProfiles();
    QMessageBox::information(this,"Password","New password is set successfully!");
}
