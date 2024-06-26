module ClientPool

import Data.SortedMap
import Control.Monad.State

%default total

CLIENT_CACHE_SIZE : Nat
CLIENT_CACHE_SIZE = 20

interface Connect a where
  connect : String -> IO a

record ClientPool a where
  constructor MkClientPool
  clientCache : SortedMap String a

ClientPoolM : Type -> Type -> Type
ClientPoolM a = StateT (ClientPool a) IO

initClientPool : ClientPool a
initClientPool = MkClientPool empty

connect : Connect a => String -> ClientPoolM a a
connect address = do
  pool <- get
  client <- lift $ connect address
  let newCache = insert address client pool.clientCache
  put $ MkClientPool newCache
  pure client

getClient : Connect a => String -> ClientPoolM a a
getClient address = do
  pool <- get
  case lookup address pool.clientCache of
    Just client => pure client
    Nothing => connect address

runClientPool : ClientPoolM a b -> IO b
runClientPool action = evalStateT action initClientPool