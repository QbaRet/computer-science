package main

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
	"unicode"
)

const (
	BoardWidth  = 15
	BoardHeight = 15
)

const (
	DefaultNrOfTravelers = 15

	MinSteps = 10
	MaxSteps = 100

	MinDelay = 10 * time.Millisecond
	MaxDelay = 50 * time.Millisecond

	Timeout = 3 * MaxDelay

	// Dzicy lokatorzy
	NrOfSquatters   = 10
	MaxAppearDelay  = 2 * time.Second // pojawia sie w losowym momencie 0..2 s
	MinSquatterLife = 500 * time.Millisecond
	MaxSquatterLife = 1500 * time.Millisecond
	MaxAppearTries  = 20

	// Pulapki
	NrOfTraps = 10
	TrapSleep = 100 * time.Millisecond // ofiara pulapki "zasypia" na chwile
)

// Pulapki sa losowane w main, zanim ruszy jakikolwiek podroznik.
// Pozniej ta tablica jest juz tylko czytana.
var isTrap [BoardWidth][BoardHeight]bool
var trapPositions []Position

var NrOfTravelers = DefaultNrOfTravelers

var startTime = time.Now()

type Position struct {
	X, Y int
}

// (BoardWidth, BoardHeight) oznacza "poza plansza"
var Hidden = Position{BoardWidth, BoardHeight}

func (p *Position) MoveDown()  { p.Y = (p.Y + 1) % BoardHeight }
func (p *Position) MoveUp()    { p.Y = (p.Y + BoardHeight - 1) % BoardHeight }
func (p *Position) MoveRight() { p.X = (p.X + 1) % BoardWidth }
func (p *Position) MoveLeft()  { p.X = (p.X + BoardWidth - 1) % BoardWidth }

func randomPosition(rng *rand.Rand) Position {
	return Position{X: rng.Intn(BoardWidth), Y: rng.Intn(BoardHeight)}
}

type Trace struct {
	TimeStamp time.Duration
	Id        int
	Position  Position
	Symbol    rune
}

func newTrace(id int, p Position, symbol rune) Trace {
	return Trace{TimeStamp: time.Since(startTime), Id: id, Position: p, Symbol: symbol}
}

func printTraces(traces []Trace) {
	for _, t := range traces {
		fmt.Printf("%.9f %d %d %d %c\n",
			t.TimeStamp.Seconds(), t.Id, t.Position.X, t.Position.Y, t.Symbol)
	}
}

// ---------------------------------------------------------------------------
// Dziki lokator: dane, ktore "mieszkaja" w gorutynie pola. Gdy lokator musi
// sie wyprowadzic, pole przekazuje caly rekord sasiedniemu polu.
// W danej chwili rekord nalezy do dokladnie jednego pola.

type Squatter struct {
	Id        int
	Symbol    rune
	deathTime time.Time
	traces    []Trace
}

func (s *Squatter) storeTrace(p Position) {
	s.traces = append(s.traces, newTrace(s.Id, p, s.Symbol))
}

// ---------------------------------------------------------------------------
// Pole jako "stateful goroutine": stan pola (kto na nim stoi) zna tylko
// jego wlasna gorutyna. Inni moga jedynie wysylac prosby przez kanaly.

type Occupant int

const (
	None Occupant = iota
	TravelerOcc
	SquatterOcc
	TrappedSquatterOcc // lokator zlapany w pulapke: "spi" i blokuje pole
)

type EnterResult int

const (
	Entered EnterResult = iota
	Blocked             // lokator nie mial dokad uciec
	Trapped             // pole bylo pulapka
)

type enterRequest struct {
	from  Position
	reply chan EnterResult
}

type arriveRequest struct {
	sq   *Squatter
	done chan struct{} // potwierdzenie: slad lokatora juz zapisany
}

type Field struct {
	pos       Position
	enter     chan enterRequest
	leave     chan struct{}
	arrive    chan arriveRequest
	getTraces chan chan []Trace // printer prosi pulapke o jej slady

	trapId     int // -1 gdy pole nie jest pulapka
	trapTraces []Trace
}

func (f *Field) isTrap() bool { return f.trapId >= 0 }

func (f *Field) storeTrapTrace() {
	f.trapTraces = append(f.trapTraces, newTrace(f.trapId, f.pos, '#'))
}

var board [BoardWidth][BoardHeight]*Field

var reports = make(chan []Trace)

func (f *Field) neighbours() []Position {
	up, down, left, right := f.pos, f.pos, f.pos, f.pos
	up.MoveUp()
	down.MoveDown()
	left.MoveLeft()
	right.MoveRight()
	return []Position{up, down, left, right}
}

