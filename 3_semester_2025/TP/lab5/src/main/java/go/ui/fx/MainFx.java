package go.ui.fx;

import go.client.GoClient;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;
// glowna klasa uruchamiajaca aplikacje JavaFX
public class MainFx extends Application{
    @Override
    public void start(Stage stage){
        // tworzenie komponentow GUI
        BoardCanvas canvas=new BoardCanvas();
        canvas.setWidth(700);
        canvas.setHeight(700);
        SidePanel side=new SidePanel();
        GUIView guiView=new GUIView(canvas, side);
        // uruchomienie klienta w osobnym watku
        Thread clientThread=new Thread( ()-> {
            GoClient client=new GoClient(guiView);
            client.connect();
        });
        // ustawienie watku tak, by zamykal sie wraz z zamknieciem aplikacji
        clientThread.setDaemon(true);
        clientThread.start();
        BorderPane root=new BorderPane();
        // rozmieszczenie komponentow w oknie
        root.setCenter(canvas);
        root.setRight(side);
        stage.setScene(new Scene(root));
        stage.setTitle("Gra w Go");
        stage.show();
    }
}
