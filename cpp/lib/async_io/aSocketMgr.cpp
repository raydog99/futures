#include "async_io/ManagerIF.h"
#include "async_io/SystemManagerIF.h"
#include "async_io/ThreadManagerIF.h"
#include "async_io/ConfigData.h"
#include "async_io/Sandstorm.h"
#include "async_io/SandstormConfig.h"
#include "async_io/aSocketImplFactory.h"
#include "async_io/Tracer.h"

using namespace root::include::async_io;
using namespace root::include::rpc;
using namespace root::internal;

#include "async_io/EventHandlerIF.h"

class aSocketMgr {
public:
    static Tracer tracer;

    static void initialize(ManagerIF* mgr, SystemManagerIF* sysmgr) throw (Exception) {
        synchronized (init_lock) {
            if (PROFILE) tracer = new Tracer("aSocketMgr");

            SandstormConfigIF cfg = mgr->getConfig();

            String provider = cfg->getString("global.aSocket.provider", "asio"); // Default to "asio"
            
            if (!provider->equals("asio")) {
                throw RuntimeException("aSocketMgr: Only 'asio' is allowed for global.aSocket.provider");
            }

            try {
                factory = aSocketImplFactory.getFactory();
            }
            catch (Exception e) {
                throw RuntimeException("aSocketMgr: Cannot create aSocketImplFactory: " + e);
            }

            aSocketTM = new aSocketThreadManager(mgr);
            sysmgr->addThreadManager("aSocket", aSocketTM);

            ReadEventHandler revh = new ReadEventHandler();
            read_handler = revh;    // save this for calls to interruptSelect.
            aSocketStageWrapper rsw;
            if (cfg->getBoolean("global.aSocket.governor.enable")) {
                aSocketRCTM = new aSocketRCTMSleep(mgr);
                sysmgr->addThreadManager("aSocketRCTM", aSocketRCTM);
                rsw = new aSocketStageWrapper(mgr, "aSocket ReadStage",
                    revh, new ConfigData(mgr), aSocketRCTM);
            }
            else {
                rsw = new aSocketStageWrapper(mgr, "aSocket ReadStage",
                    revh, new ConfigData(mgr), aSocketTM);
            }

            StageIF* readStage = sysmgr->createStage(rsw, true);
            read_sink = readStage->getSink();

            ListenEventHandler levh = new ListenEventHandler();
            aSocketStageWrapper lsw = new aSocketStageWrapper(mgr, "aSocket ListenStage",
                levh, new ConfigData(mgr), aSocketTM);
            StageIF* listenStage = sysmgr->createStage(lsw, true);
            listen_sink = listenStage->getSink();

            WriteEventHandler wevh = new WriteEventHandler();
            aSocketStageWrapper wsw = new aSocketStageWrapper(mgr, "aSocket WriteStage",
                wevh, new ConfigData(mgr), aSocketTM);
            StageIF* writeStage = sysmgr->createStage(wsw, true);
            write_sink = writeStage->getSink();

            initialized = true;
        }
    }

    static void init() {
    synchronized (init_lock) {
        if (!initialized) {
            try {
                Sandstorm* ss = Sandstorm::getSandstorm();
                if (ss != nullptr) {
                    initialize(ss->getManager(), ss->getSystemManager());
                }
                else {
                    SandstormConfig* cfg = new SandstormConfig();
                    ss = new Sandstorm(*cfg);
                }
            }
            catch (Exception e) {
                System.err.println("aSocketMgr: Warning: Initialization failed: " + e);
                e.printStackTrace();
                return;
            }
        }
    }
}

private:
    static const bool DEBUG = false;
    static const bool PROFILE = false;

    static ThreadManagerIF aSocketTM, aSocketRCTM;
    static SinkIF read_sink;
    static SinkIF listen_sink;
    static SinkIF write_sink;

    static ReadEventHandler read_handler;

    static Object init_lock;
    static bool initialized;

    static bool USE_NIO;
    static aSocketImplFactory factory;
};