package go.ui.fx;

import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.layout.VBox;
// panel boczny, zawiera historie gry i przyciski, mozna dodac chat miedzy graczami
public class SidePanel extends VBox {
    private final TextArea logArea=new TextArea();
    private final Button passBtn=new Button("Pass");
    private final Button surrenderBtn=new Button("Surrender");
    public SidePanel(){
        // ustawienia panelu bocznego
        logArea.setEditable(false);
        logArea.setPrefHeight(400);
        this.setSpacing(10);
        this.getChildren().addAll(new Label("Akcje: "), logArea, passBtn, surrenderBtn);
    }
    // metoda do dodawania wpisow do logu
    public void addLog(String message){
        logArea.appendText(message + "\n");
    }
    // gettery do przyciskow
    public Button getPassBtn(){
        return passBtn;}
    public Button getSurrenderBtn(){
        return surrenderBtn;}
}
