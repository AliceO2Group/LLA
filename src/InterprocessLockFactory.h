
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

/// \file InterprocessLockFactory.h
/// \brief Defintion of the InterprocessLockFactory class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_SRC_INTERPROCESSLOCKFACTORY_H
#define O2_LLA_SRC_INTERPROCESSLOCKFACTORY_H

#include <memory>

#include <Lla/InterprocessLockInterface.h>
#include <Lla/LockParameters.h>

namespace o2
{
namespace lla
{

class InterprocessLockFactory
{
 public:
  static std::unique_ptr<InterprocessLockInterface> getInterprocessLock(const LockParameters& params);
};

} // namespace lla
} // namespace o2

#endif // O2_LLA_INC_INTERPROCESSLOCKFACTORY_H
