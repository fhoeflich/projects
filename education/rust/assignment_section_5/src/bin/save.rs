// Create an enum called Shape and provide the values of "triangle square,
// pentagon, octagon".  Then create a method for this enum that returns the
// number of corners each shape has based on the type of shape.
//
// Example:
//
// triangle.corners() will return 3
// square.corners() will return 4
//

enum Shape {triangle, square, pentagon, octagon}

impl Shape {
    fn corners(self) -> {
        match self {
            Shape::triangle => "3",
            Shape::square => "4",
            Shape::pentagon => "5",
			_ => "8",		// presumed to be an octagon
        }
    }
}

fn main() {
    let triangle = Shape::triangle;
    let square = Shape::square;
    println!("Triangles have {} corners", triangle.corners);
    println!("Squares have {} corners", triangle.square);
    println!("Pentagons have {} corners", triangle.pentagon);
    println!("Octagons have {} corners", triangle.octagon);
}
