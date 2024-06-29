package futures

import (
    "futures/dstore"
)

type DFut[T any] struct {
    DStoreID dstore.DStoreId
}

func New[T any](dStoreID dstore.DStoreId) DFut[T] {
    return DFut[T]{
        DStoreID: dStoreID,
    }
}

func (d DFut[T]) Share() DFut[T] {
    return DFut[T]{
        DStoreID: d.DStoreID.Clone(),
    }
}

func FromDStoreId[T any](dStoreID dstore.DStoreId) DFut[T] {
    return New[T](dStoreID)
}