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
    ui->remoteProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->localProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->remoteProfiles->setModel(&remoteProfilesModel);
    ui->localProfiles->setModel(&localProfilesModel);
    fileClicked();
    loadProfiles();
}

void Widget::loadProfiles() {
    QSettings settings("yvbbrjdr","keyacid");
    if (settings.contains("keyacid/remoteProfileList")) {
        QVariantList remoteProfileList=settings.value("keyacid/remoteProfileList").toList();
        foreach (QVariant remoteProfile,remoteProfileList) {
            RemoteProfile tmp=RemoteProfile::fromVariantMap(remoteProfile.toMap());
            remoteProfiles.append(tmp);
            QStringList list=remoteProfilesModel.stringList();
            list.append(tmp.name);
            remoteProfilesModel.setStringList(list);
        }
        QVariantList localProfileList=settings.value("keyacid/localProfileList").toList();
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
    settings.setValue("keyacid/remoteProfileList",remoteProfileList);
    settings.setValue("keyacid/localProfileList",localProfileList);
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
}

void Widget::decryptClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(true);
    } else {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(false);
    }
}

void Widget::signClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(true);
    } else {
        ui->plainText->setReadOnly(false);
        ui->cipherText->setReadOnly(true);
    }
}

void Widget::verifyClicked() {
    if (ui->file->isChecked()) {
        ui->plainText->setReadOnly(true);
        ui->cipherText->setReadOnly(false);
    } else {
        ui->plainText->setReadOnly(false);
        ui->cipherText->setReadOnly(false);
    }
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

void Widget::operateClicked() {
    QModelIndexList selectedRemoteProfiles=ui->remoteProfiles->selectionModel()->selectedIndexes();
    QModelIndexList selectedLocalProfiles=ui->localProfiles->selectionModel()->selectedIndexes();
    if (ui->encrypt->isChecked()) {
        if (selectedLocalProfiles.empty()) {
            QMessageBox::critical(this,"Error","You have to select a local profile!");
            return;
        }
        if (selectedRemoteProfiles.empty()) {
            QMessageBox::critical(this,"Error","You have to select a remote profile!");
            return;
        }
        if (ui->file->isChecked()) {
            QFile inFile(QFileDialog::getOpenFileName(this,"Select a file to encrypt"));
            if (!inFile.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this,"Error","Failed to open the file!");
                return;
            }
            QByteArray unencrypted=inFile.readAll();
            inFile.close();
            if (unencrypted=="") {
                QMessageBox::critical(this,"Error","Don't encrypt an empty file!");
                return;
            }
            QByteArray encrypted=Crypto::encrypt(unencrypted,localProfiles[selectedLocalProfiles.front().row()],remoteProfiles[selectedRemoteProfiles.front().row()]);
            if (encrypted=="") {
                QMessageBox::critical(this,"Error","Encrypt failed! Corrupted profile.");
                return;
            }
            QFile outFile(QFileDialog::getSaveFileName(this,"Select a file to save"));
            if (!outFile.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this,"Error","Failed to open the file!");
                return;
            }
            outFile.write(encrypted);
            outFile.close();
        } else {
            if (ui->plainText->toPlainText()=="") {
                QMessageBox::critical(this,"Error","You have to encrypt something!");
                return;
            }
            QByteArray encrypted=Crypto::encrypt(ui->plainText->toPlainText().toUtf8(),localProfiles[selectedLocalProfiles.front().row()],remoteProfiles[selectedRemoteProfiles.front().row()]);
            ui->cipherText->setPlainText(encrypted.toBase64());
            if (encrypted=="") {
                QMessageBox::critical(this,"Error","Encrypt failed! Corrupted profile.");
                return;
            }
        }
    } else if (ui->decrypt->isChecked()) {
        if (selectedLocalProfiles.empty()) {
            QMessageBox::critical(this,"Error","You have to select a local profile!");
            return;
        }
        if (selectedRemoteProfiles.empty()) {
            QMessageBox::critical(this,"Error","You have to select a remote profile!");
            return;
        }
        if (ui->file->isChecked()) {
            QFile inFile(QFileDialog::getOpenFileName(this,"Select a file to decrypt"));
            if (!inFile.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this,"Error","Failed to open the file!");
                return;
            }
            QByteArray undecrypted=inFile.readAll();
            inFile.close();
            QByteArray decrypted=Crypto::decrypt(undecrypted,remoteProfiles[selectedRemoteProfiles.front().row()],localProfiles[selectedLocalProfiles.front().row()]);
            if (decrypted=="") {
                QMessageBox::critical(this,"Error","Decrypt failed! Wrong profile or tampered data.");
                return;
            }
            QFile outFile(QFileDialog::getSaveFileName(this,"Select a file to save"));
            if (!outFile.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this,"Error","Failed to open the file!");
                return;
            }
            outFile.write(decrypted);
            outFile.close();
        } else {
            QByteArray decrypted=Crypto::decrypt(QByteArray::fromBase64(ui->cipherText->toPlainText().toLocal8Bit()),remoteProfiles[selectedRemoteProfiles.front().row()],localProfiles[selectedLocalProfiles.front().row()]);
            ui->plainText->setPlainText(decrypted);
            if (decrypted=="") {
                QMessageBox::critical(this,"Error","Decrypt failed! Wrong profile or tampered data.");
                return;
            }
        }
    } else if (ui->sign->isChecked()) {
        if (selectedLocalProfiles.empty()) {
            QMessageBox::critical(this,"Error","You have to select a local profile!");
            return;
        }
        if (ui->file->isChecked()) {
            QFile inFile(QFileDialog::getOpenFileName(this,"Select a file to sign"));
            if (!inFile.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this,"Error","Failed to open the file!");
                return;
            }
            QByteArray data=inFile.readAll();
            inFile.close();
            QByteArray signature=Crypto::sign(data,localProfiles[selectedLocalProfiles.front().row()]);
            ui->cipherText->setPlainText(signature.toBase64());
            if (signature=="") {
                QMessageBox::critical(this,"Error","Sign failed! Corrupted profile.");
                return;
            }
        } else {
            QByteArray signature=Crypto::sign(ui->plainText->toPlainText().toUtf8(),localProfiles[selectedLocalProfiles.front().row()]);
            ui->cipherText->setPlainText(signature.toBase64());
            if (signature=="") {
                QMessageBox::critical(this,"Error","Sign failed! Corrupted profile.");
                return;
            }
        }
    } else if (ui->verify->isChecked()) {
        if (selectedRemoteProfiles.empty()) {
            QMessageBox::critical(this,"Error","You have to select a remote profile!");
            return;
        }
        if (ui->file->isChecked()) {
            QFile inFile(QFileDialog::getOpenFileName(this,"Select a file to sign"));
            if (!inFile.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this,"Error","Failed to open the file!");
                return;
            }
            QByteArray data=inFile.readAll();
            inFile.close();
            bool verify=Crypto::verify(data,QByteArray::fromBase64(ui->cipherText->toPlainText().toLocal8Bit()),remoteProfiles[selectedRemoteProfiles.front().row()]);
            if (verify) {
                QMessageBox::information(this,"Success","File verified!\nThis file is signed by "+selectedRemoteProfiles.front().data().toString());
                return;
            } else {
                QMessageBox::critical(this,"Error","Verify failed! Wrong profile or tampered data.");
                return;
            }
        } else {
            bool verify=Crypto::verify(ui->plainText->toPlainText().toUtf8(),QByteArray::fromBase64(ui->cipherText->toPlainText().toLocal8Bit()),remoteProfiles[selectedRemoteProfiles.front().row()]);
            if (verify) {
                QMessageBox::information(this,"Success","Message verified!\nThis message is signed by "+selectedRemoteProfiles.front().data().toString());
                return;
            } else {
                QMessageBox::critical(this,"Error","Verify failed! Wrong profile or tampered data.");
                return;
            }
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