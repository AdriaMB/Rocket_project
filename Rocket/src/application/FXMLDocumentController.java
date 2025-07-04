/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/javafx/FXMLController.java to edit this template
 */
package application;

import java.awt.Point;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.Scanner;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.chart.AreaChart;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tooltip;
import javafx.scene.image.Image;
import javafx.scene.input.MouseEvent;
import javafx.scene.shape.Line;
import javafx.stage.FileChooser;
import javafx.stage.Window;

/**
 * FXML Controller class
 *
 * @author adrim
 */
public class FXMLDocumentController implements Initializable {

    private File data; // file from where data will be extracted
    
    XYChart.Series seriesHeight = new XYChart.Series(); // XYChart.Series are necessary for depicting the data in the lineChart
    
    XYChart.Series seriesTemp = new XYChart.Series(); // XYChart.Series are necessary for depicting the data in the lineChart
    
    
    
    BooleanProperty fileLoaded = new SimpleBooleanProperty(false);
    

    
    @FXML
    private LineChart<Number, Number> heightChart; // Chart where the data will be depicted
    @FXML
    private LineChart<Number, Number> temperatureChart;
    
    @FXML
    private MenuItem openFileB; // Button used for choosing the File data
    @FXML
    private Label labelShowHeight; // This label will represent the values of the chart ( the point where you place the mouse )
    @FXML
    private Label labelShowTemperature;
    @FXML
    private NumberAxis yAxisH;
    @FXML
    private NumberAxis xAxisH;
    @FXML
    private Line xAxisLine;
    @FXML
    private Line yAxisLine;
    
    
    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        
        heightChart.setCreateSymbols(false);
        
   
        /**
        XYChart.Series s = new XYChart.Series();
        s.setName("July");
        s.getData().add(new XYChart.Data(1.0,4.0));
        s.getData().add(new XYChart.Data(3.0,10.0));
        s.getData().add(new XYChart.Data(6.0,15.0));
        s.getData().add(new XYChart.Data(9.0,8.0));
        s.getData().add(new XYChart.Data(12.0,5.0));
        s.getData().add(new XYChart.Data(15.0,18.0));
        s.getData().add(new XYChart.Data(18.0, 4.0));
        
        heightChart.getData().addAll(s);      // chart3.getData().addAll(s, a);
        **/
        
    }   
    
     @FXML
    private void loadDocument(ActionEvent event) throws Exception {
        // Helped by ChatGPT to allow to select an image from avatars folder
        // This method opens the file manager in the folder ROCKET_PROJECT from Github, where I can choose which file I want to open 
        
        File initialDir = new File( "/Users/adrim/Desktop/GitHub/Rocket_project"); // Opens the directory
        
        FileChooser file = new FileChooser(); // The FileChooser allows to select the file
        file.setTitle("Select data file");
        file.setInitialDirectory(initialDir);
        
        // Filter allowed file extensions
        file.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text Files", "*.txt")); // Only show the textFiles with .txt extension
        
        // The selected file will be saved in data
        data = file.showOpenDialog(new Window(){}); // shows the chooser (the FileChooser returns "the file" (probably a reference to it) that we chose)
        System.out.println(data.getPath()); // Just debugging
        
        
        //We call the method that will read and represent the data in the lineChart
        fillCharts();
    }
    
    
    /**
    // PRECONDITION: the File data was already chosen, and it can be read. Moreover, it follows a specific layout (TIME HEIGHT)
    private void fillCharts() throws Exception{
        // This method reads and represents on the LineChart the values from the data file
        
        Scanner scan = new Scanner(data); // Create the scanner for reading   
        
        // while there are still lines, we will store each individual element in an array( theArray ) for us to read. 
        while(scan.hasNext()){
            String nextLine = scan.nextLine(); // We take the next line
            String theArray[] = nextLine.split("\s"); // Split it according the the blank spaces ("\s") and store the values in an array
            float timeStamp = Float.parseFloat(theArray[0]); // We take the first value, which is the timeStamp
            
            // For each element on the line (we are considering only the case where the file only stores TIME - HEIGHT):
            for(int i = 1; i <  theArray.length; i++){
                float height = Float.parseFloat(theArray[1]); // We parse the value to a float
                seriesHeight.getData().add(  new XYChart.Data(  timeStamp, height  )  ); // We put that value in the height
                System.out.println(timeStamp + "  " + height); // Debugging
                
            }
        }
        seriesHeight.setName("Height (m)");
        
        heightChart.getData().addAll(seriesHeight); // We add the series of values to the lineChart (this actually represents the values)
        
    }
    */
    
    
    // PRECONDITION: the File data was already chosen, and it can be read. Moreover, it follows a specific layout (TIME HEIGHT TEMPERATURE)
    private void fillCharts() throws Exception{
        // This method reads and represents on the LineChart the values from the data file
        
        Scanner scan = new Scanner(data); // Create the scanner for reading   
        
        // while there are still lines, we will store each individual element in an array( theArray ) for us to read. 
        while(scan.hasNext()){
            String nextLine = scan.nextLine(); // We take the next line
            String theArray[] = nextLine.split("\s"); // Split it according the the blank spaces ("\s") and store the values in an array
            
            // [time, height, temperature]
            
            // We take the first value, which is the timeStamp
            float timeStamp = Float.parseFloat(theArray[0]); 
            //System.out.print(timeStamp + " "); // Debugging
            
            //We take the second value, the height and add it to the first series: the seriesHeight
            float height = Float.parseFloat(theArray[1]); // We parse the value to a float
            seriesHeight.getData().add(  new XYChart.Data(  timeStamp, height  )  ); // We put that value in the height
            //System.out.print(height + "  "); // Debugging
            
            //We take the second value, the height and add it to the first series: the seriesHeight
            float temp = Float.parseFloat(theArray[2]); // We parse the value to a float
            seriesTemp.getData().add(  new XYChart.Data(  timeStamp, temp  )  ); // We put that value in the height
            //System.out.print(temp + "  "); // Debugging
            
            //System.out.println(); // Just for debugging: We introduce a \n for jumping to the next line
            
            
        }
        seriesHeight.setName("Height (m)");
        seriesTemp.setName("Temperature (ºC)");
        
        heightChart.getData().addAll(seriesHeight); // We add the series of values to the lineChart (this actually represents the values)
        temperatureChart.getData().addAll(seriesTemp);
        
        fileLoaded.set(true);
        
    }

    @FXML
    private void takeHeightData(MouseEvent event) {
        
        /**
        double xInChart = event.getSceneX();
        double yInChart = event.getSceneY();
        
        System.out.println("X: " + xInChart + "    Y: " + yInChart);
        */
        
        if(fileLoaded.get()){
             
            double mouseX = event.getX();
            double mouseY = event.getY();

            
            
            
            // Convert mouseX to chart X value
            NumberAxis xAxis = (NumberAxis) heightChart.getXAxis();
            double xValue = xAxis.getValueForDisplay(mouseX).doubleValue();

            // Interpolate Y value from data
            XYChart.Series<Number, Number> series = heightChart.getData().get(0);
            double interpolatedY = interpolateY(series, xValue);

            System.out.println("Mouse over X: " + xValue + ", interpolated Y: " + interpolatedY);
            
            xAxisLine.setLayoutX(mouseX);
            yAxisLine.setLayoutY(mouseY);
           


        }
        
    }
    
    private double interpolateY(XYChart.Series<Number, Number> series, double x) {
    for (int i = 0; i < series.getData().size() - 1; i++) {
        double x1 = series.getData().get(i).getXValue().doubleValue();
        double x2 = series.getData().get(i + 1).getXValue().doubleValue();

        if (x >= x1 && x <= x2) {
            double y1 = series.getData().get(i).getYValue().doubleValue();
            double y2 = series.getData().get(i + 1).getYValue().doubleValue();

            // Linear interpolation
            return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
        }
    }
    return Double.NaN; // Out of bounds
}

    
}
