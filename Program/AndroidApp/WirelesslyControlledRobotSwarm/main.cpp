#include "mainwindow.h"

#include <QApplication>

#include <QDir>
#include <QScreen>
#include <QtCore/QUrl>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QGuiApplication>
#include <QStyleHints>
#include <QtQml/QQmlContext>
#include <QtWebView/QtWebView>

#ifdef REQUEST_PERMISSIONS_ON_ANDROID
#include <QtAndroid>

bool requestStoragePermission() {
    using namespace QtAndroid;

    QString permission = QStringLiteral("android.permission.WRITE_EXTERNAL_STORAGE");
    const QHash<QString, PermissionResult> results = requestPermissionsSync(QStringList({permission}));
    if (!results.contains(permission) || results[permission] == PermissionResult::Denied) {
        qWarning() << "Couldn't get permission: " << permission;
        return false;
    }

    return true;
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
