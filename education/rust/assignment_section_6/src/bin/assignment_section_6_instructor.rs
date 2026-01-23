//
// Create a simple print function that uses Generic T. This Generic T will
// need to implement std::fmt::Debug depending on the values you pass in. Our
// function takes one parameter of type T. Our function will then print out
// the value that is passed in.
//
// use std::fmt::Debug;

fn simple_print<T: std::fmt::Debug>(x: T) {
	println!("{:?}", x);
}

fn main() {
	// simple_print(3);		// works with default formatter

	// #[derive(Debug)]		// works with default formatter,
	struct P;			// but fails if you comment out #[derive(Debug)]
	simple_print(P);
}
