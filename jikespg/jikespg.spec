Name: jikespg
Version: 1.3
Release: 1mdk
Source: jikespg-%{version}.tar.gz
License: IBM Public License 1.0
URL: http://www.ibm.com/research/jikes
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Group: Development/Other
Summary: The Jikes Parser Generator
%description
Jikes Parser Generator is the parser generator used by the Jikes Compiler. It
takes as input an annotated version of the Java language grammar and produces
as output program source and data that is part of the source for the Jikes
compiler.                            
                              
The current version is written in C and reflects its ancient origins; in
particular, the parser generator was written at a time when memory was at a
premium (work on it began over fifteen years ago), and was designed to be able
to process a grammar for Ada in less that 300K running on an original model
IBM PC.
%prep
%setup -q
%build
%configure
make
%install
make DESTDIR=${RPM_BUILD_ROOT} install
%clean
if test "/" != "${RPM_BUILD_ROOT}"; then rm -rf ${RPM_BUILD_ROOT}; fi
%files
%defattr(-,root,root)
%doc ChangeLog jikespg.htm license.htm README.TXT examples
%{_bindir}
