#include "packageprocessor.h"

std::string PackageProcessor::getSendPackageData(int32_t cmd, int32_t seq, const std::string &data)
{
    std::string outbuf;
    net::BinaryStreamWriter writeStream(&outbuf);
    writeStream.WriteInt32(cmd);
    writeStream.WriteInt32(seq);
    writeStream.WriteCString(data.c_str(), data.length());
    writeStream.Flush();

    std::string srcbuf(outbuf.c_str(), outbuf.length());

    std::string strPackageData;
    chat_msg_header header;
    header.originsize = outbuf.length();

    //插入一个包头
    strPackageData.append((const char*)&header, sizeof(header));
    strPackageData.append(srcbuf);

    return strPackageData;
}

json PackageProcessor::getSendPackageData(std::string &data)
{
    json j;

    net::BinaryStreamReader readStream(data.c_str(), data.length());
    int32_t cmd;
    if (!readStream.ReadInt32(cmd))
    {
        // LOGE("read cmd error, client: %s", conn->peerAddress().toIpPort().c_str());
        printf("read cmd error, data: %s\n", data.c_str());
        return j;
    }

    j["cmd"] = cmd;

    int32_t seq;
    if (!readStream.ReadInt32(seq))
    {
        // LOGE("read seq error, client: %s", conn->peerAddress().toIpPort().c_str());
        printf("read seq error, data: %s", data.c_str());
        return j;
    }

    j["seq"] = seq;

    std::string content;
    size_t contentlength;
    if (!readStream.ReadString(&content, 0, contentlength))
    {
        // LOGE("read data error, client: %s", conn->peerAddress().toIpPort().c_str());
        printf("read data error, data: %s\n", data.c_str());
        return j;
    }
    j["data"] = content;

    return j;
}
