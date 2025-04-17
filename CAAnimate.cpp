#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"

emp::web::Document doc{"target"};

class CAAnimator : public emp::web::Animate {

// grid width and height
const int num_h_boxes = 10;
const int num_w_boxes = 15;
const double RECT_SIDE = 25;
const double width{num_w_boxes * RECT_SIDE};
const double height{num_h_boxes * RECT_SIDE};


//some vectors to hold information about the CA
std::vector<std::vector<double>> cells;

// where we'll draw
emp::web::Canvas canvas{width, height, "canvas"};
emp::Random random;

public:

    CAAnimator() {
        // shove canvas into the div
        // along with some control buttons
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");

        //fill the vectors with 0 to start
        cells.resize(num_w_boxes, std::vector<double>(num_h_boxes, 0));

        //begin by setting all cells at a random state between 0 and 1
        for (int i = 0; i < num_w_boxes; i++) {
            for (int j = 0; j < num_h_boxes; j++) {
                cells[i][j] = random.GetDouble(0, 1);
            }
        }
    }

    /**
     * @brief Draws a single cell on the canvas.
     * 
     * @param x The x-coordinate of the cell in the grid.
     * @param y The y-coordinate of the cell in the grid.
     * 
     * Draws a cell at the specified position with color corresponding to the cell's value.
     */
    void DrawCell(int x, int y) {
        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, emp::ColorHSV(0, 0, cells[x][y]), "black");
    }

    /**
     * @brief Wraps the x-coordinate around the grid to ensure toroidal behavior.
     * 
     * @param coord The x-coordinate to wrap.
     * @return int The wrapped x-coordinate.
     * 
     * If coord is negative, it returns num_w_boxes - 1.
     * If coord is equal to or greater than num_w_boxes, it wraps around to 0.
     * Otherwise, it returns the original coordinate.
     */
    int wrapX(int coord) {
        if (coord < 0) return num_w_boxes - 1; // Wrap negative to max
        if (coord >= num_w_boxes) return 0;    // Wrap overflow to 0
        return coord;
    }

    /**
     * @brief Wraps the y-coordinate around the grid to ensure toroidal behavior.
     * 
     * @param coord The y-coordinate to wrap.
     * @return int The wrapped y-coordinate.
     * 
     * If coord is negative, it returns num_h_boxes - 1.
     * If coord is equal to or greater than num_h_boxes, it wraps around to 0.
     * Otherwise, it returns the original coordinate.
     */
    int wrapY(int coord) {
        if (coord < 0) return num_h_boxes - 1; // Wrap negative to max
        if (coord >= num_h_boxes) return 0;    // Wrap overflow to 0
        return coord;
    }

    /**
    * @brief Counts the number of live neighbors surrounding a cell in the grid.
    * 
    * @param x The x-coordinate of the cell in the grid.
    * @param y The y-coordinate of the cell in the grid.
    * @return int The number of live neighbors (cells with value 1) surrounding the specified cell.
    * 
    * This method considers the grid as a toroidal array, meaning the edges wrap around. 
    * For example, a cell on the far-right edge has neighbors on the far-left edge.
    * 
    * Steps:
    * - Iterate through the 3x3 grid centered on the cell at (x, y).
    * - Skip the center cell itself.
    * - Use functions to handle edge cases.
    * - Count the number of live cells (value 1) in the surrounding cells.
    */
    int FindNeighbors(int x, int y) {

        int LiveNeighbors = 0;

        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                if (i == x && j == y) {
                    continue;
                }
                int WrappedX = wrapX(i);
                int WrappedY = wrapY(j);

                if (cells[WrappedX][WrappedY] >= 0.8) {
                    LiveNeighbors += 1;
                }
            }
        }

        return LiveNeighbors;
    }

    /**
     * @brief Updates the cell's intensity based on its current gradient and the number of live neighbors.
     * 
     * @param x The x-coordinate of the cell in the grid.
     * @param y The y-coordinate of the cell in the grid.
     * @param liveNeighbors The number of neighboring cells that are considered "live" (value >= 0.8).
     * 
     * @return double The new intensity value for the cell.
     * 
     * This function implements a gradient-based update for the cell using a continuous range between 0 and 1.
     * It operates as follows:
     * - If the current intensity is between 0.05 and 0.95:
     *   - If there are 2 or 3 live neighbors, the intensity increases by 0.05.
     *   - Otherwise, the intensity decreases by 0.05 to simulate gradual fading.
     * - If the intensity is outside of the [0.05, 0.95] range, a new random intensity between 0 and 1 is assigned.
     */
    double UpdateCellGradient(int x, int y, int liveNeighbors) {
        double current = cells[x][y];
        if (current >= 0.05 && current <= 0.95) { 
            if (liveNeighbors == 2 || liveNeighbors == 3) {
                return current + 0.05;
            } else {
                return current - 0.05;
            }
        } else {
            current = random.GetDouble(0, 1); // Randomly set the cell to a new state
            return current;
        }
        return 0;
    }

    /**
     * @brief Updates the state of the grid and redraws the canvas for the next animation frame.
     * 
     * This method clears the canvas, redraws all cells based on their current state, and computes the next state
     * of the grid using modified of Conway's Game of Life. The updated grid is stored in a temporary vector
     * and then replaces the current grid.
     */
    void DoFrame() override {
        canvas.Clear();
        std::vector<std::vector<double>> updatedCells = cells;

        for (int x = 0; x < num_w_boxes; x++) {
            for (int y = 0; y < num_h_boxes; y++) {
                DrawCell(x, y); // Optionally update DrawCell to render gradient colors based on intensity.
                int liveNeighbors = FindNeighbors(x, y);
                // Use the gradient update rules instead of binary rules:
                updatedCells[x][y] = UpdateCellGradient(x, y, liveNeighbors);
            }
        }

        cells = updatedCells;
    }
};

CAAnimator animator;

int main() {
    animator.DoFrame();
    animator.Step();
}
