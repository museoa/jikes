Summary: java source to bytecode compiler
%define version 1.05
Buildroot: /var/tmp/jikes-%{version}-root
Copyright: IBM Public License Version 1.0 - Jikes Compiler, http://ibm.com/developerworks/opensource/jikes/license.
Group: Development/Languages
Name: jikes
Packager: David Shields (shields@watson.ibm.com)
Prefix: /usr
Provides: jikes
Release: 1
Source: http://ibm.com/developerworks/opensource/jikes/download/jikes-%{version}_tar.gz 
URL: http://ibm.com/developerworks/opensource/jikes 
Version: %{version}


%description
The IBM Jikes compiler translates Java source files to bytecode. It
also supports incremental compilation and automatic makefile generation,
and is maintained by the Jikes Project:
  http://ibm.com/developerworks/opensource/jikes
%prep
%setup -q -n jikes
cd src

%build
cd src
./configure --prefix=$RPM_BUILD_ROOT
make

%install
rm -fr $RPM_BUILD_ROOT
cd src
make prefix=$RPM_BUILD_ROOT/usr install

%clean
rm -fr $RPM_BUILD_ROOT

%files
%defattr(-,-,root)
%doc README contrib.htm jikes.htm license.htm news.htm
%doc /usr/man/man1/jikes.1
/usr/bin/jikes
