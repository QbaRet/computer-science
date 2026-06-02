package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

const (
	NumPhilosophers = 5
	NumMeals        = 3
)

func philosopher(id int, left, right chan struct{}, sem chan struct{}, wg *sync.WaitGroup) {
	defer wg.Done()
	meals := 0
	fails := 0
	rnd := rand.New(rand.NewSource(time.Now().UnixNano() + int64(id)))

	for meals < NumMeals {
		fmt.Printf("Filozof %d myśli\n", id)
		time.Sleep(time.Duration(rnd.Intn(100)) * time.Millisecond)

		sem <- struct{}{}

		select {
		case <-left:
			select {
			case <-right:
				fmt.Printf("Filozof %d je\n", id)
				time.Sleep(time.Duration(rnd.Intn(50)) * time.Millisecond)
				meals++

				right <- struct{}{}
				left <- struct{}{}
			case <-time.After(10 * time.Millisecond):
				left <- struct{}{}
				fails++
			}
		case <-time.After(10 * time.Millisecond):
			fails++
		}

		<-sem
	}
	fmt.Printf("Filozof %d zakończył posiłki. Nieudane próby zjedzenia: %d\n", id, fails)
}

func main() {
	forks := make([]chan struct{}, NumPhilosophers)
	for i := 0; i < NumPhilosophers; i++ {
		forks[i] = make(chan struct{}, 1)
		forks[i] <- struct{}{}
	}

	sem := make(chan struct{}, NumPhilosophers-1)
	var wg sync.WaitGroup

	for i := 0; i < NumPhilosophers; i++ {
		wg.Add(1)
		go philosopher(i+1, forks[i], forks[(i+1)%NumPhilosophers], sem, &wg)
	}

	wg.Wait()
}
