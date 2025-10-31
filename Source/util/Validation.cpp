#include "Validation.h"

#include <regex>
#include <sstream>

USING_NS_AX;

namespace opendw::validation
{

char getValueDescriptor(const Value& value)
{
    switch (value.getTypeFamily())
    {
    case Value::Type::INTEGER:
    case Value::Type::FLOAT:
    case Value::Type::DOUBLE:
    case Value::Type::BOOLEAN:
        return 'N';  // Number
    case Value::Type::STRING:
        return 'S';  // String
    case Value::Type::VECTOR:
        return 'A';  // Array
    case Value::Type::MAP:
        return 'D';  // Dictionary
    default:
        return 'x';  // Null
    }
}

std::string createArrayDescriptor(const ValueVector& array)
{
    std::ostringstream result;

    for (const auto& element : array)
    {
        result << getValueDescriptor(element);
    }

    return result.str();
}

bool validateDescriptor(const std::string& descriptor, const std::string& validRegex)
{
    if (validRegex.empty())
    {
        return true;
    }

    std::smatch match;
    std::regex regex(validRegex);
    return std::regex_match(descriptor, match, regex);
}

bool validateArray(const ValueVector& array, const std::string& validRegex)
{
    return validateDescriptor(createArrayDescriptor(array), validRegex);
}

}  // namespace opendw::value_util
