// Provide a recursive solution to the problem of:
//
// 1. Stipulate the bottom row of numbers of a triangle.
// 2. Add the first and second numbers to get the first number of the next
//		row up.  Add the second and third to get the second number, etc.
// 3. At the end (top row has one number), we are done.  Output should be
//		all of the triangle rows printed in order of *increasing* length,
//		i.e. from the top of the triangle down to the bottom.
//
fn sum_triangle(array : Vec<i32>) -> Option<Vec<i32>> {
    let mut nextup: Vec<i32> = vec![];

	// Terminating step - this is the top of the triangle.
	if array.len() == 1 {
		println!("row is {:?}", array);
		return None;
	}

	// Form nextup, the next row up above us.
	for i in 0..(array.len() - 1) {
		nextup.push((array[i] + array[i+1]).try_into().unwrap());
	}

	// Recurse.  Print the rows in reverse order as we backtrack up the
	//	triangle.
	let result = sum_triangle(nextup);
	println!("row is {:?}", array);

	return result;
}

fn main() {
	let mut array: Vec<i32> = vec![];

	array.push(1);
	array.push(2);
	array.push(3);
	array.push(4);
	array.push(5);

	let result = sum_triangle(array.clone());
	match result {
		None => println!("Done."),
		_ => println!("Something went wrong"),
	}
}
