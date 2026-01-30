use std::rc::Rc;

fn main() {
    //Question 1: Create a variable on the stack and a variable on the heap. Multiply their values and print out the results.
	let var:i8 = 10;
	let boxvar = Box::new(var);

	println!("{}", var * *boxvar);

    //Question 2: Create a variable that holds a String
	let mystr = String::from("hello");

    {
        //Create a reference counting smart pointer that points to the above String.
		let s1 = Rc::new(mystr);
        
        //Print out how many references the smart pointer has.
		println!("s1 refcount (before): {:?}", Rc::strong_count(&s1));

        //Code block
        {
            //Create another reference counting smart pointer that points to our first smart pointer
			let s2 = s1.clone();

            //Print out how many references each smart pointer has
			println!("s1 refcount: {:?}  s2 refcount: {:?}",
				Rc::strong_count(&s1), Rc::strong_count(&s2));
        }
        //What value is dropped here?
		// s2 refcount gets dropped as it goes out of scope

        //Print out how many references out first smart pointer has
		println!("s1 refcount (after): {:?}", Rc::strong_count(&s1));


    } //What value is dropped here?
	// s1 refcount gets dropped as it goes out of scope

    //Comment out the line below. What do you think will happen when you try to run the program now?
    //println!("rc_value: {}", rc_value);
}
