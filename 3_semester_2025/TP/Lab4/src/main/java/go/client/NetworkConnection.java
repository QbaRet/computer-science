package go.client;

import go.logic.Board;
import go.logic.Protocol;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

import static javax.management.remote.JMXConnectorFactory.connect;

public class NetworkConnection {
    private Socket socket;
    private DataInputStream fromServer;
    private DataOutputStream toServer;

    public NetworkConnection() {
    }

    public void connect() throws IOException {
        socket = new Socket("localhost", Protocol.Port);
        fromServer = new DataInputStream(socket.getInputStream());
        toServer = new DataOutputStream(socket.getOutputStream());
    }

    public int getPlayerId() throws IOException {
        return fromServer.readInt();
    }

    public void waitForSecondPlayer() throws IOException {
        fromServer.readInt();
    }

    public void sendPassMessage() throws IOException {
        toServer.writeInt(Protocol.PASS);
        toServer.flush();
    }

    public void sendSurrenderMessage() throws IOException {
        toServer.writeInt(Protocol.SURRENDER);
        toServer.flush();
    }

    public void sendQuitMessage() throws IOException {
        toServer.writeInt(Protocol.QUIT);
        toServer.flush();
    }

    public void sendSpaceInformation(int[] coordinates) throws IOException {
        toServer.writeInt(Protocol.MOVE);
        toServer.writeInt(coordinates[0]);
        toServer.writeInt(coordinates[1]);
        toServer.flush();
    }

    public int readMessage() throws IOException {
        return fromServer.readInt();
    }

    public void receiveBoardFromServer(Board board) throws IOException {
        Protocol.receiveBoard(board, fromServer);
    }

    public int[] getCaptures() throws IOException {
        int black = fromServer.readInt();
        int white = fromServer.readInt();

        return new int[]{black, white};
    }

    public int[] getCoordinates() throws IOException {
        int x = fromServer.readInt();
        int y = fromServer.readInt();

        return new int[]{x, y};
    }
}
