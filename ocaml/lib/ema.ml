module EMA = struct
  type t = {
    mutable b: float;
    mutable n: float;
    mutable avg: float;
  }

  let create ?(b = 0.9) () = {
    b;
    n = 0.;
    avg = 0.;
  }

  let next ema v =
    ema.n <- ema.n +. 1.;
    ema.avg <- ema.b *. v +. (1. -. ema.b) *. ema.avg;
    ema.avg

  let avg ema = ema.avg
end