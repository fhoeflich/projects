fn main() {
//
// Create a vector and put in the values "2, 4, 6, 8, 10".
// Once you have created the vector perform the following: print out the
// current values, remove the value 10, add the value 12, and then print
// the vector back out to confirm your results.
//

	let mut vec = vec![2, 4, 6, 8, 10];

	println!("{:?}", vec);
	vec.pop();
	vec.push(12);
	println!("{:?}", vec);
}
