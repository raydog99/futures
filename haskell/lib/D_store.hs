module DStoreId
  ( DStoreId
  , new
  , clone
  ) where

import Data.IORef

newtype DStoreId = DStoreId (IORef Int)

new :: Int -> IO DStoreId
new id = DStoreId <$> newIORef id

clone :: DStoreId -> IO DStoreId
clone (DStoreId ref) = do
  value <- readIORef ref
  DStoreId <$> newIORef value