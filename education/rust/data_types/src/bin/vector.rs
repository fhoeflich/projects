fn main() {
	// let mut nums = vec![1,2,3];

	// nums.push(4);
	// println!("{:?}", nums);

	// nums.pop();
	// println!("{:?}", nums);

	let mut nums = Vec::new();

	nums.push("String1");
	nums.push("String2");
	nums.push("String3");
	println!("{:?}", nums);

	nums.reverse();
	println!("{:?}", nums);

	let vect = Vec::<i32>::with_capacity(2);
	println!("{}", vect.capacity());

	// let v: Vec<i32> = (0..5).collect();
	let v: Vec<i32> = 0..5;
	println!("{:?}", v);
}
