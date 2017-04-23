#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QStringListModel>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QVariantList>
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
};

#endif // WIDGET_H
