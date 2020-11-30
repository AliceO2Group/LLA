// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file InterprocessLockBase.cxx
/// \brief Implementation of the InterprocessLockBase class.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <chrono>
#include <thread>
//#include "InfoLogger/InfoLogger.hxx"

#include "InterprocessLockBase.h"
//#include "Logger.h"
#include "Lla/Exception.h"

namespace o2
{
namespace lla
{

//AliceO2::InfoLogger::InfoLogger logger; //TODO: Could be extern

InterprocessLockBase::InterprocessLockBase(const LockParameters& params)
  : mLockName(params.getLockName().get_value_or("dummy"))
{
}

InterprocessLockBase::~InterprocessLockBase()
{
}

} // namespace lla
} // namespace o2
