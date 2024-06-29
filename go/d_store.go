package futures

import "sync/atomic"

type DStoreId struct {
    id *int64
}

func NewDStoreId(id int64) DStoreId {
    return DStoreId{id: &id}
}

func (d DStoreId) Clone() DStoreId {
    newId := atomic.LoadInt64(d.id)
    return NewDStoreId(newId)
}