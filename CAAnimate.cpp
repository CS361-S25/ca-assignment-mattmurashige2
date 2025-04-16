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
std::vector<std::vector<int> > cells;
        

// where we'll draw
emp::web::Canvas canvas{width, height, "canvas"};

public:

    CAAnimator() {
        // shove canvas into the div
        // along with some control buttons
        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");

        //fill the vectors with 0 to start
        cells.resize(num_w_boxes, std::vector<int>(num_h_boxes, 0));

        //showing how to set a cell to 'alive'
        cells[1][0] = 1; 
        cells[2][0] = 1; 
        cells[3][0] = 1; 
        cells[4][0] = 1; 
        cells[4][1] = 1;
        cells[4][2] = 1;
        cells[3][3] = 1;
        cells[0][1] = 1;
        cells[0][3] = 1;
    }

    /**
     * @brief Draws a single cell on the canvas.
     * 
     * @param x The x-coordinate of the cell in the grid.
     * @param y The y-coordinate of the cell in the grid.
     * 
     * If the cell is dead (value 0), it is drawn as a white rectangle with a black outline.
     * If the cell is alive (value 1), it is drawn as a black rectangle.
     */
    void DrawCell(int x, int y) {
        if (cells[x][y] == 0) {
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
        } else {
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "black", "black");
        }
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
    * - Use wrapping functions (`wrapX` and `wrapY`) to handle edge cases.
    * - Count the number of live cells (value 1) in the surrounding cells.
    */
    int FindNeighbors(int x, int y) {

        auto wrapX = [this](int coord) { //
            if (coord < 0) return num_w_boxes - 1; // Wrap negative to max
            if (coord >= num_w_boxes) return 0;    // Wrap overflow to 0
            return coord;
        };

        auto wrapY = [this](int coord) { //
            if (coord < 0) return num_h_boxes - 1; // Wrap negative to max
            if (coord >= num_h_boxes) return 0;    // Wrap overflow to 0
            return coord;
        };

        int LiveNeighbors = 0;

        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                if (i == x && j == y) {
                    continue;
                }
                int WrappedX = wrapX(i);
                int WrappedY = wrapY(j);

                if (cells[WrappedX][WrappedY] == 1) {
                    LiveNeighbors += 1;
                }
            }
        }

        return LiveNeighbors;
    }

    /**
     * @brief Determines the next state of a cell based on its current state and the number of live neighbors.
     * 
     * @param x The x-coordinate of the cell in the grid.
     * @param y The y-coordinate of the cell in the grid.
     * @param LiveNeighbors The number of live neighbors surrounding the cell.
     * @return int The new state of the cell (1 for alive, 0 for dead).
     * 
     * Rules:
     * - A live cell (value 1) survives if it has 2 or 3 live neighbors; otherwise, it dies.
     * - A dead cell (value 0) becomes alive if it has exactly 3 live neighbors; otherwise, it remains dead.
     */
    int UpdateCell(int x, int y, int LiveNeighbors) {
        if (cells[x][y] == 1) {
            return (LiveNeighbors == 2 || LiveNeighbors == 3) ? 1 : 0;
        } else {
            return (LiveNeighbors == 3) ? 1 : 0;
        }
    }

    /**
     * @brief Updates the state of the grid and redraws the canvas for the next animation frame.
     * 
     * This method clears the canvas, redraws all cells based on their current state, and computes the next state
     * of the grid using the rules of Conway's Game of Life. The updated grid is stored in a temporary vector
     * and then replaces the current grid.
     */
    void DoFrame() override {
        canvas.Clear();
        std::vector<std::vector<int>> updatedCells = cells;

        for (int x = 0; x < num_w_boxes; x++) {
            for (int y = 0; y < num_h_boxes; y++) {
                DrawCell(x, y); // Draw the current cell
                int LiveNeighbors = FindNeighbors(x, y); // Count live neighbors
                updatedCells[x][y] = UpdateCell(x, y, LiveNeighbors); // Update cell state
            }
        }

        cells = updatedCells; // Replace the current grid with the updated grid
    }
};

CAAnimator animator;

int main() {
    animator.DoFrame();
    animator.Step();
}