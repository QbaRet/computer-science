package go.logic;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class Protocol {
    public static final int Port=8000;
    public static final int Player1=1;
    public static final int Player2=2;
    public static final int MOVE=10;
    public static final int PASS=11;
    public static final int SURRENDER=12;
    public static final int QUIT=13;
    public static final int GAME_OVER=14;
    public static final int INVALID_MOVE=15;
    public static final int BOARD_STATE=16;
    public static final int CAPTURES=17;

    public static void sendBoard(Board board, DataOutputStream out) throws IOException {
        out.writeInt(board.getSize());

        for (int x = 0; x < board.getSize(); x++) {
            for (int y = 0; y < board.getSize(); y++) {
                Stone stone = board.getField(x, y);
                out.writeInt(stone.ordinal());
            }
        }
    }


    public static void receiveBoard(Board board, DataInputStream in) throws IOException {
        int size = in.readInt();

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                int ordinal = in.readInt();
                Stone color = Stone.values()[ordinal];
                board.setField(x, y, color);
            }
        }
    }
}
