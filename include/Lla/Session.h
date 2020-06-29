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
  Session(SessionParameters& params, LockType::Type lockType);
#endif
  Session(SessionParameters& params);
  ~Session();

  bool start();
  bool timedStart(int timeOut);
  void stop();
  bool isStarted();

 private:
  void checkAndSetParameters();
  void makeLockName();

  std::string mSessionName;
  int mCardId;
  std::unique_ptr<InterprocessLockInterface> mLock;
  SessionParameters mParams;
  LockParameters mLockParams;
  bool mIsStarted = false;
};

} // namespace lla
} // namespace o2

#endif // O2_LLA_INC_SESSION_H
