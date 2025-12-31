#![allow(unused)]
fn main() {
    // let name = String::from("Tyler");
    // let course = "Rust".to_string();
    // let new_name = name.replace("Tyler", "Ty");

    // println!("{}", name);
    // println!("{}", course);
    // println!("{}", new_name);

    // &str = "string slice" or "stir"
    // let str1 = "hello"; //&str
    // let str2 = str1.to_string();
    // let str3 = &str2;

    // println!("{}", str1);
    // println!("{}", str2);
    // println!("{}", str3);

    // compare string == != (does not equal)
    // println!("{}", "ONE".to_lowercase() == "one");

    // let rust = "\x52\x75\x73\x74";
    // println!("{}", rust);


	// Some practice on other string/slice methods from the Rust
	// standard library documentation.
	// let s = "this is old";
	// assert_eq!("this is new", s.replace("old", "new"));
	// assert_eq!("than an old", s.replace("is", "an"));

	// let bananas = "bananas";
	// assert!(bananas.contains("nana"));
	// assert!(!bananas.contains("apples"));

	// assert!(bananas.starts_with("bana"));
	// assert!(!bananas.starts_with("nana"));

	// assert!(bananas.starts_with(&['b', 'a', 'n', 'a']));
	// assert!(!bananas.starts_with(['b', 'a', 'n', 'a']));
	// assert!(bananas.starts_with(&['a', 'b', 'c', 'd']));

	let x = &[5, 7];
	let y = x.clone();

	println!("x = {:?}, y = {:?}", x, y);
}
