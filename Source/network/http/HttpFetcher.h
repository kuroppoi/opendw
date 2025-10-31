#ifndef __HTTP_FETCHER_H__
#define __HTTP_FETCHER_H__

#include "network/HttpRequest.h"
#include "rapidjson/document.h"

namespace opendw::http
{

typedef std::function<void(const std::string&, const rapidjson::Document&)> RequestCallback;

ax::network::ccHttpRequestCallback createCallbackWrapper(RequestCallback callback);
ax::network::HttpRequest* createHttpRequest(ax::network::HttpRequest::Type type,
                                            const std::string& url,
                                            RequestCallback callback);

void get(const std::string& url, RequestCallback callback);
void post(const std::string& url, const char* body, size_t length, RequestCallback callback);
void post(const std::string& url, const rapidjson::Document& document, RequestCallback callback);

}  // namespace opendw::http

#endif  // __HTTP_FETCHER_H__
