package go.logic;

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

    public void setField(int wspX, int wspY, Stone stone) {
        if (!isFieldOnBoard(wspX, wspY)) {
            throw new IllegalArgumentException("Podane pole nie nalezy do planszy!");
        }
        fields[wspX][wspY] = stone;
    }

    public Stone getField(int wspX, int wspY) {
        if (!isFieldOnBoard(wspX, wspY)) {
            throw new IllegalArgumentException("Podane pole nie nalezy do planszy!");
        }
        return fields[wspX][wspY];
    }

    public boolean isFieldOnBoard(int x, int y) {
        return x >= 0 && x < size && y >= 0 && y < size;
    }
}
