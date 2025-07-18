/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package application;

import java.io.File;
import java.io.IOException;
import java.util.Scanner;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Parent;
import javafx.scene.PerspectiveCamera;
import javafx.scene.Scene;
import javafx.scene.chart.XYChart;
import javafx.scene.image.Image;
import javafx.scene.paint.Color;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.stage.Window;
import javafx.stage.WindowEvent;

///// NOT USED ///////////////////////77
//3D chart //////////////////////////////////////////////////////
import javafx.scene.paint.PhongMaterial;
import javafx.scene.shape.Box;
////////////////////////////////////////////////////////////////////////////

/**
 *
 * @author adrim
 */
public class mainRocket extends Application{

    private File data; // file from where data will be extracted
    
    ///////////// NOT USED ///////////////////////////////////////////////////
    // 3D chart ///////////////////////////////////////////////////////////
    private static final double AXIS_LENGTH = 250.0;
    final Group root = new Group();
    final Xform axisGroup = new Xform();
    final Xform world = new Xform();
    
    final PerspectiveCamera camera = new PerspectiveCamera(true);
    final Xform cameraXform = new Xform();
    final Xform cameraXform2 = new Xform();
    final Xform cameraXform3 = new Xform();
    private static final double CAMERA_INITIAL_DISTANCE = -450;
    private static final double CAMERA_INITIAL_X_ANGLE = 70.0;
    private static final double CAMERA_INITIAL_Y_ANGLE = 320.0;
    private static final double CAMERA_NEAR_CLIP = 0.1;
    private static final double CAMERA_FAR_CLIP = 10000.0;
    
    
    
    //////////////////////////////////////////////////////////////////////////////
    @Override
    public void start(Stage stage) throws Exception {
        /**
        Parent root = FXMLLoader.load(getClass().getResource("/application/FXMLDocument.fxml"));
        Scene scene = new Scene(root);
        stage.setTitle("Flight data analysis");
        stage.setScene(scene);
        */
        Scene scene = new Scene(root);
        loadDocument(); // loads the document. After this method, data should be the chosen document
        
        createWindow(1);
        createWindow(2);
        createWindow(3);
        create3DWindow(4);
        
        buildCamera();
        buildAxes();
        
        
        scene.setCamera(camera);
        
    }
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);
    }
    
    
    private void loadDocument() throws Exception {
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
        
        
    }
    
    
    /*
     // PRECONDITION: the File data was already chosen, and it can be read. Moreover, it follows a specific layout (TIME HEIGHT TEMPERATURE)
    private void fillCharts() throws Exception{
        // This method reads and represents on the LineChart the values from the data file
        
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
            float height = Float.parseFloat(theArray[1]); // We parse the value to a float
            seriesHeight.getData().add(  new XYChart.Data(  timeStamp, height  )  ); // We put that value in the height
            //System.out.print(height + "  "); // Debugging
            
            //We take the second value, the height and add it to the first series: the seriesHeight
            float temp = Float.parseFloat(theArray[2]); // We parse the value to a float
            seriesTemp.getData().add(  new XYChart.Data(  timeStamp, temp  )  ); // We put that value in the height
            //System.out.print(temp + "  "); // Debugging
            
            //We take the second value, the height and add it to the first series: the seriesHeight
            float pressure = Float.parseFloat(theArray[3]); // We parse the value to a float
            seriesPressure.getData().add(  new XYChart.Data(  timeStamp, pressure  )  ); // We put that value in the height
            
            
            //System.out.print(temp + "  "); // Debugging
            
            
            
            //System.out.println(); // Just for debugging: We introduce a \n for jumping to the next line
            
            
        }
        seriesHeight.setName("Height (m)");
        seriesTemp.setName("Temperature (ºC)");
        
        heightChart.getData().addAll(seriesHeight); // We add the series of values to the lineChart (this actually represents the values)
        temperatureChart.getData().addAll(seriesTemp);
        
        fileLoaded.set(true);
        
    }
    
    */
    
    private void createWindow(int measure) throws Exception{
        //This method will load a window that will represent the specified measure
        //The int measure will be the position in the array that the chart code will read for representing. 
        
        
         FXMLLoader miCargador = new 
                    FXMLLoader(getClass().getResource("/application/FXMLDocument.fxml"));
        
        //Parent: base class for all nodes that have childern in the scene graph. It handels all hierachichal scene graph operations: adding, removing...
        Parent root = miCargador.load();
        
        // The code initChart() receives as arguments the reference to the file that we opened and an int measure that will reflect the
        // position of the array that we will access. This array will be generated when reading the file: each line is parsed according
        // to the commas. 
        // EX: If the measure == 1, we are saying that this chart will measure the height (because that position 
        // of the height in the array is the postion 1. REMEMBER THAT POSITION 0 IS THE TIME)
        FXMLDocumentController controlador2 = miCargador.getController();
        controlador2.initChart(data, measure);
        
        //Scene: container for all content in a scene graph
        Scene scene = new Scene(root, 500, 300);
        Stage stage = new Stage();
        stage.setScene(scene);
        
   
        stage.show(); // inicia un segundo hilo de eventos anidado con el primero
        // para obtener el valor modificado en la ventana emergente...        
    }

    private void create3DWindow(int measure) throws Exception{
        //This method will load a window that will represent the specified measure
        //The int measure will be the position in the array that the chart code will read for representing. 
        
        
         FXMLLoader miCargador = new 
                    FXMLLoader(getClass().getResource("/application/FXML3DChart.fxml"));
        
        //Parent: base class for all nodes that have childern in the scene graph. It handels all hierachichal scene graph operations: adding, removing...
        Parent root = miCargador.load();
        
        // The code initChart() receives as arguments the reference to the file that we opened and an int measure that will reflect the
        // position of the array that we will access. This array will be generated when reading the file: each line is parsed according
        // to the commas. 
        // EX: If the measure == 1, we are saying that this chart will measure the height (because that position 
        // of the height in the array is the postion 1. REMEMBER THAT POSITION 0 IS THE TIME)
        FXML3DChartController controlador = miCargador.getController();
        controlador.initChart3D(data);
        
        //Scene: container for all content in a scene graph
        Scene scene = new Scene(root, 500, 300);
        Stage stage = new Stage();
        stage.setScene(scene);
        
   
        stage.show(); // inicia un segundo hilo de eventos anidado con el primero
        // para obtener el valor modificado en la ventana emergente...        
    }
  
    
