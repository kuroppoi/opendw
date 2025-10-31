#ifndef __VALIDATION_H__
#define __VALIDATION_H__

#include "base/Value.h"

namespace opendw::validation
{

char getValueDescriptor(const ax::Value& value);

std::string createArrayDescriptor(const ax::ValueVector& array);

bool validateDescriptor(const std::string& descriptor, const std::string& validRegex);
bool validateArray(const ax::ValueVector& array, const std::string& validRegex);

}  // namespace opendw::validation

#endif  // __VALIDATION_H__
