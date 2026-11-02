// Zadanie 3: wersja Zadania 2 w Go.
// Na jednym polu może przebywać co najwyżej jeden podróżnik.
// Każde pole to kanał o pojemności 1 (token = pole zajęte).
// Podejrzenie deadlocku wykrywane jest przez Timeout.
//
// Uruchomienie:  ./travelers [NrOfTravelers]   (domyślnie 15)
package main

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
	"unicode"
)

// 2D Board with torus topology
const (
	BoardWidth  = 15
	BoardHeight = 15
)

// Travelers moving on the board
const (
	DefaultNrOfTravelers = 15

	MinSteps = 10
	MaxSteps = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond

	// A traveler that waits longer than this for a free field
	// suspects a deadlock (its neighbour had a few chances to move away).
	Timeout = 3 * MaxDelay
)

// set once in main; optional first argument changes the crowding of the board
var NrOfTravelers = DefaultNrOfTravelers

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

func tryEnter(p Position) bool {
	select {
	case board[p.X][p.Y] <- struct{}{}:
		return true
	default:
		return false
	}
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

func (t *Traveler) randomNeighbour() Position {
	p := t.Position
	switch t.rng.Intn(4) {
	case 0:
		p.MoveUp()
	case 1:
		p.MoveDown()
	case 2:
		p.MoveLeft()
	case 3:
		p.MoveRight()
	}
	return p
}

func (t *Traveler) randomDelay() time.Duration {
	return MinDelay + time.Duration(t.rng.Int63n(int64(MaxDelay-MinDelay)+1))
}

func (t *Traveler) init(id int, seed int64, symbol rune) {
	t.rng = rand.New(rand.NewSource(seed))
	t.Id = id
	t.Symbol = symbol
	// Random initial position on a free field:
	for {
		t.Position = Position{X: t.rng.Intn(BoardWidth), Y: t.rng.Intn(BoardHeight)}
		if tryEnter(t.Position) {
			break
		}
	}
	t.storeTrace() // store starting position
	// Number of steps to be made by the traveler
	t.nrOfSteps = MinSteps + t.rng.Intn(MaxSteps-MinSteps+1)
}

func (t *Traveler) run(start <-chan struct{}, reports chan<- []Trace) {
	<-start // wait for initialisations of the remaining travelers
	for step := 0; step < t.nrOfSteps; step++ {
		time.Sleep(t.randomDelay())
		next := t.randomNeighbour()
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
	if len(os.Args) > 1 {
		n, err := strconv.Atoi(os.Args[1])
		if err != nil || n < 1 || n > BoardWidth*BoardHeight {
			fmt.Fprintf(os.Stderr, "Nr_Of_Travelers must be in 1..%d\n", BoardWidth*BoardHeight)
			os.Exit(1)
		}
		NrOfTravelers = n
	}

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
		travelers[i].init(i, seed+int64(i)*7919, rune('A'+i%26))
	}

	// start travelers
	for i := range travelers {
		go travelers[i].run(start, reports)
	}
	close(start)

	<-done
}
