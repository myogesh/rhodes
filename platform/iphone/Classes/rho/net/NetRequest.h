#pragma once

#ifdef __cplusplus

#include "net/INetRequest.h"
#include "logging/RhoLog.h"

namespace rho {
namespace net {

class CNetRequest : public INetRequest
{
    DEFINE_LOGCLASS;
	boolean m_bCancel;
public:
	void* m_pConnData;
	
    CNetRequest(void) : m_pConnData(0){}
    virtual ~CNetRequest(void){}

    virtual INetData* pullData(const String& strUrl );
    virtual boolean pushData(const String& strUrl, const String& strBody);
    virtual boolean pushFile(const String& strUrl, const String& strFileName);
    virtual boolean pullFile(const String& strUrl, const String& strFilePath);
    virtual boolean pullCookies(const String& strUrl, const String& strBody);
    //if strUrl.length() == 0 delete all cookies if possible
    virtual void deleteCookie(const String& strUrl);

    virtual String resolveUrl(const String& strUrl);

    virtual void cancel();
};

}
}
#else
#include "common/RhoPort.h"
#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

int rho_net_has_network();
	
#ifdef __cplusplus
};
#endif //__cplusplus
	
