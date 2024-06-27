package futures

type EMA struct {
    B   float64
    N   float64
    Avg float64
}

func NewEMA() *EMA {
    return &EMA{
        B:   0.9,
        N:   0,
        Avg: 0,
    }
}

func (e *EMA) Next(v float64) float64 {
    e.N++
    e.Avg = e.B*v + (1-e.B)*e.Avg
    return e.Avg
}

func (e *EMA) Avg() float64 {
    return e.Avg
}