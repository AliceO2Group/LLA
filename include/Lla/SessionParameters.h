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

#include <ReadoutCard/CardFinder.h>
#include <ReadoutCard/Parameters.h>

namespace roc = AliceO2::roc;

namespace o2
{
namespace lla
{

class CardIdVisitor : public boost::static_visitor<int>
{
 public:
  int operator()(const char* s) const { return roc::findCard(std::string(s)).sequenceId; };
  int operator()(std::string s) const { return roc::findCard(s).sequenceId; };
  int operator()(roc::Parameters::CardIdType cardId) const { return roc::findCard(cardId).sequenceId; };
};

struct ParametersPimpl;

/// Class holding Session Parameters
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

  /// Type for the Session Name
  using SessionNameType = std::string;

  /// Type for the CardId
  using CardIdType = boost::variant<const char*, std::string, roc::Parameters::CardIdType>;

  // Setters

  /// Sets the SessionName parameter
  ///
  /// Required parameter.
  ///
  /// \param value The value to set
  /// \return Reference to this object for chaining calls
  auto setSessionName(SessionNameType value) -> SessionParameters&;

  /// Sets the CardId parameter
  ///
  /// Required parameter.
  ///
  /// \param value The value to set
  /// \return Reference to this object for chaining calls

  auto setCardId(CardIdType value) -> SessionParameters&;

  // Optional Getters

  /// Gets the SessionName parameter
  /// \return The value wrapped in optional if it is present, or empty optional otherwise
  auto getSessionName() const -> boost::optional<SessionNameType>;

  /// Gets the CardId parameter
  /// \return The value wrapped in optional if it is present, or empty optional otherwise
  auto getCardId() const -> boost::optional<CardIdType>;

  // Throwing Getters

  /// Gets the SessionName parameter
  /// \return The value
  /// \throws o2::lla::ParameterException if not present
  auto getSessionNameRequired() const -> SessionNameType;

  /// Gets the SessionName parameter
  /// \return The value
  /// \throws o2::lla::ParameterException if not present
  auto getCardIdRequired() const -> CardIdType;

  /// Convenience function to make a SessionParameters object
  /// \return The newly created SessionParameters object
  static SessionParameters makeParameters()
  {
    return SessionParameters();
  }

  /// Convenience function to make a SessionParameters object with the required SessionName and CardId parameters
  /// \param sessionName The SessionName parameter
  /// \param cardId The CardId parameter
  /// \return The newly created SessionParameters object
  static SessionParameters makeParameters(SessionNameType sessionName, CardIdType cardId)
  {
    return SessionParameters()
      .setSessionName(sessionName)
      .setCardId(cardId);
  }

 private:
  std::unique_ptr<ParametersPimpl> mPimpl;

  static void disableInfoLogger();
};

} // namespace lla
} // namespace o2

#endif
