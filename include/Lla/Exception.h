// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Exception.h
/// \brief Defintion of the LLA exceptions.
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#ifndef O2_LLA_INC_EXCEPTION_H_
#define O2_LLA_INC_EXCEPTION_H_

#include <Common/Exceptions.h>

namespace o2
{
namespace lla
{

struct LlaException : AliceO2::Common::Exception {
};

struct ParameterException : AliceO2::Common::Exception {
};

namespace ErrorInfo
{
using Message = AliceO2::Common::ErrorInfo::Message;
} // namespace ErrorInfo

} // namespace lla
} // namespace o2

#endif // O2_LLA_SRC_EXCEPTION_H_
