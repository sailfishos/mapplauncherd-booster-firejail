# Default Firejail profile for Sailfish apps

include disable-common.inc
include disable-devel.inc
include disable-exec.inc

caps.drop all
nonewprivs
seccomp
netfilter

### home directory whitelisting
whitelist ~/.cache/ambienced
whitelist ~/.config/dconf

## data dir whitelisting
whitelist /usr/share/themes
whitelist /usr/share/translations
whitelist /usr/share/fonts
whitelist /usr/share/fontconfig
whitelist /usr/share/pixmaps
whitelist /usr/share/icons
whitelist /usr/share/qt5
whitelist /usr/share/glib-2.0/schemas
whitelist /usr/share/X11/xkb
include /etc/firejail/whitelist-common.inc

private-etc machine-id,dconf,xdg,fonts,selinux

#dbus
dbus-system filter
dbus-system.talk com.nokia.NonGraphicFeedback1.Backend
dbus-user filter
dbus-user.talk ca.desrt.dconf
dbus-user.talk com.nokia.profiled
dbus-user.talk org.malitt.server
dbus-user.talk org.nemo.transferengine

### environment
shell none

disable-mnt
private-tmp
blacklist /var