// Proba oddania lokatora wolnemu sasiadowi (innemu niz from).
// select z default: jesli sasiad nie moze przyjac lokatora od razu,
// probujemy nastepnego - pole nigdy nie czeka na inne pole.
func (f *Field) moveSquatterAway(sq *Squatter, from Position) bool {
	for _, n := range f.neighbours() {
		if n == from {
			continue
		}
		req := arriveRequest{sq: sq, done: make(chan struct{})}
		select {
		case board[n.X][n.Y].arrive <- req:
			<-req.done
			return true
		default:
		}
	}
	return false
}

func (f *Field) run() {
	occupant := None
	var sq *Squatter           // wazne tylko gdy occupant == SquatterOcc
	var death <-chan time.Time // zegar smierci lokatora (nil = brak)

	for {
		// "Strazniki" jak w Adzie: kanal nil nigdy nie jest gotowy,
		// wiec dana galaz select jest wtedy wylaczona.
		var enterCh chan enterRequest
		var leaveCh chan struct{}
		var arriveCh chan arriveRequest
		switch occupant {
		case None:
			enterCh, arriveCh = f.enter, f.arrive
		case TravelerOcc:
			leaveCh = f.leave
		case SquatterOcc:
			enterCh = f.enter
		}

		select {
		case req := <-enterCh:
			if occupant == SquatterOcc {
				if !f.moveSquatterAway(sq, req.from) {
					req.reply <- Blocked
					continue
				}
				sq, death = nil, nil
			}
			occupant = TravelerOcc
			if f.isTrap() {
				req.reply <- Trapped
			} else {
				req.reply <- Entered
			}

		case <-leaveCh:
			occupant = None
			if f.isTrap() {
				f.storeTrapTrace() // ofiara zniknela, znow '#'
			}

		case req := <-arriveCh:
			sq = req.sq
			if f.isTrap() {
				// lokator wpadl w pulapke: '!', chwila snu, potem znika
				sq.Symbol = '!'
				sq.deathTime = time.Now().Add(TrapSleep)
				occupant = TrappedSquatterOcc
			} else {
				occupant = SquatterOcc
			}
			sq.storeTrace(f.pos)
			death = time.After(time.Until(sq.deathTime))
			close(req.done)

		case <-death:
			sq.storeTrace(Hidden) // znika z planszy
			if occupant == TrappedSquatterOcc {
				f.storeTrapTrace() // pulapka znow widoczna jako '#'
			}
			reports <- sq.traces
			sq, death = nil, nil
			occupant = None

		case reply := <-f.getTraces:
			reply <- f.trapTraces
		}
	}
}

// ---------------------------------------------------------------------------

