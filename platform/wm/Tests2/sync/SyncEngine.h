#pragma once

#include "common/RhoStd.h"
#include "common/IRhoClassFactory.h"
#include "net/INetRequest.h"

#include "SyncSource.h"
#include "db/DBAdapter.h"

namespace rho {
namespace sync {

class CSyncEngine
{
public:
    enum ESyncState{ esNone, esSyncAllSources, esSyncSource, esStop, esExit };

    static String SYNC_SOURCE_FORMAT() { return "?format=json"; }
    static String SYNC_ASK_ACTION() { return "/ask"; }
    static int MAX_SYNC_TRY_COUNT() { return 2; }
#ifdef OS_SYMBIAN
    static String SYNC_PAGE_SIZE() { return "200"; }
#else
    static String SYNC_PAGE_SIZE() { return "1000"; }
#endif

private:
    VectorPtr<CSyncSource*> m_sources;
    db::CDBAdapter& m_dbAdapter;
    net::INetRequest* m_NetRequest;
    ESyncState m_syncState;
    String     m_clientID;

public:
    CSyncEngine(db::CDBAdapter& db): m_dbAdapter(db), m_NetRequest(0){}
    ~CSyncEngine(void){}

    void setFactory(common::IRhoClassFactory* factory){ 
        m_NetRequest = factory->createNetRequest();
    }

    void doSyncAllSources();
    void doSyncSource();

    void setState(ESyncState eState){ m_syncState = eState; }
    ESyncState getState()const{ return m_syncState; }
    boolean isContinueSync()const{ return m_syncState != esExit && m_syncState != esStop; }
    void stopSync(){ setState(esStop); }

//private:
    String getClientID()const{ return m_clientID; }

    CSyncEngine(): m_dbAdapter(db::CDBAdapter()), m_NetRequest(0){}

    void loadAllSources();
    void syncAllSources();
    VectorPtr<CSyncSource*>& getSources(){ return m_sources; }
    int getStartSource();
    void loadClientID();
    String requestClientIDByNet();

    void fireNotification( int nSourceID, int nSyncObjectsCount)
    {
        //TODO: fireNotification
    }

private:
    db::CDBAdapter& getDB(){ return m_dbAdapter; }
    net::INetRequest& getNet(){ return *m_NetRequest;}

    friend class CSyncSource;
};

}
}