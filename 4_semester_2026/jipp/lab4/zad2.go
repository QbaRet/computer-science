package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

const NumPhilosophers = 5
const NumMeals = 3

func philosopher(id int, left, right *sync.Mutex, wg *sync.WaitGroup) {
	defer wg.Done()
	meals := 0
	fails := 0

	for meals < NumMeals {
		fmt.Printf("Filozof %d myśli.\n", id)
		time.Sleep(time.Duration(rand.Intn(50)) * time.Millisecond)

		// Próba podniesienia lewego widelca
		if left.TryLock() {
			// Próba podniesienia prawego widelca
			if right.TryLock() {
				fmt.Printf("Filozof %d JE.\n", id)
				time.Sleep(time.Duration(rand.Intn(50)) * time.Millisecond)
				meals++
				right.Unlock()
				left.Unlock()
			} else {
				// Porażka - prawy zajęty, odkładamy lewy
				left.Unlock()
				fails++
			}
		} else {
			// Porażka - lewy zajęty
			fails++
		}
	}
	fmt.Printf("--> Filozof %d zakończył. Nieudane próby zjedzenia: %d\n", id, fails)
}

func main() {
	var forks [NumPhilosophers]sync.Mutex
	var wg sync.WaitGroup

	rand.Seed(time.Now().UnixNano())

	for i := 0; i < NumPhilosophers; i++ {
		wg.Add(1)
		// Przekazujemy wskaźniki na widelce. Używamy modulo, aby zamknąć stół w koło.
		go philosopher(i+1, &forks[i], &forks[(i+1)%NumPhilosophers], &wg)
	}

	wg.Wait()
	fmt.Println("Koniec symulacji.")
}