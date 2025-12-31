use std::borrow::ToOwned;

fn main() {
    let borrowed_str: &str = "Jai Shree Ram!";
    let mut owned_string: String = borrowed_str.to_owned();
	owned_string.push_str("Jai Bajrang bali");

	println!("Borrowed string: {}",borrowed_str);
    println!("Owned string: {}", owned_string);
}
