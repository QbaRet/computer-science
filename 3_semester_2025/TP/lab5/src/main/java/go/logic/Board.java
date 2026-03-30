package go.logic;

// klasa Board tworzy początkowy stan planszy oraz wykonuje zmiany podczas rozgrywki, przechowując jednocześnie obecny stan planszy

public class Board {
    private final int size;
    public final Stone[][] fields;

    public Board(int size) {
        this.size = size;
        this.fields = new Stone[size][size];

        for (int i = 0; i<size; i++) {
            for (int j = 0; j < size; j++) {
                fields[i][j] = Stone.EMPTY;
            }
        }
    }

    public int getSize() {
        return size;
    }


    // metoda zmieniająca wskazane pole na planszy na wartość przekazaną jako argument
    public void setField(int wspX, int wspY, Stone stone) {
        if (!isFieldOnBoard(wspX, wspY)) {
            throw new IllegalArgumentException("Podane pole nie nalezy do planszy!");
        }
        fields[wspX][wspY] = stone;
    }


    //funkcja zwracająca wartość pola wskazanego w argumencie metody
    public Stone getField(int wspX, int wspY) {
        if (!isFieldOnBoard(wspX, wspY)) {
            throw new IllegalArgumentException("Podane pole nie nalezy do planszy!");
        }
        return fields[wspX][wspY];
    }


    //funkcja pomocnicza określająca, czy wskazane pole nalezy do planszy
    public boolean isFieldOnBoard(int x, int y) {
        return x >= 0 && x < size && y >= 0 && y < size;
    }

    @Override
    public boolean equals(Object o) {
        // 1. Sprawdzenie czy to ten sam obiekt w pamięci
        if (this == o) return true;

        // 2. Sprawdzenie czy obiekt nie jest nullem i czy jest tą samą klasą
        if (o == null || getClass() != o.getClass()) return false;

        Board board = (Board) o;

        // 4. Porównanie rozmiaru (szybkie sprawdzenie)
        if (size != board.size) return false;

        // 5. KLUCZOWE: Porównanie zawartości tablicy dwuwymiarowej
        return java.util.Arrays.deepEquals(this.fields, board.fields);
    }

    @Override
    public int hashCode() {
        // Generujemy unikalny skrót na podstawie rozmiaru i zawartości tablicy
        int result = java.util.Objects.hash(size);
        result = 31 * result + java.util.Arrays.deepHashCode(fields);
        return result;
    }

    public void copyBoard(Board destinationBoard) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                destinationBoard.fields[i][j] = this.fields[i][j];
            }
        }
    }
}
