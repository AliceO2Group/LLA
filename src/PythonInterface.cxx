// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file PythonInterface.cxx
/// \brief Python interface for session control
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <iostream>
#include <boost/python.hpp>

#include "Lla/Lla.h"

namespace
{

// Documentation for the init function
auto sSessionDocString =
  R"(Initializes a Session object

Args:
  session name: The name of the session
  card id: The sequence number of the card to start a session for)";

auto sStartDocString =
  R"(Starts a session, guaranteeing exclusive access to the Slow Control interface of the specified card

Args:
  None
Returns:
  True if started succesfully, False otherwise)";

auto sTimedStartDocString =
  R"(Tries to start a session over the specified period of time

Args:
  time out: Time (in ms) within which to keep trying to start a session
Returns:
  True if started succesfully, False otherwise)";

auto sStopDocString =
  R"(Stops a session, releasing exclusive access

Args:
  None
Returns:
  None)";

namespace lla = o2::lla;

class Session
{
 public:
  Session(std::string sessionName, int cardId)
  {
    lla::SessionParameters params = lla::SessionParameters::makeParameters()
                                      .setSessionName(sessionName)
                                      .setCardId(cardId);
    mSession = std::make_shared<lla::Session>(params);
  }

  bool start()
  {
    return mSession->start();
  }

  bool timedStart(int timeOut)
  {
    return mSession->timedStart(timeOut);
  }

  void stop()
  {
    mSession->stop();
  }

 private:
  std::shared_ptr<lla::Session> mSession;
};
} // Anonymous namespace

BOOST_PYTHON_MODULE(libO2Lla)
{
  using namespace boost::python;

  class_<Session>("Session", init<std::string, int>(sSessionDocString))
    .def("start", &Session::start, sStartDocString)
    .def("timed_start", &Session::timedStart, sTimedStartDocString)
    .def("stop", &Session::stop, sStopDocString);
}
