module EMA (EMA, defaultEMA, next, avg) where

import Data.IORef

data EMA = EMA
  { b :: IORef Double
  , n :: IORef Double
  , avg :: IORef Double
  }

defaultEMA :: IO EMA
defaultEMA = do
  b <- newIORef 0.9
  n <- newIORef 0
  avg <- newIORef 0
  return $ EMA b n avg

next :: EMA -> Double -> IO Double
next ema v = do
  modifyIORef' (n ema) (+ 1)
  b' <- readIORef (b ema)
  avg' <- readIORef (avg ema)
  let newAvg = b' * v + (1 - b') * avg'
  writeIORef (avg ema) newAvg
  return newAvg

avg :: EMA -> IO Double
avg ema = readIORef (avg ema)