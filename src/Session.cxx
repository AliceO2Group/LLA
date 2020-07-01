// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
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
#include "Logger.h"

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

Session::~Session()
{
}

void Session::checkAndSetParameters()
{
  mSessionName = mParams.getSessionNameRequired();

  try {
    mCardId = boost::apply_visitor(CrdIdVisitor(), mParams.getCardIdRequired());
  } catch (const roc::Exception& e) {
    BOOST_THROW_EXCEPTION(ParameterException() << ErrorInfo::Message(e.what()));
  }
}

bool Session::start()
{
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
  if (!isStarted()) {
    if (mLock->timedLock(timeOut)) {
      mIsStarted = true;
      return true;
    }
    return false;
  }

  return true;
}

void Session::stop()
{
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
  ss << "_CRU" << mCardId << "_lla_lock";
  mLockParams = LockParameters::makeParameters()
                  .setLockName(ss.str());
}

} // namespace lla
} // namespace o2
