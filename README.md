# LLA

## Introduction
The LLA(Low-Level Arbitration) library offers an arbitration layer between DCS's [FRED](https://gitlab.cern.ch/alialfred/FREDServer) and Detector Debug Tools, allowing for "concurrent" access to the slow control interface of the CRUs.

The lower parts of the chain (ReadoutCard, CRU FW), are not aware of the existence of LLA, and continue to allow access whenever requested. The system is thus functional only when FRED/ALF and the Detector Debug Tools commonly adhere to the use of this arbitration layer for what concerns Slow Control CRU accesses. 

## Interface
In order to acquire atomic access to the Slow Control interface of the CRU, it is necessary to acquire a `Session` handle. To initialize this Session one needs to provide the `SessionParameters`, comprised of:
1) An `std-string` **Session Name** (currently a placeholder, to be used for logging, monitoring & RC tools)
2) An `std-string` **Card ID**; The ID of the card to get an atomic session for. The string may be a PCI Address (e.g. `"af:00.0"`), a Serial Number / Endpoint ID pair (e.g. `"10234:1"`), or a sequence ID (e.g. `"#2"`).

To initialize the Session Parameters, and then acquire the Session handle:
```
SessionParameters params = SessionParameters::makeParameters("example sess", "3b:00.0");
Session session = Session(params);
```

At this point, to guarantee atomic access, the session needs to be started. The following function returns `true` if atomic access is granted, and `false` otherwise:
```
bool ok = session.start();
```

Another option is to use the timed start function. This instructs the LLA to try and acquire atomic access over a period of time (in milliseconds), assuming that the resource might not be readily available.
```
bool ok = session.timedStart(100);
```

To check the status of the session object at any time:
```
bool isStarted = session.isStarted();
```

To release access, simply call the following, returning void:
```
session.stop();
```

The complete sequence to run a critical section looks like this:
```
SessionParameters params = SessionParameters::makeParameters("example sess", "3b:00.0");
Session session = Session(params);

if (session.start()) {
  std::this_thread::sleep_for(std::chrono::seconds(4)); // critical section
  session.stop();
} else {
  std::cerr << "Could not start session" << std::endl;
}
```

More information on the API can be found in the header files doxygen docs, for the [SessionParameters](include/Lla/SessionParameters.h) and the [Session](include/Lla/Session.h).

## Using LLA
### C++
To use the LLA library, the `"Lla/Lla.h"` convenience header may be used, as seen in the [example](src/example.cxx). To build against the LLA library it is necessary to load the alisw environment (`aliswmod enter LLA`) and run the following g++ command. Make sure to adjust the versions according to `aliswmod list` output, when the environment is loaded.

```
g++ -Wall \
  -I /opt/alisw/el7/LLA/v0.1.0-1/include \
  -I /opt/alisw/el7/Common-O2/v1.4.9-27/include/ \
  -I /opt/alisw/el7/ReadoutCard/v0.21.3-1/include \
  -I /opt/alisw/el7/boost/v1.72.0-alice1-36/include/ \
  -lO2Lla \
  -L /opt/alisw/el7/LLA/v0.1.0-1/lib \
  lla-example.cxx -o llae
```

### Python
Python bindings for the library are also offered, as seen in [this](src/example.py) example. To run, load the alisw environemnt (`aliswmod enter LLA`) and make sure `import libO2Lla` is present.
