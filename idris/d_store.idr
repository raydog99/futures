module DStoreId

export
data DStoreId = MkDStoreId Integer

export
new : Integer -> DStoreId
new = MkDStoreId

export
clone : DStoreId -> DStoreId
clone (MkDStoreId id) = MkDStoreId id