func printer(nrOfReports int, done chan<- struct{}) {
	for i := 0; i < nrOfReports; i++ {
		printTraces(<-reports)
	}
	// wszyscy podroznicy i lokatorzy skonczyli: zbieramy slady pulapek
	for _, p := range trapPositions {
		reply := make(chan []Trace)
		board[p.X][p.Y].getTraces <- reply
		printTraces(<-reply)
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
	t.traces = append(t.traces, newTrace(t.Id, t.Position, t.Symbol))
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

// Prosba o wejscie z limitem czasu. ok == false oznacza timeout.
func enterWithTimeout(p, from Position, timeout time.Duration) (res EnterResult, ok bool) {
	req := enterRequest{from: from, reply: make(chan EnterResult)}
	timer := time.NewTimer(timeout)
	defer timer.Stop()
	select {
	case board[p.X][p.Y].enter <- req:
		return <-req.reply, true
	case <-timer.C:
		return Blocked, false
	}
}

func (t *Traveler) init(id int, seed int64, symbol rune) {
	t.rng = rand.New(rand.NewSource(seed))
	t.Id = id
	t.Symbol = symbol
	for {
		t.Position = randomPosition(t.rng)
		if isTrap[t.Position.X][t.Position.Y] {
			continue
		}
		res, ok := enterWithTimeout(t.Position, t.Position, time.Millisecond)
		if ok && res == Entered {
			break
		}
	}
	t.storeTrace()
	t.nrOfSteps = MinSteps + t.rng.Intn(MaxSteps-MinSteps+1)
}

func (t *Traveler) run(start <-chan struct{}) {
	<-start
	for step := 0; step < t.nrOfSteps; step++ {
		time.Sleep(t.randomDelay())
		next := t.randomNeighbour()
		res, ok := enterWithTimeout(next, t.Position, Timeout)
		if !ok {
			t.Symbol = unicode.ToLower(t.Symbol)
			t.storeTrace()
			break
		}
		if res == Blocked {
			continue // lokator nie mial dokad uciec: w nastepnym kroku nowy kierunek
		}
		prev := t.Position
		t.Position = next
		if res == Trapped {
			// wpadl w pulapke: mala litera, chwila snu na polu, znika
			t.Symbol = unicode.ToLower(t.Symbol)
			t.storeTrace()
			board[prev.X][prev.Y].leave <- struct{}{}
			time.Sleep(TrapSleep)
			t.Position = Hidden
			t.storeTrace()
			board[next.X][next.Y].leave <- struct{}{} // pulapka znow '#'
			break
		}
		t.storeTrace() // przed leave, zeby slad byl wczesniej niz kolejne wejscie
		board[prev.X][prev.Y].leave <- struct{}{}
	}
	reports <- t.traces
}

// Gorutyna, ktora w losowym momencie stawia lokatora na losowym wolnym
// polu. Dalej lokatorem zajmuja sie juz gorutyny pol.
func squatterSpawner(id int, seed int64, symbol rune, start <-chan struct{}) {
	rng := rand.New(rand.NewSource(seed))
	sq := &Squatter{Id: id, Symbol: symbol}
	<-start
	time.Sleep(time.Duration(rng.Int63n(int64(MaxAppearDelay))))

	for try := 0; try < MaxAppearTries; try++ {
		p := randomPosition(rng)
		for isTrap[p.X][p.Y] { // lokator nie startuje w pulapce
			p = randomPosition(rng)
		}
		life := MinSquatterLife + time.Duration(rng.Int63n(int64(MaxSquatterLife-MinSquatterLife)))
		sq.deathTime = time.Now().Add(life)
		req := arriveRequest{sq: sq, done: make(chan struct{})}
		select {
		case board[p.X][p.Y].arrive <- req:
			<-req.done
			return // od teraz lokatorem zajmuje sie pole
		default: // pole zajete, losujemy inne
		}
		time.Sleep(10 * time.Millisecond)
	}
	reports <- nil // pusty raport, zeby printer sie doliczyl
}

func main() {
	if len(os.Args) > 1 {
		n, err := strconv.Atoi(os.Args[1])
		if err != nil || n < 1 || n > BoardWidth*BoardHeight-NrOfTraps {
			fmt.Fprintf(os.Stderr, "Nr_Of_Travelers must be in 1..%d\n", BoardWidth*BoardHeight-NrOfTraps)
			os.Exit(1)
		}
		NrOfTravelers = n
	}

	// Id: podroznicy 0..k-1, potem dzicy lokatorzy, potem pulapki
	fmt.Printf("-1 %d %d %d\n", NrOfTravelers+NrOfSquatters+NrOfTraps, BoardWidth, BoardHeight)

	seed := time.Now().UnixNano()
	trapRng := rand.New(rand.NewSource(seed))
	for len(trapPositions) < NrOfTraps { // losowanie roznych pol na pulapki
		p := randomPosition(trapRng)
		if !isTrap[p.X][p.Y] {
			isTrap[p.X][p.Y] = true
			trapPositions = append(trapPositions, p)
		}
	}

	for x := range board {
		for y := range board[x] {
			board[x][y] = &Field{
				pos:       Position{x, y},
				enter:     make(chan enterRequest),
				leave:     make(chan struct{}),
				arrive:    make(chan arriveRequest),
				getTraces: make(chan chan []Trace),
				trapId:    -1,
			}
		}
	}
	for i, p := range trapPositions {
		f := board[p.X][p.Y]
		f.trapId = NrOfTravelers + NrOfSquatters + i
		f.storeTrapTrace() // pierwszy slad pulapki, przed startem podroznikow
	}
	for x := range board {
		for y := range board[x] {
			go board[x][y].run()
		}
	}

	done := make(chan struct{})
	start := make(chan struct{})
	go printer(NrOfTravelers+NrOfSquatters, done)

	travelers := make([]Traveler, NrOfTravelers)
	for i := range travelers {
		travelers[i].init(i, seed+int64(i)*7919, rune('A'+i%26))
	}

	for i := range travelers {
		go travelers[i].run(start)
	}
	for i := 0; i < NrOfSquatters; i++ {
		go squatterSpawner(NrOfTravelers+i, seed+int64(1000+i)*7919, rune('0'+i%10), start)
	}
	close(start)

	<-done
}
