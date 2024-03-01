#ifndef PACKAGEPROCESSOR_H
#define PACKAGEPROCESSOR_H

#include <string>
#include <cstdint>
#include "protocolstream.h"
#include "json.hpp"
#include "Msg.h"


using json = nlohmann::json;


class PackageProcessor
{
public:
    PackageProcessor() = delete;
    static std::string getSendPackageData(int32_t cmd, int32_t seq, const std::string& data);
    static json getSendPackageData(std::string& data);
};

#endif // PACKAGEPROCESSOR_H
