module EMA

record EMA where
  constructor MkEMA
  b : Double
  n : Double
  avg : Double

defaultEMA : EMA
defaultEMA = MkEMA 0.9 0 0

next : EMA -> Double -> (EMA, Double)
next ema v =
  let newN = ema.n + 1
      newAvg = ema.b * v + (1 - ema.b) * ema.avg
      newEMA = record { n = newN, avg = newAvg } ema
  in (newEMA, newAvg)

avg : EMA -> Double
avg = avg