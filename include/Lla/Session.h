// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Session.h
/// \brief Definition of the Session class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_INC_SESSION_H
#define O2_LLA_INC_SESSION_H

#include "Lla/SessionParameters.h"
#include "Lla/InterprocessLockInterface.h"
#include "Lla/LockParameters.h"

#include <memory>

namespace o2
{
namespace lla
{

class Session
{

 public:
#ifdef O2_LLA_BENCH_ENABLED
#pragma message("O2_LLA_BENCH_ENABLED defined")
  /// Session constructor only for debugging
  /// \param params The Session parameters
  /// \param lockType The type of the underlying lock implementation
  Session(SessionParameters& params, LockType::Type lockType);
#endif
  /// Session constructor
  /// \param params The Session parameters
  Session(SessionParameters& params);
  Session(const Session& other);
  Session(Session&& other);
  Session& operator=(const Session& other);
  Session& operator=(Session&& other);
  ~Session();

  /// Start a Session, within which atomic access to the card's SC interface is guaranteed
  /// \return boolean; true if successful, otherwise False
  bool start();

  /// Start a Session, trying until the timeOut has expired
  /// \param timeOut Timeout in ms after which to stop trying to start the session
  /// \return boolean; true if successful, otherwise false
  bool timedStart(int timeOut);

  /// Stops a Session, releasing atomic access to the card's SC interface
  void stop();

  /// Reports on the state of the Session
  /// \return boolean; true if started, false otherwise
  bool isStarted();

 private:
  void checkAndSetParameters();
  void makeLockName();

  std::string mSessionName;
  int mCardId;
  SessionParameters mParams;
  LockParameters mLockParams;
  std::unique_ptr<InterprocessLockInterface> mLock;
  bool mIsStarted = false;

  static void disableInfoLogger();
};

} // namespace lla
} // namespace o2

#endif // O2_LLA_INC_SESSION_H
