// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file SessionParameters.h
/// \brief Definition of the SessionParameters and related functions.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_INC_SESSIONPARAMETERS_H
#define O2_LLA_INC_SESSIONPARAMETERS_H

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace o2
{
namespace lla
{

struct ParametersPimpl;

class SessionParameters
{
 public:
  SessionParameters();
  ~SessionParameters();
  SessionParameters(const SessionParameters& other);
  SessionParameters(SessionParameters&& other);
  SessionParameters& operator=(const SessionParameters(&other));
  SessionParameters& operator=(SessionParameters&& other);

  // Types for parameter values
  using SessionNameType = std::string;
  using CardIdType = int;

  // Setters
  auto setSessionName(SessionNameType value) -> SessionParameters&;
  auto setCardId(CardIdType value) -> SessionParameters&;

  // Optional Getters
  auto getSessionName() const -> boost::optional<SessionNameType>;
  auto getCardId() const -> boost::optional<CardIdType>;

  // Throwing Getters
  auto getSessionNameRequired() const -> SessionNameType;
  auto getCardIdRequired() const -> CardIdType;

  static SessionParameters makeParameters()
  {
    return SessionParameters();
  }

 private:
  std::unique_ptr<ParametersPimpl> mPimpl;
};

} // namespace lla
} // namespace o2

#endif
