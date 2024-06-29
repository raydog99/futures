module Futures
  ( DFut(..)
  , new
  , share
  ) where

import qualified DStoreId

data DFut a = DFut
  { dStoreId :: DStoreId.DStoreId
  }

new :: DStoreId.DStoreId -> DFut a
new dStoreId = DFut { dStoreId = dStoreId }

share :: DFut a -> DFut a
share dfut = DFut { dStoreId = DStoreId.clone (dStoreId dfut) }

instance DStoreId.FromDStoreId (DFut a) where
  fromDStoreId = new