package go.logic;


public class GameMechanics {
    public int blackCaptures = 0;
    public int whiteCaptures = 0;
    
    public boolean IsMovePossible(Board board, int x, int y, Stone color) {
        if (!board.isFieldOnBoard(x, y)) return false;
        if (board.getField(x, y) != Stone.EMPTY ) return false;
        board.setField(x, y, color);

        CheckCaptures(board, x, y, color);

        if (AreNeighboursFieldsFree(board, x, y)) {
            return true;
        }

        board.setField(x, y, Stone.EMPTY);
        return false;
    }

    private boolean AreNeighboursFieldsFree(Board board, int x, int y) {

        for (Direction d : Direction.values()) {
            int newX = x + d.getDx();
            int newY = y + d.getDy();

            if (board.isFieldOnBoard(newX, newY)) {
                if (board.getField(newX, newY) == Stone.EMPTY) {
                    return true;
                }
            }
        }
        return false;
    }

    private void CheckCaptures(Board board, int x, int y, Stone color) {
        for (Direction d : Direction.values()) {
            int newX = x + d.getDx();
            int newY = y + d.getDy();

            if (board.isFieldOnBoard(newX, newY) && board.getField(newX, newY) == color.opponent()) {
                if (!AreNeighboursFieldsFree(board, newX, newY)) {
                    board.setField(newX, newY, Stone.EMPTY);
                    if (color == Stone.BLACK) {
                        blackCaptures++;
                    } else {
                        whiteCaptures++;
                    }
                }
            }
        }
    }
}
