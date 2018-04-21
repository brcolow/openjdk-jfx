package test.javafx.scene;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.layout.BorderPane;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

import com.sun.glass.ui.Robot;


/**
 * Simple standalone FX robot test. Compile with:
 *  --add-exports javafx.graphics/com.sun.glass.ui=ALL-UNNAMED
 */
public class SimpleFXRobotTest extends Application {

    private Robot robot;
    private boolean clicked = false;

    @Override public void start(Stage stage) {
        stage.setTitle("Simple Robot Test");
        stage.initStyle(StageStyle.UNDECORATED);

        BorderPane root = new BorderPane();
        Scene scene = new Scene(root, 100, 100);

        Button button = new Button("Click me");
        button.setOnAction(e -> clicked = true);

        root.setCenter(button);
        stage.setScene(scene);
        stage.setX(300);
        stage.setY(300);
        stage.show();

        robot = com.sun.glass.ui.Application.GetApplication().createRobot();

        Thread testThread = new Thread(() -> {
            Platform.runLater(() -> {
                //robot.mouseMove(0, 0);
                int x = 350;
                int y = 350;
                System.err.println("mouseMove(" + x + ", " + y + ")");
                robot.mouseMove(x, y);
            });
            try { Thread.sleep(500); } catch (Exception ex) {}
            Platform.runLater(() -> {
                robot.mousePress(Robot.MOUSE_LEFT_BTN);
            });
            try { Thread.sleep(500); } catch (Exception ex) {}
            Platform.runLater(() -> {
                robot.mouseRelease(Robot.MOUSE_LEFT_BTN);
            });
            try { Thread.sleep(500); } catch (Exception ex) {}
            if (clicked) {
                System.out.println("PASSED");
            } else {
                System.out.println("***** FAILED");
            }
            Platform.exit();
        });
        testThread.start();
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Application.launch(args);
    }
}
