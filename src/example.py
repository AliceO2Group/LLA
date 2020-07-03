#!/usr/bin/env python

import libO2Lla
import time

session = libO2Lla.Session("PythonSession", "af:00.0")
ok = session.start()
if (ok):
  time.sleep(4) # critical section
  session.stop()
else:
  print("Couldn't start session, exclusive access granted elsewhere")

session = libO2Lla.Session("PythonSession", "#2")
ok = session.timed_start(1000) # try to start for 1000ms
if (ok):
  time.sleep(4) # critical section
  session.stop()
else:
  print("Couldn't start session, exclusive access granted elsewhere")
