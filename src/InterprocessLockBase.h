// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file InterprocessLockBase.h
/// \brief Definition of the InterprocessLockBase class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_SRC_INTERPROCESSLOCKBASE_h
#define O2_LLA_SRC_INTERPROCESSLOCKBASE_h

#include <thread>

#include "Lla/InterprocessLockInterface.h"
#include "Lla/LockParameters.h"

namespace o2
{
namespace lla
{

class InterprocessLockBase : public InterprocessLockInterface
{

 public:
  InterprocessLockBase(const LockParameters& params);
  ~InterprocessLockBase();

 protected:
  std::string mLockName;
};

} // namespace lla
} // namespace o2

#endif
