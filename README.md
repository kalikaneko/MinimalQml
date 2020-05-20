A minimal QML c++/go application
================================

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

* [ ] explore how to pass structs around
* [ ] way to pass a callback from c code to Go (so that we can emit signals instead of polling)
* [ ] demonstrate persisting object in go (spawn a goroutine that inits and hold it, use channels etc)
* [ ] provide Vagrantfile (or Dockerfile) with mxe toolchain pre-installed.
* [ ] document quirks on complation of the toolchain (uh... maybe give another try to debian packages, now that I got the go cross-compilation right!)
* [ ] explore qtquick deployment "the qt way" 
  * [ ] https://doc.qt.io/qt-5/qtquick-deployment.html
  * [ ] windeployqt? if this works for UWP, that'd be super nice https://doc.qt.io/qt-5/windows-deployment.html *"The Windows deployment tool windeployqt is designed to automate the process of creating a deployable folder containing the Qt-related dependencies (libraries, QML imports, plugins, and translations) required to run the application from that folder. It creates a sandbox for Universal Windows Platform (UWP) or an installation tree for Windows desktop applications, which can be easily bundled into an installation package."*



useful references
-----------------
* https://github.com/draffensperger/go-interlang
* https://eli.thegreenplace.net/2019/passing-callbacks-and-pointers-to-cgo/
* https://github.com/golang/go/wiki/cgo
* https://dev.to/mattn/call-go-function-from-c-function-1n3
