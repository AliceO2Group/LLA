// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file SocketLock.h
/// \brief Definition of the SocketLock class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_SRC_SOCKETLOCK_H
#define O2_LLA_SRC_SOCKETLOCK_H

#include <sys/socket.h>
#include <sys/un.h>

#include "InterprocessLockBase.h"

//#define LOCK_TIMEOUT 5
#define UNIX_SOCK_NAME_LENGTH 104 //108 for most UNIXs, 104 for macOS

namespace o2
{
namespace lla
{

class SocketLock : public InterprocessLockBase
{
 public:
  SocketLock(const LockParameters& params);
  virtual ~SocketLock() override;
  virtual void lock() override;
  virtual bool tryLock() override;
  virtual bool timedLock(int timeOut) override;
  virtual void unlock() override;

 private:
  std::string hashSocketLockName();
  unsigned long hashDjb2(const char* str, size_t length);

  int mSocketFd;
  std::string mSafeSocketLockName;
  bool mLocked = false;
  struct sockaddr_un mServerAddress;
  socklen_t mAddressLength = sizeof(struct sockaddr_un);
};

} // namespace lla
} // namespace o2

#endif // O2_LLA_SRC_SOCKETLOCK_H
