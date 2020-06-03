// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file SocketLock.cxx
/// \brief Implementation of the SocketLock class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <iostream>
#include <chrono>
#include <fcntl.h>
#include <boost/throw_exception.hpp>

#include "Lla/Exception.h"
#include "Logger.h"
#include "SocketLock.h"

namespace o2
{
namespace lla
{

SocketLock::SocketLock(const LockParameters& params)
  : InterprocessLockBase(params)
{
  //  getLogger() << __func__ << " : " << mLockName << endm;

  memset(&mServerAddress, 0, sizeof(mServerAddress));
  mServerAddress.sun_family = AF_UNIX;

  // Care in case the filename is longer than the unix socket name length
  mSafeSocketLockName = hashSocketLockName();
  strcpy(mServerAddress.sun_path, mSafeSocketLockName.c_str());
  mServerAddress.sun_path[0] = 0; //this makes the unix domain socket *abstract*
}

SocketLock::~SocketLock()
{
  //  getLogger() << __func__ << endm;
  unlock();
}

void SocketLock::lock()
{
  //  getLogger() << __func__ << endm;

  if (!mLocked) {
    if ((mSocketFd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) { // TODO: Don't reopen if already open?
      BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("Couldn't create abstract socket fd for InterprocessLock"));
    }

    if (bind(mSocketFd,
             (const struct sockaddr*)&mServerAddress,
             mAddressLength) < 0) {
      close(mSocketFd);
      BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("Couldn't bind to socket " + mSafeSocketLockName));
    }

    mLocked = true;
  } else {
    BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("Already locked " + mSafeSocketLockName));
  }
}

bool SocketLock::tryLock()
{
  try {
    lock();
    return true;
  } catch (const LlaException& e) {
    //    getWarningLogger() << boost::diagnostic_information(e) << endm;
  }
  return false;
}

bool SocketLock::timedLock(int timeOut)
{
  const auto start = std::chrono::steady_clock::now();
  auto timeExceeded = [&]() { return ((std::chrono::steady_clock::now() - start) > std::chrono::milliseconds(timeOut)); };

  while (!timeExceeded()) {
    if (tryLock()) {
      return true;
    }
  }

  return false;
}

void SocketLock::unlock()
{
  //  getLogger() << __func__ << endm;
  if (mLocked && fcntl(mSocketFd, F_GETFD) != -1) { // Close the socket fd in case it is still open
    close(mSocketFd);
    mLocked = false;
  }
}

std::string SocketLock::hashSocketLockName()
{
  if (mLockName.length() >= UNIX_SOCK_NAME_LENGTH) { // TODO: FRED-DDT LLA never uses this part, could be used for RoC as well
    std::string lockType = mLockName.substr(0, 17);  // isolate the class that created the lock

    unsigned long lockNameHash = hashDjb2(mLockName.c_str(), mLockName.size());   // hash the mutable part
    std::string safeLockName = lockType + std::to_string(lockNameHash) + "_lock"; // return conformant name
    return safeLockName;
  } else {
    return mLockName;
  }
}

unsigned long SocketLock::hashDjb2(const char* str, size_t length)
{
  unsigned long hash = 5381;
  for (size_t i = 0; i < length; ++i)
    hash = 33 * hash + (unsigned char)str[i];
  return hash;
}

} // namespace lla
} // namespace o2
