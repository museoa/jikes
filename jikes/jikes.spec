Summary: java source to bytecode compiler
%define version 1.12
Copyright: IBM Public License, http://oss.software.ibm.com/developerworks/opensource/license10.html
Group: Development/Languages
Name: jikes
Packager: David Shields (shields@watson.ibm.com)
Prefix: /usr
Provides: jikes
Release: 1
Source: jikes-%{version}.tar.gz
URL: http://oss.software.ibm.com/developerworks/opensource/jikes/project/
Version: %{version}
Buildroot: /tmp/jikesrpm

%description
The IBM Jikes compiler translates Java source files to bytecode. It
also supports incremental compilation and automatic makefile generation,
and is maintained by the Jikes Project:
  http://oss.software.ibm.com/developerworks/opensource/jikes/project/

%prep
%setup -q

%build
./configure CXXFLAGS=-O3 --prefix=$RPM_BUILD_ROOT/usr
make

%install
rm -fr $RPM_BUILD_ROOT
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,-,root)
/usr/bin/jikes
/usr/doc/jikes-%{version}/contrib.html
/usr/doc/jikes-%{version}/jikes.gif
/usr/doc/jikes-%{version}/jikes.html
/usr/doc/jikes-%{version}/license.htm
/usr/doc/jikes-%{version}/news.html
/usr/man/man1/jikes.1
