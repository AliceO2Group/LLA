// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file LockParameters.h
/// \brief Defintions of the LLA LockParameters and associated functions.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_INC_LOCKPARAMETERS_H
#define O2_LLA_INC_LOCKPARAMETERS_H

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "Lla/ParameterTypes/LockType.h"

namespace o2
{
namespace lla
{

struct ParametersPimpl;

class LockParameters
{
 public:
  LockParameters();
  ~LockParameters();
  LockParameters(const LockParameters& other);
  LockParameters(LockParameters&& other);
  LockParameters& operator=(const LockParameters(&other));
  LockParameters& operator=(LockParameters&& other);

  // Types for parameter values
  using LockTypeType = LockType::Type;
  using LockNameType = std::string;

  // Setters
  auto setLockType(LockTypeType value) -> LockParameters&;
  auto setLockName(LockNameType value) -> LockParameters&;

  // Optional Getters
  auto getLockType() const -> boost::optional<LockTypeType>;
  auto getLockName() const -> boost::optional<LockNameType>;

  // Throwing Getters
  auto getLockTypeRequired() const -> LockTypeType;
  auto getLockNameRequired() const -> LockNameType;

  static LockParameters makeParameters(LockTypeType lockType)
  {
    return LockParameters().setLockType(lockType);
  }

  static LockParameters makeParameters()
  {
    return LockParameters().setLockType(LockType::Type::SocketLock);
  }

 private:
  std::unique_ptr<ParametersPimpl> mPimpl;
};

} // namespace lla
} // namespace o2

#endif
