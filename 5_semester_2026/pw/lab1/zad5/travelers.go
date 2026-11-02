// Zadanie 5: modyfikacja Zadania 3 (odpowiednik Zadania 4 w Go).
// NrOfTravelers = BoardWidth = BoardHeight = 15.
// Podróżnik o identyfikatorze i startuje na polu (i, i).
// Parzysty Id losuje kierunek w pionie, nieparzysty w poziomie,
// i potem zawsze idzie w tym samym kierunku.
package main

import (
	"fmt"
	"math/rand"
	"time"
	"unicode"
)

// Travelers moving on the board
const (
	NrOfTravelers = 15

	MinSteps = 10
	MaxSteps = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond

	// A traveler that waits longer than this for a free field
	// suspects a deadlock (its neighbour had a few chances to move away).
	Timeout = 3 * MaxDelay
)

// 2D Board with torus topology
const (
	BoardWidth  = 15
	BoardHeight = 15
)

// Timing
var startTime = time.Now() // global starting time

// Positions on the board
type Position struct {
	X, Y int
}

// elementary steps
func (p *Position) MoveDown()  { p.Y = (p.Y + 1) % BoardHeight }
func (p *Position) MoveUp()    { p.Y = (p.Y + BoardHeight - 1) % BoardHeight }
func (p *Position) MoveRight() { p.X = (p.X + 1) % BoardWidth }
func (p *Position) MoveLeft()  { p.X = (p.X + BoardWidth - 1) % BoardWidth }

type Direction int

const (
	Up Direction = iota
	Down
	Left
	Right
)

func (p *Position) Move(d Direction) {
	switch d {
	case Up:
		p.MoveUp()
	case Down:
		p.MoveDown()
	case Left:
		p.MoveLeft()
	case Right:
		p.MoveRight()
	}
}

// Each field of the board: a channel with capacity 1.
// Sending a token = entering the field, receiving it = leaving the field.
var board [BoardWidth][BoardHeight]chan struct{}

func initBoard() {
	for x := range board {
		for y := range board[x] {
			board[x][y] = make(chan struct{}, 1)
		}
	}
}

func enter(p Position) {
	board[p.X][p.Y] <- struct{}{}
}

func enterWithTimeout(p Position, timeout time.Duration) bool {
	timer := time.NewTimer(timeout)
	defer timer.Stop()
	select {
	case board[p.X][p.Y] <- struct{}{}:
		return true
	case <-timer.C:
		return false
	}
}

func leave(p Position) {
	<-board[p.X][p.Y]
}

// traces of travelers
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

// Printer: receives the histories of all travelers and prints them
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
	Direction Direction
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

func (t *Traveler) randomDelay() time.Duration {
	return MinDelay + time.Duration(t.rng.Int63n(int64(MaxDelay-MinDelay)+1))
}

func (t *Traveler) init(id int, seed int64, symbol rune) {
	t.rng = rand.New(rand.NewSource(seed))
	t.Id = id
	t.Symbol = symbol
	// Start on the diagonal; the fields (i, i) are all different.
	t.Position = Position{X: id, Y: id}
	enter(t.Position)
	// Even Id moves vertically, odd Id horizontally; fixed direction.
	if id%2 == 0 {
		t.Direction = Direction(t.rng.Intn(2)) // Up or Down
	} else {
		t.Direction = Left + Direction(t.rng.Intn(2)) // Left or Right
	}
	t.storeTrace() // store starting position
	// Number of steps to be made by the traveler
	t.nrOfSteps = MinSteps + t.rng.Intn(MaxSteps-MinSteps+1)
}

func (t *Traveler) run(start <-chan struct{}, reports chan<- []Trace) {
	<-start // wait for initialisations of the remaining travelers
	for step := 0; step < t.nrOfSteps; step++ {
		time.Sleep(t.randomDelay())
		next := t.Position
		next.Move(t.Direction)
		if !enterWithTimeout(next, Timeout) {
			// suspected deadlock: leave the last trace and stop
			t.Symbol = unicode.ToLower(t.Symbol)
			t.storeTrace()
			break
		}
		leave(t.Position)
		t.Position = next
		t.storeTrace()
	}
	reports <- t.traces
}

func main() {
	// Print the line with the parameters needed for display script:
	fmt.Printf("-1 %d %d %d\n", NrOfTravelers, BoardWidth, BoardHeight)

	initBoard()
	reports := make(chan []Trace)
	done := make(chan struct{})
	start := make(chan struct{})
	go printer(reports, done)

	// init travelers
	travelers := make([]Traveler, NrOfTravelers)
	seed := time.Now().UnixNano()
	for i := range travelers {
		travelers[i].init(i, seed+int64(i)*7919, rune('A'+i))
	}

	// start travelers
	for i := range travelers {
		go travelers[i].run(start, reports)
	}
	close(start)

	<-done
}
