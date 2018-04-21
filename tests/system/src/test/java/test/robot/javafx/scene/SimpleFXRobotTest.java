package test.robot.javafx.scene;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.layout.BorderPane;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.stage.Stage;
import javafx.stage.StageStyle;
import org.junit.Test;
import test.robot.testharness.VisualTestBase;

import com.sun.glass.ui.Robot;


/**
 * Simple standalone FX robot test. Compile with:
 *  --add-exports javafx.graphics/com.sun.glass.ui=ALL-UNNAMED
 */
public class SimpleFXRobotTest extends VisualTestBase {

    private Stage testStage;
    private Scene testScene;
    private boolean clicked = false;

    @Test(timeout = 15000)
    public void test() {
        runAndWait(() -> {
            BorderPane root = new BorderPane();
            Scene scene = new Scene(root, 100, 100);

            Button button = new Button("Click me");
            button.setOnAction(e -> clicked = true);

            root.setCenter(button);
            testStage = getStage();
            testScene = new Scene(root, 100, 100);

            testStage.setScene(testScene);

            testStage.setTitle("Simple Robot Test");
            testStage.initStyle(StageStyle.UNDECORATED);

            testStage.setScene(scene);
            testStage.setX(300);
            testStage.setY(300);
            testStage.show();

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
        });
    }
}
