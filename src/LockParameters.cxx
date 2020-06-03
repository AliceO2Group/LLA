// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file LockParameters.cxx
/// \brief Implementation of the LockParameters and associated functions.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <boost/throw_exception.hpp>
#include <map>
#include "Lla/Exception.h"
#include "Lla/LockParameters.h"

namespace o2
{
namespace lla
{

using Variant = boost::variant<std::string, LockType::Type>;
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

#define _PARAMETER_FUNCTIONS(_param_name, _key_string)                              \
  auto LockParameters::set##_param_name(_param_name##Type value)->LockParameters&   \
  {                                                                                 \
    setParam(mPimpl->map, _key_string, value);                                      \
    return *this;                                                                   \
  }                                                                                 \
                                                                                    \
  auto LockParameters::get##_param_name() const->boost::optional<_param_name##Type> \
  {                                                                                 \
    return getParam<_param_name##Type>(mPimpl->map, _key_string);                   \
  }                                                                                 \
                                                                                    \
  auto LockParameters::get##_param_name##Required() const->_param_name##Type        \
  {                                                                                 \
    return getParamRequired<_param_name##Type>(mPimpl->map, _key_string);           \
  }

_PARAMETER_FUNCTIONS(LockType, "lock_type")
_PARAMETER_FUNCTIONS(LockName, "lock_name")

#undef _PARAMETER_FUNCTIONS

LockParameters::LockParameters() : mPimpl(std::make_unique<ParametersPimpl>())
{
}

LockParameters::LockParameters(const LockParameters& other)
{
  mPimpl = std::make_unique<ParametersPimpl>();
  mPimpl->map = other.mPimpl->map;
}

LockParameters::LockParameters(LockParameters&& other)
{
  mPimpl = std::move(other.mPimpl);
}

LockParameters::~LockParameters()
{
}

LockParameters& LockParameters::operator=(const LockParameters& other)
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

LockParameters& LockParameters::operator=(LockParameters&& other)
{
  if (&other == this) {
    return *this;
  }

  mPimpl = std::move(other.mPimpl);
  return *this;
}

} // namespace lla
} // namespace o2
