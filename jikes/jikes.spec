Summary: java source to bytecode compiler
%define version 1.13
Copyright: IBM Public License, http://oss.software.ibm.com/developerworks/opensource/license10.html
Group: Development/Languages
Name: jikes
Packager: David Shields (shields@watson.ibm.com)
Prefix: /usr
Provides: jikes
Release: 1
Source: jikes-%{version}.tar.gz
URL: http://oss.software.ibm.com/developerworks/opensource/jikes
Version: %{version}
Buildroot: /tmp/jikesrpm

%description
The IBM Jikes compiler translates Java source files to bytecode. It
also supports incremental compilation and automatic makefile generation,
and is maintained by the Jikes Project:
  http://oss.software.ibm.com/developerworks/opensource/jikes

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
/usr/doc/jikes-%{version}/license.htm
/usr/man/man1/jikes.1
