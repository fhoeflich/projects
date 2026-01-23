//
// Create a function that takes one argument called val that is of type Vec
// with the values: 1,3,5,7. Inside of this function check the first value of
// the vector and see if it is equal to one. If the value is equal to one,
// then return true, otherwise return false. Next add the value 15 to the
// vector. Print out the vector to confirm your results.
//

fn func(val: Vec<i32>) -> bool {
	if val[0] == 1 {
		true
	} else {
		false
	}
}

fn main() {
	let mut val: Vec<i32> = vec![1,3,5,7];

	if func(val.clone()) {
		println!("true");
	} else {
		println!("false");
	}
	val.push(15);
	println!("{:?}", val);
}
