import java.util.Random;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.Semaphore;

public class zad3 {
    static final int NUM_PHILOSOPHERS = 5;
    static final int NUM_MEALS = 3;
    static Semaphore sem;

    static class Philosopher extends Thread {
        int id;
        ReentrantLock left, right;
        Random rand = new Random();

        Philosopher(int id, ReentrantLock left, ReentrantLock right) {
            this.id = id;
            this.left = left;
            this.right = right;
        }

        @Override
        public void run() {
            int meals = 0;
            try {
                while (meals < NUM_MEALS) {
                    System.out.println("Filozof " + id + " myśli.");
                    Thread.sleep(rand.nextInt(50));
                    
                    sem.acquire();
                    
                    left.lock();
                    try {
                        right.lock();
                        try {
                            System.out.println("Filozof " + id + " JE.");
                            Thread.sleep(rand.nextInt(50));
                            meals++;
                        } finally {
                            right.unlock();
                        }
                    } finally {
                        left.unlock();
                    }
                    
                    sem.release();
                }
                System.out.println("--> Filozof " + id + " zakończył posiłki.");
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public static void main(String[] args) {
        ReentrantLock[] forks = new ReentrantLock[NUM_PHILOSOPHERS];
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            forks[i] = new ReentrantLock(true);
        }
        
        sem = new Semaphore(NUM_PHILOSOPHERS - 1);

        Philosopher[] phils = new Philosopher[NUM_PHILOSOPHERS];
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            phils[i] = new Philosopher(i + 1, forks[i], forks[(i + 1) % NUM_PHILOSOPHERS]);
            phils[i].start();
        }

        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            try {
                phils[i].join();
            } catch (InterruptedException e) {}
        }
        System.out.println("Koniec symulacji. System był sprawiedliwy.");
    }
}