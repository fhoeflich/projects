//
// Create a struct called Car with the fields: mpg, color, and top_speed.
// Once the struct is created, implement the following methods: set_mpg,
// set_color, and set_top_speed. Once you have created these methods, create
// a car, provide it default values, and then set the mpg, color, and top
// speed and then print them out.
//
fn main() {
	struct Car {
		mpg : i32,
		color : String,
		top_speed : i32,
	}

	impl Car {
		fn set_mpg(&mut self, new_mpg: i32) {
			self.mpg = new_mpg;
		}

		// fn set_color(&mut self, new_color: String) {
		fn set_color(&mut self, new_color: &str) {
			self.color = new_color.to_string();
		}

		fn set_top_speed(&mut self, new_top_speed: i32) {
			self.top_speed = new_top_speed;
		}
	}

    let mut car = Car{mpg: 1, color: "White".to_string(), top_speed: 1};

	car.set_mpg(20);
	car.set_color("Red");
	car.set_top_speed(200);

	println!("mpg: {}", car.mpg);
	println!("color: {}", car.color);
	println!("top speed: {}", car.top_speed);
}
