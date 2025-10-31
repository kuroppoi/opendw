#include "HttpFetcher.h"

#include <format>

#include "network/HttpClient.h"
#include "network/HttpResponse.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

USING_NS_AX;
using namespace network;

namespace opendw::http
{

ccHttpRequestCallback createCallbackWrapper(RequestCallback callback)
{
    return [callback](HttpClient* client, HttpResponse* response) {
        std::string error;
        rapidjson::Document document;

        if (!response->isSucceed())
        {
            auto status = response->getResponseCode();

            switch (status)
            {
            case -1:
                error = "Couldn't connect to the server.";
                break;
            default:
                error = std::format("Oops - a server error occured ({})", status);
                break;
            }
        }
        else
        {
            auto data   = response->getResponseData();
            auto length = data->length();

            if (length > 0)
            {
                document.Parse(data->begin(), length);
            }
            else
            {
                document.SetObject();  // Interpret empty response as empty object
            }

            if (document.HasParseError() || !document.IsObject())
            {
                error = "Oops - received an invalid response";
            }
            else if (document.HasMember("error"))
            {
                auto& value = document["error"];
                error       = value.IsString() ? value.GetString() : "Oops - an error occured";
            }
        }

        callback(error, document);
    };
}

HttpRequest* createHttpRequest(HttpRequest::Type type, const std::string& url, RequestCallback callback)
{
    auto request = new HttpRequest();
    request->setRequestType(type);
    request->setUrl(url);

    if (callback)
    {
        request->setResponseCallback(createCallbackWrapper(callback));
    }

    return request;
}

void get(const std::string& url, RequestCallback callback)
{
    AXLOGI("[HttpFetcher] Sending GET request: {}", url);
    auto request = createHttpRequest(HttpRequest::Type::GET, url, callback);
    HttpClient::getInstance()->send(request);
    request->release();
}

void post(const std::string& url, const char* body, size_t length, RequestCallback callback)
{
    AXLOGI("[HttpFetcher] Sending POST request: {}", url);
    auto request = createHttpRequest(HttpRequest::Type::POST, url, callback);
    request->setRequestData(body, length);
    HttpClient::getInstance()->send(request);
    request->release();
}

void post(const std::string& url, const rapidjson::Document& document, RequestCallback callback)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    auto body = buffer.GetString();
    auto size = buffer.GetSize();
    post(url, body, size, callback);
}

}  // namespace opendw::http