//////////////////////////////////// NOT USED ////////////////////////////////
/////// 3D charts /////////////////////////////////////////////////////////////////
    
      private void buildCamera() {
        root.getChildren().add(cameraXform);
        cameraXform.getChildren().add(cameraXform2);
        cameraXform2.getChildren().add(cameraXform3);
        cameraXform3.getChildren().add(camera);
        cameraXform3.setRotateZ(180.0);
 
        camera.setNearClip(CAMERA_NEAR_CLIP);
        camera.setFarClip(CAMERA_FAR_CLIP);
        camera.setTranslateZ(CAMERA_INITIAL_DISTANCE);
        cameraXform.ry.setAngle(CAMERA_INITIAL_Y_ANGLE);
        cameraXform.rx.setAngle(CAMERA_INITIAL_X_ANGLE);
    }
    
    
    private void buildAxes(){
        
        final PhongMaterial redMaterial = new PhongMaterial();
        redMaterial.setDiffuseColor(Color.DARKRED);
        redMaterial.setSpecularColor(Color.RED);
 
        final PhongMaterial greenMaterial = new PhongMaterial();
        greenMaterial.setDiffuseColor(Color.DARKGREEN);
        greenMaterial.setSpecularColor(Color.GREEN);
 
        final PhongMaterial blueMaterial = new PhongMaterial();
        blueMaterial.setDiffuseColor(Color.DARKBLUE);
        blueMaterial.setSpecularColor(Color.BLUE);
 
        final Box xAxis = new Box(AXIS_LENGTH, 1, 1);
        final Box yAxis = new Box(1, AXIS_LENGTH, 1);
        final Box zAxis = new Box(1, 1, AXIS_LENGTH);
        
        xAxis.setMaterial(redMaterial);
        yAxis.setMaterial(greenMaterial);
        zAxis.setMaterial(blueMaterial);
 
        axisGroup.getChildren().addAll(xAxis, yAxis, zAxis);
        axisGroup.setVisible(true);
        world.getChildren().addAll(axisGroup);
        
    }
    
//////////////////////////////////////////////////////////////////////////////////77
    
    
    
}
