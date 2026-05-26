package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

const N = 5 
const M = 3 

type Message struct {
	sender int
	target int
}

func server(serverChan chan Message, userChans []chan int) {
	for i := 0; i < N*M; i++ {
		msg := <-serverChan
		userChans[msg.target] <- msg.sender 
	}
	for i := 0; i < N; i++ {
		close(userChans[i])
	}
}

func user(id int, serverChan chan Message, myChan chan int, wg *sync.WaitGroup) {
	defer wg.Done()
	sent := 0
	received := 0
	active := true

	for active {
		if sent < M {
			target := rand.Intn(N)
			select {
			case serverChan <- Message{sender: id, target: target}:
				fmt.Printf("Użytkownik %d -> WYSŁAŁ do %d\n", id, target)
				sent++
			case sender, ok := <-myChan:
				if !ok {
					active = false
					continue
				}
				fmt.Printf("Użytkownik %d <- OTRZYMAŁ od %d\n", id, sender)
				received++
			}
		} else {
			sender, ok := <-myChan
			if !ok {
				active = false
				continue
			}
			fmt.Printf("Użytkownik %d <- OTRZYMAŁ od %d\n", id, sender)
			received++
		}
	}
	fmt.Printf("=== Użytkownik %d odebrał łącznie: %d komunikatów ===\n", id, received)
}

func main() {
	rand.Seed(time.Now().UnixNano())

	serverChan := make(chan Message) 
	userChans := make([]chan int, N)
	for i := range userChans {
		userChans[i] = make(chan int) 
	}

	var wg sync.WaitGroup

	go server(serverChan, userChans)

	for i := 0; i < N; i++ {
		wg.Add(1)
		go user(i, serverChan, userChans[i], &wg)
	}

	wg.Wait()
	fmt.Println("Symulacja zakończona.")
}