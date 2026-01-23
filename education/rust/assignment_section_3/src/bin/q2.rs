//
// Create a function called "add_two". This function is going to take one
// parameter that is i8 and add two to it. For the function, do you have to
// pass the value by reference in order for you to maintain ownership of it
// inside of main?
//
fn add_two(i:i8) -> i8 {
	i+2
}

fn main() {
	let i:i8 = 5;
	let v:i8;

	v = add_two(i);
	println!("{}", v);
}
