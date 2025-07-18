/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/javafx/FXMLController.java to edit this template

    THIS CODE WILL CONTROL AN INDIVIDUAL CHART. THE IDEA IS TO LAUNCH IT SEVERAL TIMES, HAVING AT THE SAME TIME SEVERAL WINDOWS WITH
    1 CHART ON IT. EACH WINDOW (FXMLDocument.fxml) WILL BE RESIZABLE, SO THAT THE CHART WILL REDIMENSIONATE ITSELF TO ACCOMODATE TO THE
    WINDOW'S SIZE. THUS, THE USER CAN CHOOSE THE WINDOW SIZE IT WANTS. MOREOVER, THE GREATER THE WINDOW, THE MORE DETAIL THERE IS (I am 
    quite sure it works that way)

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
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Point2D;
import javafx.scene.Node;
import javafx.scene.chart.AreaChart;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.chart.XYChart.Data;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tooltip;
import javafx.scene.image.Image;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
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
    
    private int m;
    
    private String name;
    
    private String axisName;
    
    private float max = 0;
    
    XYChart.Series series = new XYChart.Series(); // XYChart.Series are necessary for depicting the data in the lineChart
    
    BooleanProperty fileLoaded = new SimpleBooleanProperty(false);
    
    @FXML
    private LineChart<Number, Number> chart; // Chart where the data will be depicted
    @FXML
    private NumberAxis yAxis;
    @FXML
    private NumberAxis xAxis;
    @FXML
    private Line xAxisLine;
    @FXML
    private Line yAxisLine;
    
    @FXML
    private Pane pane;
    
    
    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        
        chart.setCreateSymbols(true);
        
        pane.visibleProperty().bind(fileLoaded); // If a file is loaded, the boolean will be true and the red axis will show up
   
        pane.setMouseTransparent(true);
        xAxisLine.setMouseTransparent(true);
        yAxisLine.setMouseTransparent(true);


        
    }   
    
    public void initChart( File f, int measure) throws Exception{
        //Method that fills the chart with the data according to measure
        //POSSIBLE VALUES OF MEASURE:
        //  1 == height
        //  2 == temperature
        //  3 == pressure 
        data = f;
        m = measure;
        
        //POSSIBLE VALUES OF MEASURE( m ):
            //  0 == time (MEASURE WON'T BE THIS VALUE)
            //  1 == height (blue)
            //  2 == temperature (red)
            //  3 == pressure (green)
            //  4 == acceleration (yellow)
            //  5 == velocity (purple)
        
        switch(m){
            case 1:
                name = "Height";
                axisName = "m";
                
                break;
            case 2:
                name = "Temperature";
                axisName = "ºC";
                break;
            case 3:
                name = "Pressure";
                axisName = "hPa";
                break;
            case 4:
                name = "Acceleration";
                axisName = "ms/2";
                break;
            case 5:
                name = "Velocity";
                axisName = "m/s";
                break;
            default:
                System.out.println("CANNOT EXIST THIS OPTION");
                name = "NaN";
                axisName = "There is no measure";
                break;
        }
        
        fillCharts();
    }
   
    // PRECONDITION: the File data was already chosen, and it can be read. Moreover, it follows a specific layout (TIME HEIGHT TEMPERATURE)
    private void fillCharts() throws Exception{
        // This method reads and represents on the LineChart the values from the data file
        int i = 0;
        Scanner scan = new Scanner(data); // Create the scanner for reading   
        
        //There is one extra line: Time(ms), Altitude(m), Temperature(C), Pressure(hPa)
        System.out.println(scan.nextLine());
        
        // while there are still lines, we will store each individual element in an array( theArray ) for us to read. 
        while(scan.hasNext()){
            String nextLine = scan.nextLine(); // We take the next line
            String theArray[] = nextLine.split(","); // Split it according the the blank spaces ("\s") and store the values in an array
            
            // [time, height, temperature]
            
            // We take the first value, which is the timeStamp
            float timeStamp = Float.parseFloat(theArray[0]); 
            //System.out.print(timeStamp + " "); // Debugging
            
            //We take the second value, the height and add it to the first series: the seriesHeight
            //POSSIBLE VALUES OF MEASURE( m ):
            //  0 == time (MEASURE WON'T BE THIS VALUE)
            //  1 == height (blue)
            //  2 == temperature (red)
            //  3 == pressure (green)
            //  4 == acceleration (yellow)
            //  5 == velocity (purple)
            float dataForThisChart = Float.parseFloat(theArray[m]); // We parse the value to a float
            XYChart.Data<Number, Number> aux = new XYChart.Data(  timeStamp, dataForThisChart  );
            
            series.getData().add( aux ); // We put that value in the series
            
            System.out.print(name + ":  " + dataForThisChart + "  " + axisName); // Debugging
            
            chart.setTitle(name);
            yAxis.setLabel(name + " (" + axisName + ")");
            
            if(max < dataForThisChart){max = dataForThisChart;}
            
            
            //System.out.println(); // Just for debugging: We introduce a \n for jumping to the next line
            
            i++;
        }       
        series.setName(name);
       
        chart.getData().addAll(series); // We add the series of values to the lineChart (this actually represents the values
        
        // Tooltips
        ObservableList<Data<Number,Number>> aux = series.getData();
        for(XYChart.Data<Number, Number> data : aux){
            Tooltip tp = new Tooltip(name + ": " + data.getYValue() + " " + axisName +"\nTime: " + data.getXValue() + " ms");
            Tooltip.install(data.getNode(), tp);
        }
        
        // Change the color of the line according to the measure
        Node line = series.getNode();
        
        switch(m){
            case 1://  1 == height (blue)
                line.setStyle("-fx-stroke: cornflowerblue");
                break;
            case 2://  2 == temperature (red)
                line.setStyle("-fx-stroke: crimson");
                break;
            case 3://  3 == pressure (green)
                line.setStyle("-fx-stroke: lightgreen");
                break;
            case 4://  4 == acceleration (yellow)
                line.setStyle("-fx-stroke: yellow");
                break;
            case 5://  5 == velocity (purple)
                line.setStyle("-fx-stroke: mediumpurple");
                break;
            default:
                System.out.println("CANNOT EXIST THIS OPTION");
                break;
        }
        
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
            NumberAxis xAxis = (NumberAxis) chart.getXAxis();
            double xValue = xAxis.getValueForDisplay(mouseX).doubleValue();

            // Interpolate Y value from data
            XYChart.Series<Number, Number> series = chart.getData().get(0);
            double interpolatedY = interpolateY(series, xValue);

            //System.out.println("Mouse over X: " + xValue + ", interpolated Y: " + interpolatedY);
            
            double limitXAxisLine = Math.max(100, mouseX);
            double limitYAxisLine = Math.min(300, mouseY);
            
            xAxisLine.setLayoutX(mouseX);
            yAxisLine.setLayoutY(mouseY);
            
            
           


        }
        
    }
    
    @FXML
    private void mouseOverAxis(MouseEvent event){
        
        
        Point2D localCoords = xAxis.sceneToLocal(event.getSceneX(), event.getSceneY());
        //System.out.println("Mouse in NumberAxis: " + localCoords.getX() + ", " + localCoords.getY());

        Number dataValue = xAxis.getValueForDisplay(event.getSceneX());
        System.out.println("Value at pixel: " + dataValue);

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
