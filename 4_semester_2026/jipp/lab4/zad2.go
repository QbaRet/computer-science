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
	rnd := rand.New(rand.NewSource(time.Now().UnixNano() + int64(id)))

	for meals < NumMeals {
		fmt.Printf("Filozof %d myśli.\n", id)
		time.Sleep(time.Duration(rnd.Intn(100)) * time.Millisecond)

		sem <- struct{}{}

		<-left
		<-right

		fmt.Printf("Filozof %d JE.\n", id)
		time.Sleep(time.Duration(rnd.Intn(50)) * time.Millisecond)
		meals++

		left <- struct{}{}
		right <- struct{}{}

		<-sem
	}
	fmt.Printf("Filozof %d zakończył posiłki.\n", id)
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
	fmt.Println("Wszyscy filozofowie zjedli obiad")
}