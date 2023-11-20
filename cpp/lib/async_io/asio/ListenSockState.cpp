
class SelectSourceIF;

class SelectSource : public SelectSourceIF {
public:
    void registerItem(Selectable* sel) override {
        // Implementation for registering Selectable items
    }

    void deregisterItem(Selectable* sel) override {
        // Implementation for deregistering Selectable items
    }
};

class ListenSockStateNbio : public ListenSockState {
private:
    NonblockingServerSocket nbservsock;
    SelectItem si;
    SelectSource listen_selsource;

public:
    ListenSockStateNbio(ATcpListenRequest* req, SelectSourceIF* listen_selsource) : ListenSockState(req) {
        this->listen_selsource = *dynamic_cast<SelectSource*>(listen_selsource);
        si = SelectItem(&nbservsock, this, Selectable::ACCEPT_READY);
        listen_selsource.registerItem(&si);
    }

protected:
    Socket accept() override {
        if (nbservsock == nullptr) return nullptr; // If already closed
        NonblockingSocket nbsock;
        try {
            nbsock = nbservsock.nbAccept();
        } catch (const SocketException& e) {
            // Assume this is a 'Too many open files' exception
            // XXX Probably want to throttle the listenthread somehow?
            return nullptr;
        } catch (const IOException& e) {
            std::cerr << "LSS: nbAccept got IOException: " << e.what() << std::endl;

            ATcpServerSocketClosedEvent dead(servsock);
            compQ.enqueue_lossy(dead);
            // Deregister
            listen_selsource.deregisterItem(&si);
            throw e;
        }

        return nbsock;
    }

    void suspend() override {
        if (nbservsock == nullptr) return; // If already closed
        std::cerr << "LSS: Suspending accept on " << servsock << std::endl;
        si.events &= ~(Selectable::ACCEPT_READY);
        listen_selsource.update(&si);
    }

    void resume() override {
        if (nbservsock == nullptr) return; // If already closed
        std::cerr << "LSS: Resuming accept on " << servsock << std::endl;
        si.events |= Selectable::ACCEPT_READY;
        listen_selsource.update(&si);
    }
};