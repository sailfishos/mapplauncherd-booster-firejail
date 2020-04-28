/***************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
** All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "booster-firejail.h"
#include "launcherlib.h"
#include "daemon.h"
#include "logger.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <gmodule.h>

const string FirejailBooster::m_boosterType  = "firejail";
#define FIREJAIL_EXEC "/usr/bin/firejail"

const string & FirejailBooster::boosterType() const
{
    return m_boosterType;
}

bool FirejailBooster::preload()
{
    return true;
}

void addArg(GArray *arr, const char *arg) {
    int i = 0;
    while (char *elem = g_array_index(arr, char *, i++)) {
        if (!strcmp(arg, elem)) { // duplicate
            return;
        }
    }
	char *val = strdup(arg);
	g_array_append_val(arr, val);
	Logger::logDebug("Arg: %s", val);
}

int FirejailBooster::launchProcess()
{
    GArray *fullargv;
    Booster::setEnvironmentBeforeLaunch();

    fullargv = g_array_new(TRUE, FALSE, sizeof(char *));

    // argv[0] = firejail
    addArg(fullargv, FIREJAIL_EXEC);
    gchar *cmdname = g_strrstr(appData()->fileName().c_str(), "/")+1;

    // Check for named profile
    gchar *profile_path = g_strdup_printf("/etc/firejail/%s.profile", cmdname);
    if ( access( profile_path, F_OK) != -1) {
        gchar *profile = g_strdup_printf("--profile=%s", cmdname);
        addArg(fullargv, profile);
        g_free(profile);
    }
    else { // Or use the default base
        addArg(fullargv, "--profile=sailfish-app");
    }
    g_free(profile_path);

    // TODO: link this to the daemon --debug mode
    addArg(fullargv, "--debug");

    // Privileges
/*
 *  Valid permission categories are:
     a = accounts/signon data
     c = calendar data
     d = documents & downloads
     e = email data
     h = communication history data
     i = image data
     l = location/gps data
     m = messaging data
     n = events/notifications data
     p = people/contacts data
     r = rpm packages data
     s = settings data
     u = multimedia/music/video data
     w = network/internet
 */

    if (appData()->privileges().find('a') != std::string::npos) { // Accounts/signon
        addArg(fullargv, "--whitelist=~/.config/libaccounts-glib");
        addArg(fullargv, "--whitelist=~/.config/signond");
        addArg(fullargv, "--dbus-user.talk=org.freedesktop.Telepathy.AccountManager");
        addArg(fullargv, "--dbus-user.talk=org.freedesktop.DevicePresence");
    }
    if (appData()->privileges().find('c') != std::string::npos) { // Calendar
        addArg(fullargv, "--whitelist=~/.local/share/system/privileged/Calendar");
        addArg(fullargv, "--dbus-system.talk=com.nokia.time");
    }
    if (appData()->privileges().find('d') != std::string::npos) { // Documents & Downloads
        addArg(fullargv, "--whitelist=~/Documents");
        addArg(fullargv, "--whitelist=~/Downloads");
        addArg(fullargv, "--dbus-user.talk=org.freedesktop.Tracker1");
    }
    if (appData()->privileges().find('e') != std::string::npos) { // Email data
        addArg(fullargv, "--whitelist=~/.qmf");
    }
    if (appData()->privileges().find('i') != std::string::npos || appData()->privileges().find('u') != std::string::npos) { // Tracker
        addArg(fullargv, "--whitelist=~/.cache/tracker");
        addArg(fullargv, "--dbus-user.talk=org.freedesktop.Tracker1");
        gchar *mpris = g_strdup_printf("--dbus-user.own=org.mpris.MediaPlayer2.%s", cmdname);
        addArg(fullargv, mpris);
        g_free(mpris);
        addArg(fullargv, "--whitelist=~/.cache/org.nemomobile/thumbnails");
        addArg(fullargv, "--whitelist=~/.cache/media-art");
    }
    if (appData()->privileges().find('i') != std::string::npos) { // Images
        addArg(fullargv, "--whitelist=~/Pictures");
    }
    if (appData()->privileges().find('l') != std::string::npos) { // Notifications
        addArg(fullargv, "--dbus-user.talk=org.freedesktop.Geoclue");
    }

    if (appData()->privileges().find('m') != std::string::npos || appData()->privileges().find('h') != std::string::npos) { // Messaging/ Comm History
        addArg(fullargv, "--whitelist=~/.local/share/commhistory");
        addArg(fullargv, "--dbus-user.talk=org.nemomobile.CommHistory");
    }
    if (appData()->privileges().find('n') != std::string::npos) { // Notifications
        addArg(fullargv, "--dbus-user.talk=org.freedesktop.Notifications");
    }
    if (appData()->privileges().find('p') != std::string::npos) { // People
        addArg(fullargv, "--whitelist=~/.local/share/system/Contacts");
        addArg(fullargv, "--whitelist=~/.local/share/system/privileged/Contacts");
    }
    if (appData()->privileges().find('r') != std::string::npos) { // RPM/Packaging
        addArg(fullargv, "--dbus-system.talk=org.freedesktop.PackageKit");
        addArg(fullargv, "--dbus-system.talk=org.freedesktop.PackageKit.*");
        addArg(fullargv, "--dbus-session.talk=org.sailfishos.installationhandler");
    }
    if (appData()->privileges().find('u') != std::string::npos) { // Multimedia
        addArg(fullargv, "--whitelist=~/Videos");
        addArg(fullargv, "--whitelist=~/Music");
        addArg(fullargv, "--whitelist=~/.config/pulse");
        addArg(fullargv, "--whitelist=~/.cache/gstreamer-1.0");
    }
    else {
        addArg(fullargv, "--nosound");
    }
    if (appData()->privileges().find('w') != std::string::npos) { // No network
        addArg(fullargv, "--protocol=unix,netlink,inet,inet6");
    }
    else {
        addArg(fullargv, "--protocol=unix,netlink");
        addArg(fullargv, "--net=none");
    }

    // Data dirs
    gchar *whitelist = g_strdup_printf("--whitelist=~/.local/share/%s", cmdname);
    addArg(fullargv, whitelist);
    g_free(whitelist);
    whitelist = g_strdup_printf("--whitelist=/usr/share/%s", cmdname);
    addArg(fullargv, whitelist);
    g_free(whitelist);

    // Only show own binary
    gchar *bin = g_strdup_printf("--private-bin=%s", cmdname);
    addArg(fullargv, bin);
    g_free(bin);

    // Finish firejail options
    addArg(fullargv, "--");

    // Command to launch
    addArg(fullargv, appData()->fileName().c_str());

    // Copy passed params
    const int argc = appData()->argc();
    for (int i = 1; i < argc; i++)
        addArg(fullargv, appData()->argv()[i]);

    // Exec the binary (execv returns only in case of an error).
    char **argv = (char**)g_array_free(fullargv, FALSE);

    execv(FIREJAIL_EXEC, argv);

    // Delete dummy argv if execv failed
    for (int i = 0; argv[i]; i++)
        free(argv[i]);

    delete [] argv;


    return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
    FirejailBooster *booster = new FirejailBooster;

    Daemon d(argc, argv);
    d.run(booster);
}

