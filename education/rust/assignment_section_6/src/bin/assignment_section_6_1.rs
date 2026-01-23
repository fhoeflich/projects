//
// Modify the solution to the Section 4 assignment by creating a Trait that
// has the set_mpg, set_color, and set_top_speed methods. Then create a
// Motorcycle struct with the same fields as the Car struct: mpg, color, and
// top_speed. Now implement your Trait on both the Car and Motorcycle struct.
// Print out the results to confirm a working solution.
//
struct Car {
	mpg : i32,
	color : String,
	top_speed : i32,
}

struct Motorcycle {
	mpg : i32,
	color : String,
	top_speed : i32,
}

trait Vehicle {
	fn set_mpg(&mut self, new_mpg: i32) -> i32 {
		new_mpg
	}

	fn set_color(&mut self, new_color: &str) -> String {
		new_color.to_string()
	}

	fn set_top_speed(&mut self, new_top_speed: i32) -> i32 {
		new_top_speed
	}
}

impl Vehicle for Car {}
impl Vehicle for Motorcycle {}

fn main() {
    let car = Car{mpg: 1, color: "Blue".to_string(), top_speed: 200};
    let motorcycle = Motorcycle{mpg: 10, color: "Red".to_string(), top_speed: 100};

	println!("mpg: {}", car.mpg);
	println!("color: {}", car.color);
	println!("top speed: {}", car.top_speed);

	println!("mpg: {}", motorcycle.mpg);
	println!("color: {}", motorcycle.color);
	println!("top speed: {}", motorcycle.top_speed);
}
