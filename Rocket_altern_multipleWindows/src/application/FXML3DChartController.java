/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/javafx/FXMLController.java to edit this template
 */
package application;

import java.io.File;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.layout.BorderPane;
import javafx.scene.web.WebView;



/**
 * FXML Controller class
 *
 * @author adrim
 */
public class FXML3DChartController implements Initializable {

    
    private File data; // file from where data will be extracted
    
    private BorderPane borderPane;
    
    
    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        // TODO
    }    
    
    
    public void initChart3D( File f ) throws Exception{
        //Method that fills the chart with the data according to measure
        //POSSIBLE VALUES OF MEASURE:
        //  1 == height
        //  2 == temperature
        //  3 == pressure 
        data = f;
        
        WebView webView = new WebView();
        webView.getEngine().load("https://www.wolframcloud.com/env/4135eb87-42fa-4f01-90e8-4d5549dcfcc2");
        borderPane.setCenter(webView);
        
    }
    
    
}
