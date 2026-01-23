// Create an enum called Shape and provide the values of "triangle square,
// pentagon, octagon".  Then create a method for this enum that returns the
// number of corners each shape has based on the type of shape.
//
// Example:
//
// triangle.corners() will return 3
// square.corners() will return 4
//

enum Shape {Triangle, Square, Pentagon, Octagon}

impl Shape {
    fn corners(self) -> &'static str {
        match self {
            Shape::Triangle => "3",
            Shape::Square => "4",
            Shape::Pentagon => "5",
			_ => "8",		// presumed to be an octagon
        }
    }
}

fn main() {
    let tri = Shape::Triangle;
    let sq = Shape::Square;
    let pent = Shape::Pentagon;
    let oct = Shape::Octagon;

    println!("Triangles have {} corners", tri.corners());
    println!("Squares have {} corners", sq.corners());
    println!("Pentagons have {} corners", pent.corners());
    println!("Octagons have {} corners", oct.corners());
}
