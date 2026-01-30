use std::rc::Rc;

fn main() {
    // Create a new Rc and get the initial reference count
    let five = Rc::new(5);
    println!("Initial count: {}", Rc::strong_count(&five)); // Output: Initial count: 1

    // Create new references via `clone()`
    let _five_clone_1 = Rc::clone(&five);
    println!("Count after clone 1: {}", Rc::strong_count(&five)); // Output: Count after clone 1: 2

    {
        let _five_clone_2 = Rc::clone(&five);
        println!("Count after clone 2: {}", Rc::strong_count(&five)); // Output: Count after clone 2: 3

        // five_clone_2 goes out of scope here
    }

    println!("Count after clone 2 goes out of scope: {}", Rc::strong_count(&five)); // Output: Count after clone 2 goes out of scope: 2

    // five_clone_1 goes out of scope here (at the end of main)
    // five goes out of scope here
}
