
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

/// \file InterprocessLockFactory.cxx
/// \brief Implementation of the InterprocessLockFactory class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <iostream>
#include <memory>

#include "Lla/Exception.h"
#include "InterprocessLockFactory.h"
#include "NamedMutex.h"
#include "SocketLock.h"

namespace o2
{
namespace lla
{

std::unique_ptr<InterprocessLockInterface> InterprocessLockFactory::getInterprocessLock(const LockParameters& params)
{
  if (params.getLockTypeRequired() == LockType::Type::SocketLock) {
    return std::make_unique<SocketLock>(params);
  } else if (params.getLockTypeRequired() == LockType::Type::NamedMutex) {
    return std::make_unique<NamedMutex>(params);
  } else {
    BOOST_THROW_EXCEPTION(LlaException() << ErrorInfo::Message("Unknown LockType provided"));
  }
}

} // namespace lla
} // namespace o2
