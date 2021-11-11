
// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Session.cxx
/// \brief Implementation of the Session class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include "ReadoutCard/Exception.h"

#include "Lla/Exception.h"
#include "Lla/Session.h"

#include "InterprocessLockFactory.h"

namespace o2
{
namespace lla
{

#ifdef O2_LLA_BENCH_ENABLED
#pragma message("O2_LLA_BENCH_ENABLED defined")
Session::Session(SessionParameters& params, LockType::Type lockType)
{
  mParams = SessionParameters(params);
  checkAndSetParameters();
  makeLockName();
  mLockParams.setLockType(lockType);
  mLock = InterprocessLockFactory::getInterprocessLock(mLockParams);
}
#endif

Session::Session(SessionParameters& params)
{
  mParams = SessionParameters(params);
  checkAndSetParameters();
  makeLockName();
  mLock = InterprocessLockFactory::getInterprocessLock(mLockParams);
}

Session::Session(const Session& other)
{
  mSessionName = other.mSessionName;
  mCardId = other.mCardId;
  mParams = other.mParams;
  mLockParams = other.mLockParams;
  mLock = InterprocessLockFactory::getInterprocessLock(mLockParams);
  mIsStarted = false;
}

Session::Session(Session&& other)
{
  mSessionName = other.mSessionName;
  mCardId = other.mCardId;
  mParams = other.mParams;
  mLockParams = other.mLockParams;
  mLock = std::move(other.mLock);
  mIsStarted = other.mIsStarted;
}

Session& Session::operator=(const Session& other)
{
  if (this == &other) {
    return *this;
  }

  mSessionName = other.mSessionName;
  mCardId = other.mCardId;
  mParams = other.mParams;
  mLockParams = other.mLockParams;
  mLock = InterprocessLockFactory::getInterprocessLock(mLockParams);
  mIsStarted = false;
  return *this;
}

Session& Session::operator=(Session&& other)
{
  if (this == &other) {
    return *this;
  }
  mSessionName = other.mSessionName;
  mCardId = other.mCardId;
  mParams = other.mParams;
  mLockParams = other.mLockParams;
  mLock = std::move(mLock);
  mIsStarted = other.mIsStarted;
  return *this;
}

/* Make sure that the session is stopped, so the lock is released */
Session::~Session()
{
  stop();
}

void Session::checkAndSetParameters()
{
  mSessionName = mParams.getSessionNameRequired();

  try {
    mCardId = boost::apply_visitor(CardIdVisitor(), mParams.getCardIdRequired());
  } catch (const roc::Exception& e) {
    BOOST_THROW_EXCEPTION(ParameterException() << ErrorInfo::Message(e.what()));
  }
}

bool Session::start()
{
  // In case of start when mutex is kept, immediately return
  std::unique_lock<std::mutex> ul(mMutex, std::try_to_lock);
  if (!ul.owns_lock()) { return false; }

  if (!isStarted()) {
    if (mLock->tryLock()) {
      mIsStarted = true;
      return true;
    }
    return false;
  }

  return true;
}

bool Session::timedStart(int timeOut)
{
  // In case of timed start keep trying to take the mutex and start
  const auto start = std::chrono::steady_clock::now();
  auto timeExceeded = [&]() { return ((std::chrono::steady_clock::now() - start) > std::chrono::milliseconds(timeOut)); };

  while (!timeExceeded()) {

    std::unique_lock<std::mutex> ul(mMutex, std::try_to_lock);
    if (!ul.owns_lock()) { continue; }

    if (!isStarted()) {
      if (mLock->tryLock()) {
        mIsStarted = true;
        return true;
      }
    } else {
      return true;
    }
  }

  return false;
}

void Session::stop()
{
  // In case of stop, block until mutex acquired
  std::unique_lock<std::mutex> ul(mMutex);
  
  if (isStarted()) {
    mLock->unlock();
    mIsStarted = false;
  }
}

bool Session::isStarted()
{
  return mIsStarted;
}

void Session::makeLockName()
{
  std::stringstream ss;
  ss << "_CRU_" << mCardId << "_lla_lock";
  mLockParams = LockParameters::makeParameters()
                  .setLockName(ss.str());
}

} // namespace lla
} // namespace o2
