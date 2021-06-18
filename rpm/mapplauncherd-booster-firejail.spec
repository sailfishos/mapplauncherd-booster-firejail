Name:       mapplauncherd-booster-firejail
Summary:    Firejail application launcher
Version:    0.0.2
Release:    1
License:    LGPLv2.1
URL:        https://github.org/sailfishos/mapplauncherd-booster-firejail
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig
BuildRequires:  qt5-qmake
BuildRequires:  systemd
BuildRequires:  pkgconfig(applauncherd) >= 4.2.3
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(gmodule-2.0)
BuildRequires:  pkgconfig(gobject-2.0)
Requires:  mapplauncherd >= 4.2.3
Requires:  systemd-user-session-targets
Requires:  firejail
Requires:  xdg-dbus-proxy

%description
Proof-of-concept booster implementation for launching apps in a Firejail
sandbox with optional privileges.

%prep
%autosetup -p1 -n %{name}-%{version}

%build
%qmake5

%make_build

%install
%qmake_install

mkdir -p %{buildroot}%{_userunitdir}/user-session.target.wants || true
ln -s ../booster-firejail.service %{buildroot}%{_userunitdir}/user-session.target.wants/

%files
%defattr(-,root,root,-)
%attr(2755, root, privileged) %{_libexecdir}/mapplauncherd/booster-firejail
%{_sysconfdir}/firejail/sailfish-app.profile
%{_userunitdir}/booster-firejail.service
%{_userunitdir}/user-session.target.wants/booster-firejail.service

