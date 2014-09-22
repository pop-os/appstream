AppStream
=========

AppStream is a cross-distro effort for enhancing the way we interact with the software repositories provided by the
distribution by standardizing sets of additional metadata.
AppStream provides the foundation to build software-center applications. It additionally provides specifications
for things like an unified software metadata database, screenshot services and various other things needed to create
user-friendly application-centers for (Linux) distributions.

This repository contains the AppStream specification and a library for accessing the Xapian database which has been
generated from AppStream metadata.

![AppStream Architecture](docs/sources/images/architecture-small.png "AppStream Architecture")

## Useful Links
[AppStream Documentation](http://www.freedesktop.org/software/appstream/docs/) - The AppStream specification and help  
[Releases](http://www.freedesktop.org/software/appstream/releases/) - All releases of AppStream  
[AppStream on Freedesktop](http://www.freedesktop.org/wiki/Distributions/AppStream/) - The original Freedesktop.org page  

## Developers
[![Build Status](https://travis-ci.org/ximion/appstream.svg?branch=master)](https://travis-ci.org/ximion/appstream)

### Dependencies

#### Core
 * glib2 (>= 2.36)
 * GObject-Introspection
 * libxml2
 * Xapian

#### Optional
 * Vala Compiler (vapigen) (for Vala API file)
 * libyaml (for DEP-11 support)

#### Documentation
 * Publican

#### Qt (for libappstream-qt)
 * Qt4 or Qt5 Core


### Compiling instructions

To compile AppStream, make sure that you have all required libraries (development files!) installed.
Then continue. (the build system will complain about missing dependencies)

Use CMake to configure AppStream and build it with make:
```bash
mkdir build
cd build
cmake <flags> ..
make
```
Possible AppStream-specific flags are:  
 -DPROVIDER_DEBIAN_DEP11=ON      -- Enable support for Debian DEP11 AppStream format  
 -DPROVIDER_UBUNTU_APPINSTALL=ON -- Enable support for Ubuntu's AppInstall data  
 -DDOCUMENTATION=ON              -- (Re)generate API documentation  
 -DMAINTAINER=ON                 -- Enable strict compiler options - use this if you write a patch for AppStream

### Installation

To install the compiled binaries and required data files, execute
"make install" with superuser rights.
