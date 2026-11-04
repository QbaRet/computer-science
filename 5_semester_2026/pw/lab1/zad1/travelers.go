package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	NrOfTravelers = 15

	MinSteps = 10
	MaxSteps = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond
)

const (
	BoardWidth  = 15
	BoardHeight = 15
)

var startTime = time.Now()

type Position struct {
	X, Y int
}

func (p *Position) MoveDown()  { p.Y = (p.Y + 1) % BoardHeight }
func (p *Position) MoveUp()    { p.Y = (p.Y + BoardHeight - 1) % BoardHeight }
func (p *Position) MoveRight() { p.X = (p.X + 1) % BoardWidth }
func (p *Position) MoveLeft()  { p.X = (p.X + BoardWidth - 1) % BoardWidth }

type Trace struct {
	TimeStamp time.Duration
	Id        int
	Position  Position
	Symbol    rune
}

func printTraces(traces []Trace) {
	for _, t := range traces {
		fmt.Printf("%.9f %d %d %d %c\n",
			t.TimeStamp.Seconds(), t.Id, t.Position.X, t.Position.Y, t.Symbol)
	}
}

func printer(reports <-chan []Trace, done chan<- struct{}) {
	for i := 0; i < NrOfTravelers; i++ {
		printTraces(<-reports)
	}
	close(done)
}

type Traveler struct {
	Id        int
	Symbol    rune
	Position  Position
	rng       *rand.Rand
	nrOfSteps int
	traces    []Trace
}

func (t *Traveler) storeTrace() {
	t.traces = append(t.traces, Trace{
		TimeStamp: time.Since(startTime),
		Id:        t.Id,
		Position:  t.Position,
		Symbol:    t.Symbol,
	})
}

func (t *Traveler) makeStep() {
	switch t.rng.Intn(4) {
	case 0:
		t.Position.MoveUp()
	case 1:
		t.Position.MoveDown()
	case 2:
		t.Position.MoveLeft()
	case 3:
		t.Position.MoveRight()
	}
}

func (t *Traveler) randomDelay() time.Duration {
	return MinDelay + time.Duration(t.rng.Int63n(int64(MaxDelay-MinDelay)+1))
}

func (t *Traveler) init(id int, seed int64, symbol rune) {
	t.rng = rand.New(rand.NewSource(seed))
	t.Id = id
	t.Symbol = symbol
	t.Position = Position{X: t.rng.Intn(BoardWidth), Y: t.rng.Intn(BoardHeight)}
	t.storeTrace()
	t.nrOfSteps = MinSteps + t.rng.Intn(MaxSteps-MinSteps+1)
}

func (t *Traveler) run(start <-chan struct{}, reports chan<- []Trace) {
	<-start
	for step := 0; step < t.nrOfSteps; step++ {
		time.Sleep(t.randomDelay())
		t.makeStep()
		t.storeTrace()
	}
	reports <- t.traces
}

func main() {
	fmt.Printf("-1 %d %d %d\n", NrOfTravelers, BoardWidth, BoardHeight)

	reports := make(chan []Trace)
	done := make(chan struct{})
	start := make(chan struct{})
	go printer(reports, done)

	travelers := make([]Traveler, NrOfTravelers)
	seed := time.Now().UnixNano()
	symbol := 'A'
	for i := range travelers {
		travelers[i].init(i, seed+int64(i)*7919, symbol)
		symbol++
	}

	for i := range travelers {
		go travelers[i].run(start, reports)
	}
	close(start)

	<-done
}
