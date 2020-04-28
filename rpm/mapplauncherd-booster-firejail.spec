Name:       mapplauncherd-booster-firejail
Summary:    Firejail application launcher
Version:    0.0.1
Release:    1
License:    LGPLv2.1
URL:        https://github.org/jolla/mapplauncherd-booster-firejail
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig
BuildRequires:  mapplauncherd-devel >= 4.1.0
BuildRequires:  qt5-qmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(gmodule-2.0)
BuildRequires:  pkgconfig(gobject-2.0)
Requires:  mapplauncherd >= 4.1.0
Requires:  systemd-user-session-targets
Requires:  firejail
Requires:  xdg-dbus-proxy

%{!?qtc_make:%define qtc_make make}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}

%description
Proof-of-concept booster implementation for launching apps in a Firejail
sandbox with optional privileges.

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%qtc_qmake5

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake_install

mkdir -p %{buildroot}/usr/lib/systemd/user/user-session.target.wants || true
ln -s ../booster-firejail.service %{buildroot}/usr/lib/systemd/user/user-session.target.wants/

%files
%defattr(-,root,root,-)
%attr(2755, root, privileged) %{_libexecdir}/mapplauncherd/booster-firejail
%{_sysconfdir}/firejail/sailfish-app.profile
%{_libdir}/systemd/user/booster-firejail.service
%{_libdir}/systemd/user/user-session.target.wants/booster-firejail.service

