import java.util.Random;
import java.util.concurrent.SynchronousQueue;

class Message {
    int sender;
    int target;
    Message(int sender, int target) {
        this.sender = sender;
        this.target = target;
    }
}

public class zad6 {
    static final int N = 5;
    static final int M = 3;

    public static void main(String[] args) throws InterruptedException {
        // SynchronousQueue działa jak rura - element wejdzie, dopiero gdy ktoś go odbierze (0 bufora)
        SynchronousQueue<Message> serverQueue = new SynchronousQueue<>();
        SynchronousQueue<Integer>[] userQueues = new SynchronousQueue[N];
        
        for (int i = 0; i < N; i++) {
            userQueues[i] = new SynchronousQueue<>();
        }

        // Definicja serwera
        Thread server = new Thread(() -> {
            try {
                for (int i = 0; i < N * M; i++) {
                    Message msg = serverQueue.take();
                    userQueues[msg.target].put(msg.sender);
                }
                // Poison pill - wysyłamy -1, by zakończyć działanie wątków
                for (int i = 0; i < N; i++) {
                    userQueues[i].put(-1);
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        });

        // Definicja użytkowników
        Thread[] users = new Thread[N];
        for (int i = 0; i < N; i++) {
            final int id = i;
            users[i] = new Thread(() -> {
                int sent = 0;
                int received = 0;
                boolean active = true;
                Random rand = new Random();

                try {
                    while (active) {
                        // Krok 1: Sprawdź, czy serwer nie próbuje nam czegoś wcisnąć
                        Integer incoming = userQueues[id].poll(); // Nie blokuje
                        if (incoming != null) {
                            if (incoming == -1) {
                                active = false;
                                break;
                            }
                            received++;
                            System.out.println("Użytkownik " + id + " <- OTRZYMAŁ od " + incoming);
                            continue; // Wymuszamy nowy cykl (priorytetyzujemy odbiór)
                        }

                        // Krok 2: Jeśli my chcemy coś wysłać
                        if (sent < M) {
                            int target = rand.nextInt(N);
                            // offer nie blokuje wątku, zwraca false, jeśli serwer zajęty
                            if (serverQueue.offer(new Message(id, target))) {
                                sent++;
                                System.out.println("Użytkownik " + id + " -> WYSŁAŁ do " + target);
                            } else {
                                // Odciążamy procesor, czekając chwilę przed kolejną próbą
                                Thread.sleep(1);
                            }
                        } else {
                            // Skończyliśmy wysyłać, więc możemy zacząć bezpiecznie oczekiwać z blokadą
                            incoming = userQueues[id].take(); // Blokuje (jest to teraz bezpieczne)
                            if (incoming == -1) {
                                active = false;
                                break;
                            }
                            received++;
                            System.out.println("Użytkownik " + id + " <- OTRZYMAŁ od " + incoming);
                        }
                    }
                    System.out.println("=== Użytkownik " + id + " odebrał łącznie: " + received + " komunikatów ===");
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            });
            users[i].start();
        }

        server.start();

        // Oczekiwanie na zakończenie
        for (int i = 0; i < N; i++) {
            users[i].join();
        }
        server.join();
        System.out.println("Symulacja zakończona.");
    }
}