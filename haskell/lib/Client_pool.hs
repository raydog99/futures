{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE FlexibleContexts #-}

module ClientPool 
  ( ClientPool
  , Connectable(..)
  , create
  , connect
  , getClient
  ) where

import Control.Concurrent.STM
import Control.Monad.IO.Class
import Data.Cache.LRU as LRU
import Data.Text (Text)
import qualified Data.Text as T

data ClientPool a = ClientPool
  { clientCache :: TVar (LRU Text a)
  }

class Connectable a where
  connect :: Text -> IO a

clientCacheSize :: Int
clientCacheSize = 20

create :: IO (ClientPool a)
create = ClientPool <$> newTVarIO (LRU.newLRU (Just clientCacheSize))

connectClient :: (MonadIO m, Connectable a) => ClientPool a -> Text -> m a
connectClient pool address = liftIO $ do
  client <- connect address
  atomically $ do
    cache <- readTVar (clientCache pool)
    let (_, cache') = LRU.insert address client cache
    writeTVar (clientCache pool) cache'
  return client

getClient :: (MonadIO m, Connectable a) => ClientPool a -> Text -> m a
getClient pool address = liftIO $ atomically $ do
  cache <- readTVar (clientCache pool)
  case LRU.lookup address cache of
    Just client -> return client
    Nothing -> do
      client <- liftIO $ connect address
      let (_, cache') = LRU.insert address client cache
      writeTVar (clientCache pool) cache'
      return client