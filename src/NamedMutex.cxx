
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

/// \file NamedMutex.cxx
/// \brief Implementation of the NamedMutex class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <iostream>
#include <chrono>
#include <fcntl.h>
#include <boost/throw_exception.hpp>

#include "Lla/Exception.h"
#include "NamedMutex.h"

namespace o2
{
namespace lla
{

NamedMutex::NamedMutex(const LockParameters& params)
  : InterprocessLockBase(params),
    mMutex(bip::open_or_create, mLockName.c_str())
{
}

NamedMutex::~NamedMutex()
{
  unlock();
}

void NamedMutex::lock()
{
  if (!mLocked) {
    try {
      if (mMutex.try_lock()) {
        mLocked = true;
      } else {
        BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("whoops"));
      }
    } catch (bip::interprocess_exception& e) {
      BOOST_THROW_EXCEPTION(e);
    }
  }
}

bool NamedMutex::tryLock()
{
  try {
    lock();
    return true;
  } catch (const LlaException& e) {
  }
  return false;
}

bool NamedMutex::timedLock(int timeOut)
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

void NamedMutex::unlock()
{
  if (mLocked) {
    mMutex.unlock();
    mLocked = false;
  }
}

} // namespace lla
} // namespace o2
