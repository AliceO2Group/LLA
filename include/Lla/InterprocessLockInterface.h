// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file InterprocessLockInterface.h
/// \brief Definition of the InterprocessLockInterface class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_INC_INTERPROCESSLOCKINTERFACE_H
#define O2_LLA_INC_INTERPROCESSLOCKINTERFACE_H

namespace o2
{
namespace lla
{

class InterprocessLockInterface
{

 public:
  virtual ~InterprocessLockInterface(){};

  // have to be implemented by derived classes
  virtual void lock() = 0;
  virtual bool tryLock() = 0;
  virtual bool timedLock(int timeOut) = 0;
  virtual void unlock() = 0;
};

} // namespace lla
} // namespace o2

#endif // O2_LLA_SRC_INTERPROCESSLOCKINTERFACE_H
