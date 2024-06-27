package futures

import (
    "sync"
    lru "github.com/hashicorp/golang-lru"
)

const ClientCacheSize = 20

type Connectable interface {
    Connect(address string) (interface{}, error)
}

type ClientPool struct {
    clientCache *lru.Cache
    mutex       sync.Mutex
}

func NewClientPool() (*ClientPool, error) {
    cache, err := lru.New(ClientCacheSize)
    if err != nil {
        return nil, err
    }
    return &ClientPool{
        clientCache: cache,
    }, nil
}

func (p *ClientPool) Connect(c Connectable, address string) (interface{}, error) {
    p.mutex.Lock()
    defer p.mutex.Unlock()

    if client, ok := p.clientCache.Get(address); ok {
        return client, nil
    }

    client, err := c.Connect(address)
    if err != nil {
        return nil, err
    }

    p.clientCache.Add(address, client)
    return client, nil
}

func (p *ClientPool) GetClient(c Connectable, address string) (interface{}, error) {
    p.mutex.Lock()
    defer p.mutex.Unlock()

    if client, ok := p.clientCache.Get(address); ok {
        return client, nil
    }

    return p.Connect(c, address)
}