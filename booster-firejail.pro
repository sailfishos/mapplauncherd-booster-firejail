TEMPLATE = app

TARGET = booster-firejail

QT -= core gui
CONFIG += link_pkgconfig
PKGCONFIG += gio-2.0 glib-2.0 gmodule-2.0 gobject-2.0 applauncherd

target.path = /usr/libexec/mapplauncherd/
qml.path = /usr/share/$${TARGET}/

service.path = /usr/lib/systemd/user/
service.files = data/$${TARGET}.service

profile.path = /etc/firejail/
profile.files = data/sailfish-app.profile

INSTALLS += target service profile

SOURCES += src/booster-firejail.cpp
HEADERS += src/booster-firejail.h

