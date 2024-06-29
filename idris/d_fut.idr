module Futures

import public DStoreId

export
data DFut : Type -> Type where
  MkDFut : DStoreId -> DFut a

export
new : DStoreId -> DFut a
new = MkDFut

export
share : DFut a -> DFut a
share (MkDFut id) = MkDFut (clone id)

export
fromDStoreId : DStoreId -> DFut a
fromDStoreId = new