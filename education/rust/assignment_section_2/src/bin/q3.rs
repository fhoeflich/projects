fn concat_string(str: &str) -> String {
	let mut retval = str.to_string();

	retval.push_str(" World");
	retval
}

fn main() {
//
// Create a function called "concat_string". Create a string variable and
// assign the value "Hello" to it. The function is going to take one argument
// that is of type string and is going to return a String. Inside this
// function, concatenate the string " World". Print out the results in main()
// to confirm your results.
//
	let str = "Hello";
	let result = concat_string(str);

	println!("Concatenated string is {:?}", result);
}
