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
        SynchronousQueue<Message> serverQueue = new SynchronousQueue<>();
        SynchronousQueue<Integer>[] userQueues = new SynchronousQueue[N];
        
        for (int i = 0; i < N; i++) {
            userQueues[i] = new SynchronousQueue<>();
        }

        Thread server = new Thread(() -> {
            try {
                for (int i = 0; i < N * M; i++) {
                    Message msg = serverQueue.take();
                    userQueues[msg.target].put(msg.sender);
                }
                for (int i = 0; i < N; i++) {
                    userQueues[i].put(-1);
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        });

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
                        Integer incoming = userQueues[id].poll(); 
                        if (incoming != null) {
                            if (incoming == -1) {
                                active = false;
                                break;
                            }
                            received++;
                            System.out.println("Użytkownik " + id + " <- OTRZYMAŁ od " + incoming);
                            continue; 
                        }

                        if (sent < M) {
                            int target = rand.nextInt(N);
                            if (serverQueue.offer(new Message(id, target))) {
                                sent++;
                                System.out.println("Użytkownik " + id + " -> WYSŁAŁ do " + target);
                            } else {
                                Thread.sleep(1);
                            }
                        } else {
                            incoming = userQueues[id].take(); 
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

        for (int i = 0; i < N; i++) {
            users[i].join();
        }
        server.join();
        System.out.println("Symulacja zakończona.");
    }
}