#include <stdio.h>
#include <stdlib.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_SHAPES 100

#define MY_ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct {
    int id;
    int type; /* 1 = Line, 2 = Rectangle, 3 = Circle, 4 = Triangle */
    int active;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int w, h;
    int r;
} Shape;

Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;
char canvas[HEIGHT][WIDTH];

/* Helper to clear stdin buffer to prevent infinite loops on malformed inputs */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Helper to read an integer with a prompt */
int read_int(const char* prompt) {
    int val;
    printf("%s", prompt);
    while (scanf("%d", &val) != 1) {
        printf("Invalid input. Please enter an integer: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    return val;
}

/* Initialize canvas with '_' */
void clear_canvas() {
    int y, x;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

/* Display canvas */
void display_canvas() {
    int y, x;
    printf("\n+");
    for (x = 0; x < WIDTH; x++) {
        printf("-");
    }
    printf("+\n");

    for (y = 0; y < HEIGHT; y++) {
        printf("|");
        for (x = 0; x < WIDTH; x++) {
            putchar(canvas[y][x]);
        }
        printf("|\n");
    }

    printf("+");
    for (x = 0; x < WIDTH; x++) {
        printf("-");
    }
    printf("+\n\n");
}

/* Bresenham's Line Algorithm */
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = MY_ABS(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -MY_ABS(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_rectangle(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    draw_line(x, y, x + w - 1, y);
    draw_line(x, y + h - 1, x + w - 1, y + h - 1);
    draw_line(x, y, x, y + h - 1);
    draw_line(x + w - 1, y, x + w - 1, y + h - 1);
}

/* Midpoint Circle Algorithm */
void draw_circle(int cx, int cy, int r) {
    int x, y, p, i;
    if (r < 0) return;
    x = r;
    y = 0;
    p = 1 - r;

    while (x >= y) {
        int points[8][2];
        points[0][0] = cx + x; points[0][1] = cy + y;
        points[1][0] = cx + y; points[1][1] = cy + x;
        points[2][0] = cx - y; points[2][1] = cy + x;
        points[3][0] = cx - x; points[3][1] = cy + y;
        points[4][0] = cx - x; points[4][1] = cy - y;
        points[5][0] = cx - y; points[5][1] = cy - x;
        points[6][0] = cx + y; points[6][1] = cy - x;
        points[7][0] = cx + x; points[7][1] = cy - y;

        for (i = 0; i < 8; i++) {
            int px = points[i][0];
            int py = points[i][1];
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                canvas[py][px] = '*';
            }
        }

        y++;
        if (p < 0) {
            p += 2 * y + 1;
        } else {
            x--;
            p += 2 * (y - x) + 1;
        }
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

/* Re-renders all shapes to the canvas */
void render_canvas() {
    int i;
    clear_canvas();
    for (i = 0; i < shape_count; i++) {
        if (!shapes[i].active) continue;

        switch (shapes[i].type) {
            case 1:
                draw_line(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
                break;
            case 2:
                draw_rectangle(shapes[i].x1, shapes[i].y1, shapes[i].w, shapes[i].h);
                break;
            case 3:
                draw_circle(shapes[i].x1, shapes[i].y1, shapes[i].r);
                break;
            case 4:
                draw_triangle(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3);
                break;
        }
    }
}

void list_shapes() {
    int i;
    int found = 0;
    printf("\n--- Active Shapes ---\n");
    for (i = 0; i < shape_count; i++) {
        if (!shapes[i].active) continue;
        found = 1;
        printf("ID %2d: ", shapes[i].id);
        switch (shapes[i].type) {
            case 1:
                printf("Line from (%d, %d) to (%d, %d)\n",
                       shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
                break;
            case 2:
                printf("Rectangle at (%d, %d), width=%d, height=%d\n",
                       shapes[i].x1, shapes[i].y1, shapes[i].w, shapes[i].h);
                break;
            case 3:
                printf("Circle at (%d, %d), radius=%d\n",
                       shapes[i].x1, shapes[i].y1, shapes[i].r);
                break;
            case 4:
                printf("Triangle vertices: (%d, %d), (%d, %d), (%d, %d)\n",
                       shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3);
                break;
        }
    }
    if (!found) {
        printf("No active shapes on the canvas.\n");
    }
    printf("---------------------\n\n");
}

int add_shape() {
    int type_choice;
    Shape s;

    if (shape_count >= MAX_SHAPES) {
        printf("Error: Maximum shape limit reached (%d).\n", MAX_SHAPES);
        return -1;
    }

    printf("\nChoose Shape Type to Add:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    type_choice = read_int("Enter selection (1-4): ");

    s.id = next_id++;
    s.active = 1;
    s.type = type_choice;

    switch (type_choice) {
        case 1:
            printf("Enter Line Coordinates:\n");
            s.x1 = read_int("  x1: ");
            s.y1 = read_int("  y1: ");
            s.x2 = read_int("  x2: ");
            s.y2 = read_int("  y2: ");
            break;
        case 2:
            printf("Enter Rectangle details:\n");
            s.x1 = read_int("  x (top-left): ");
            s.y1 = read_int("  y (top-left): ");
            s.w = read_int("  width: ");
            s.h = read_int("  height: ");
            break;
        case 3:
            printf("Enter Circle details:\n");
            s.x1 = read_int("  cx (center x): ");
            s.y1 = read_int("  cy (center y): ");
            s.r = read_int("  radius: ");
            break;
        case 4:
            printf("Enter Triangle vertices:\n");
            s.x1 = read_int("  x1: ");
            s.y1 = read_int("  y1: ");
            s.x2 = read_int("  x2: ");
            s.y2 = read_int("  y2: ");
            s.x3 = read_int("  x3: ");
            s.y3 = read_int("  y3: ");
            break;
        default:
            printf("Invalid shape type.\n");
            return -1;
    }

    shapes[shape_count++] = s;
    printf("Shape added successfully with ID: %d!\n", s.id);
    return s.id;
}

void delete_shape_ui() {
    int i;
    int id = read_int("Enter ID of shape to delete: ");
    for (i = 0; i < shape_count; i++) {
        if (shapes[i].id == id && shapes[i].active) {
            shapes[i].active = 0;
            printf("Shape ID %d has been deleted.\n", id);
            return;
        }
    }
    printf("Shape ID %d not found or already deleted.\n", id);
}

void modify_shape_ui() {
    int i;
    int id = read_int("Enter ID of shape to modify: ");
    for (i = 0; i < shape_count; i++) {
        if (shapes[i].id == id && shapes[i].active) {
            switch (shapes[i].type) {
                case 1:
                    printf("Modifying Line (ID %d):\n", id);
                    shapes[i].x1 = read_int("  new x1: ");
                    shapes[i].y1 = read_int("  new y1: ");
                    shapes[i].x2 = read_int("  new x2: ");
                    shapes[i].y2 = read_int("  new y2: ");
                    break;
                case 2:
                    printf("Modifying Rectangle (ID %d):\n", id);
                    shapes[i].x1 = read_int("  new x (top-left): ");
                    shapes[i].y1 = read_int("  new y (top-left): ");
                    shapes[i].w = read_int("  new width: ");
                    shapes[i].h = read_int("  new height: ");
                    break;
                case 3:
                    printf("Modifying Circle (ID %d):\n", id);
                    shapes[i].x1 = read_int("  new cx (center x): ");
                    shapes[i].y1 = read_int("  new cy (center y): ");
                    shapes[i].r = read_int("  new radius: ");
                    break;
                case 4:
                    printf("Modifying Triangle (ID %d):\n", id);
                    shapes[i].x1 = read_int("  new x1: ");
                    shapes[i].y1 = read_int("  new y1: ");
                    shapes[i].x2 = read_int("  new x2: ");
                    shapes[i].y2 = read_int("  new y2: ");
                    shapes[i].x3 = read_int("  new x3: ");
                    shapes[i].y3 = read_int("  new y3: ");
                    break;
            }
            printf("Shape ID %d updated successfully.\n", id);
            return;
        }
    }
    printf("Shape ID %d not found or not active.\n", id);
}

int main() {
    int choice;
    printf("=== Interactive 2D ASCII Graphics Editor ===\n");
    printf("Canvas size is %d columns x %d rows.\n", WIDTH, HEIGHT);
    printf("Drawing is done with '*' and empty spaces with '_'.\n");

    /* Add default shapes programmatically using flat structures */
    shapes[shape_count].id = next_id++;
    shapes[shape_count].type = 2; /* Rectangle */
    shapes[shape_count].active = 1;
    shapes[shape_count].x1 = 5;
    shapes[shape_count].y1 = 2;
    shapes[shape_count].w = 10;
    shapes[shape_count].h = 5;
    shape_count++;

    shapes[shape_count].id = next_id++;
    shapes[shape_count].type = 3; /* Circle */
    shapes[shape_count].active = 1;
    shapes[shape_count].x1 = 30;
    shapes[shape_count].y1 = 8;
    shapes[shape_count].r = 4;
    shape_count++;
    while (1) {
        printf("\nMain Menu:\n");
        printf("1. Add a Shape\n");
        printf("2. Delete a Shape by ID\n");
        printf("3. Modify a Shape by ID\n");
        printf("4. Display Canvas (Render)\n");
        printf("5. List Active Shapes\n");
        printf("6. Clear Canvas\n");
        printf("7. Exit\n");

        choice = read_int("Enter selection (1-7): ");
        switch (choice) {
            case 1:
                add_shape();
                break;
            case 2:
                list_shapes();
                delete_shape_ui();
                break;
            case 3:
                list_shapes();
                modify_shape_ui();
                break;
            case 4:
                render_canvas();
                display_canvas();
                break;
            case 5:
                list_shapes();
                break;
            case 6:
                shape_count = 0;
                next_id = 1;
                clear_canvas();
                printf("Canvas cleared!\n");
                break;
            case 7:
                printf("Exiting editor. Goodbye!\n");
                return 0;
            default:
                printf("Invalid selection. Try again.\n");
        }
    }
    return 0;
}