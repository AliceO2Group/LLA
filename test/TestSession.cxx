// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file TestSession.cxx
/// \brief Tests for the Session of the LLA library.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#define BOOST_TEST_MODULE LLA_TestSession
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>
#include <chrono>
#include <thread>

#include <Lla/Exception.h>
#include <Lla/Session.h>

using namespace o2::lla;

BOOST_AUTO_TEST_SUITE(LowLevelArbitrationSession)

BOOST_AUTO_TEST_CASE(SessionEasyCreate)
{
  SessionParameters params = SessionParameters::makeParameters("KSA", "#3");
  Session session = Session(params);
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SessionCreate)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("KSA")
                               .setCardId(std::string("#3"));
  Session session = Session(params);
  if (session.start()) {
    session.stop();
  } else {
    BOOST_CHECK(false);
  }

  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SessionCreateFail)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("WILLFAIL");
  BOOST_CHECK_THROW(Session session = Session(params), ParameterException);
}

BOOST_AUTO_TEST_CASE(SessionCreateFailID)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("WILLFAIL")
                               .setCardId(std::string("#8"));
  BOOST_CHECK_THROW(Session session = Session(params), ParameterException);
}

BOOST_AUTO_TEST_CASE(TimedSessionCreate)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("WILLSUCCEED")
                               .setCardId(std::string("#2"));
  Session session = Session(params);
  if (!session.timedStart(10)) {
    BOOST_CHECK(false);
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TimedSessionsCreate)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("KSA")
                               .setCardId(std::string("#3"));
  Session sessionA = Session(params);
  params.setCardId(std::string("#2"));
  Session sessionB = Session(params);
  if (!sessionA.timedStart(10)) {
    BOOST_CHECK(false);
  }
  if (!sessionB.timedStart(10)) {
    BOOST_CHECK(false);
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TimedSessionsCreateLocked)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("KSA")
                               .setCardId(std::string("#3"));
  Session sessionA = Session(params);
  Session sessionB = Session(params);
  if (!sessionA.timedStart(10)) {
    BOOST_CHECK(false);
  }
  if (!sessionB.timedStart(10)) {
    BOOST_CHECK(true);
  } else {
    BOOST_CHECK(false);
  }
}

BOOST_AUTO_TEST_CASE(SessionsCreate)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("KSA")
                               .setCardId(std::string("#3"));
  Session sessionA = Session(params);
  params.setCardId(std::string("#2"));
  Session sessionB = Session(params);
  if (!sessionA.start()) {
    BOOST_CHECK(false);
  }
  if (!sessionB.start()) {
    BOOST_CHECK(false);
  }
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(SessionsCreateLocked)
{
  SessionParameters params = SessionParameters::makeParameters()
                               .setSessionName("KSA")
                               .setCardId(std::string("#3"));
  Session sessionA = Session(params);
  Session sessionB = Session(params);
  if (!sessionA.start()) {
    BOOST_CHECK(false);
  }
  if (!sessionB.start()) {
    BOOST_CHECK(true);
  } else {
    BOOST_CHECK(false);
  }
}

BOOST_AUTO_TEST_SUITE_END()
