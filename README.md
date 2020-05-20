Simple QML go/c++ project
=========================

This project aims at proving a minimal feasible toolchain to build a Qt/Qml c++
application that is a light wrapper around the functionality exposed by a go
library.

Ease of cross-compilation is a must.


Building blocks
---------------

* The entrypoint of the application lives in src/main.cpp. Here we instantiate
  a QApplication that loads the main Qml (in src/main.qml).
* To demostrate the use of signals, we expose bi-direccional functionality with
  signals and slots that are connected to functions exported by the Go library.
  Handlers are declared in src/handletextfield.h. Here we import the Go wrapper
  library.

Cross-compilation
-----------------

Cross-compilation is done using mxe. You should have the mxe cloned and have
built the toolchain in the parent folder. Then you can build for windows like
this:

```
XBUILD=win64 ./build.sh
```

You should see the binary in `release/MinimalQml.exe`.

TODO
------------------

* [ ] provide Vagrantfile (or Dockerfile) with mxe toolchain pre-installed.
* [ ] document quirks on complation of the toolchain (uh... maybe give another try to debian packages!)


