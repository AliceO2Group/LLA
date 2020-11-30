// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file SessionParameters.cxx
/// \brief Implementation of SessionParameters and associated functions.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <boost/throw_exception.hpp>
#include <map>
#include "Lla/SessionParameters.h"
#include "Lla/Exception.h"

namespace o2
{
namespace lla
{

using Variant = boost::variant<std::string, int, SessionParameters::CardIdType>;
using KeyType = const char*;
using Map = std::map<KeyType, Variant>;

struct ParametersPimpl {
  Map map;
};

namespace
{

template <typename T>
void setParam(Map& map, KeyType key, const T& value)
{
  map[key] = Variant{ value };
}

template <typename T>
auto getParam(const Map& map, KeyType key) -> boost::optional<T>
{
  auto it = map.find(key);
  if (it != map.end()) {
    auto var = it->second;
    auto val = boost::get<T>(var);
    return val;
  } else {
    return boost::none;
  }
}

template <typename T>
auto getParamRequired(const Map& map, KeyType key) -> T
{
  if (auto optional = getParam<T>(map, key)) {
    return *optional;
  } else {
    BOOST_THROW_EXCEPTION(ParameterException()
                          << ErrorInfo::Message("Parameter was not set")
                          << ErrorInfo::Message(key));
  }
}
} // anonymous namespace

#define _PARAMETER_FUNCTIONS(_param_name, _key_string)                                  \
  auto SessionParameters::set##_param_name(_param_name##Type value)->SessionParameters& \
  {                                                                                     \
    setParam(mPimpl->map, _key_string, value);                                          \
    return *this;                                                                       \
  }                                                                                     \
                                                                                        \
  auto SessionParameters::get##_param_name() const->boost::optional<_param_name##Type>  \
  {                                                                                     \
    return getParam<_param_name##Type>(mPimpl->map, _key_string);                       \
  }                                                                                     \
                                                                                        \
  auto SessionParameters::get##_param_name##Required() const->_param_name##Type         \
  {                                                                                     \
    return getParamRequired<_param_name##Type>(mPimpl->map, _key_string);               \
  }

_PARAMETER_FUNCTIONS(SessionName, "session_name")
_PARAMETER_FUNCTIONS(CardId, "card_id")

#undef _PARAMETER_FUNCTIONS

SessionParameters::SessionParameters() : mPimpl(std::make_unique<ParametersPimpl>())
{
  SessionParameters::disableInfoLogger();
}

SessionParameters::SessionParameters(const SessionParameters& other)
{
  SessionParameters::disableInfoLogger();
  mPimpl = std::make_unique<ParametersPimpl>();
  mPimpl->map = other.mPimpl->map;
}

SessionParameters::SessionParameters(SessionParameters&& other)
{
  SessionParameters::disableInfoLogger();
  mPimpl = std::move(other.mPimpl);
}

SessionParameters::~SessionParameters()
{
}

SessionParameters& SessionParameters::operator=(const SessionParameters& other)
{
  if (&other == this) {
    return *this;
  }

  if (!mPimpl) {
    mPimpl = std::make_unique<ParametersPimpl>();
  }
  mPimpl->map = other.mPimpl->map;
  return *this;
}

SessionParameters& SessionParameters::operator=(SessionParameters&& other)
{
  if (&other == this) {
    return *this;
  }

  mPimpl = std::move(other.mPimpl);
  return *this;
}

void SessionParameters::disableInfoLogger()
{
  setenv("INFOLOGGER_MODE", "stdout", true);
}

} // namespace lla
} // namespace o2
