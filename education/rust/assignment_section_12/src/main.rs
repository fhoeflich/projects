// Create a vector with the values 1, 3, 5, 7, and 9. Then use an iterator and
// a closure to multiply all of the values by 10 and store the result in
// another vector. Print out the vector to confirm your results.
//

fn main() {
	let vec1 = vec![1,3,5,7,9];
	let vec2: Vec<i16> = vec1.iter().map(|x| x * 10).collect();

    for val in vec1.iter() {
        println!("{:?}", val);
    }

	println!(" ");

    for val in vec2.iter() {
        println!("{:?}", val);
    }
}
