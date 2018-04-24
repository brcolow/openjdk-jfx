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

import static org.junit.Assert.assertTrue;

/**
 * Simple standalone FX robot test. Compile with:
 *  --add-exports javafx.graphics/com.sun.glass.ui=ALL-UNNAMED
 */
public class SimpleFXRobotTest extends VisualTestBase {

    private Stage testStage;
    private Scene testScene;
    private boolean clicked = false;

    @Test(timeout = 15000)
    public void robotTest() throws Exception {
        runAndWait(() -> {
            BorderPane root = new BorderPane();
            Button button = new Button("Click me");
            button.setOnAction(e -> clicked = true);
            root.setCenter(button);
            testStage = getStage();
            testScene = new Scene(root, 100, 100);
            testStage.setScene(testScene);
            testStage.setTitle("Simple Robot Test");
            testStage.initStyle(StageStyle.UNDECORATED);
            testStage.setX(300);
            testStage.setY(300);
            testStage.show();
            Platform.runLater(() -> {
                //robot.mouseMove(0, 0);
                int x = 350;
                int y = 350;
                System.out.println("mouseMove(" + x + ", " + y + ")");
                robot.mouseMove(x, y);
            });
            Thread.sleep(500);
            Platform.runLater(() -> robot.mousePress(Robot.MOUSE_LEFT_BTN));
            Thread.sleep(500);
            Platform.runLater(() -> robot.mouseRelease(Robot.MOUSE_LEFT_BTN));
            Thread.sleep(500);
            assertTrue(clicked);
        });
    }
}
