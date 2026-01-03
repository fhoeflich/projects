fn control_flow(x: i32) {
	if x == 1 {
		println!("The value is one");
	} else if x < 25 {
		println!("The value is less than 25");
	} else if x > 50 {
		println!("The value is greater than 50");
	} else {
		println!("The value is greater than 25 but less than 50");
	}
}

fn main() {
//
// Create a function called control_flow. This is going to take one argument
// that is an integer. Based on this integer, print out the following: "The
// value is one", "The value is greater than 50", "The value is less than 25",
// or "The value is greater than 25 but less than 50".
//
	let x = 1;
	control_flow(x);

	let x = 51;
	control_flow(x);

	let x = 24;
	control_flow(x);

	let x = 30;
	control_flow(x);
}
