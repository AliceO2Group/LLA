
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

/// \file NamedMutex.h
/// \brief Definition of the NamedMutex class
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file
/// \brief
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_SRC_NAMEDMUTEX_H
#define O2_LLA_SRC_NAMEDMUTEX_H

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "InterprocessLockBase.h"

namespace bip = boost::interprocess;

namespace o2
{
namespace lla
{

class NamedMutex : public InterprocessLockBase
{
 public:
  NamedMutex(const LockParameters& params);
  virtual ~NamedMutex() override;
  virtual void lock() override;
  virtual bool tryLock() override;
  virtual bool timedLock(int timeOut) override;
  virtual void unlock() override;

 private:
  bool mLocked = false;
  bip::named_mutex mMutex;
};

} // namespace lla
} // namespace o2

#endif // O2_LLA_SRC_NAMEDMUTEX_H
