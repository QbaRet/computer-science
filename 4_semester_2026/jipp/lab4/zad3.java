import java.util.Random;
import java.util.concurrent.locks.ReentrantLock;

public class zad3 {
    static final int NUM_PHILOSOPHERS = 5;
    static final int NUM_MEALS = 3;

    static class Philosopher extends Thread {
        int id;
        ReentrantLock left, right;
        int fails = 0;
        Random rand = new Random();

        public Philosopher(int id, ReentrantLock left, ReentrantLock right) {
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

                    // Próba podniesienia lewego widelca
                    if (left.tryLock()) {
                        try {
                            // Próba podniesienia prawego widelca
                            if (right.tryLock()) {
                                try {
                                    System.out.println("Filozof " + id + " JE.");
                                    Thread.sleep(rand.nextInt(50));
                                    meals++;
                                } finally {
                                    right.unlock();
                                }
                            } else {
                                fails++;
                            }
                        } finally {
                            left.unlock();
                        }
                    } else {
                        fails++;
                    }
                }
                System.out.println("--> Filozof " + id + " zakończył. Nieudane próby zjedzenia: " + fails);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public static void main(String[] args) {
        ReentrantLock[] forks = new ReentrantLock[NUM_PHILOSOPHERS];
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            forks[i] = new ReentrantLock();
        }

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
        System.out.println("Koniec symulacji.");
    }
}