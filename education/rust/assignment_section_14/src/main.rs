// Using Rayon for this solution, research rayon::join. Once you have read
// into rayon::join, in the fibonacci_join function of the provided source
// code, create a solution that utilizes rayon::join so that we can see a
// performance enhancement in our Fibonacci solution. If you get an error
// stating that your stack is overflowing, please lower the argument (47)
// that is being passed into the functions.
//
// If you are unsure of what the Fibonacci sequence is, it is a series of
// numbers where the next number is found by adding up the two numbers
// before it. Example of Fibonacci(6)= 1, 1, 2, 3, 5, 8
//
// Don't forget to add the rayon dependency to your Cargo.toml file.
//
use rayon::join;
use std::time::Instant;

static FIBOARG:u32 = 47;

fn fib_recursive(n: u32) -> u32 {
    if n < 2 {
       return n;
    }

    fib_recursive(n - 1) + fib_recursive(n - 2)
}

fn fibonacci_join(n: u32) -> u32 {
    if n < 2 {
       return n;
	} else if n < 20 {
	   return fib_recursive(n);
    } else {
       let (a, b) = join(|| fibonacci_join(n-1), || fibonacci_join(n-2));
       return a + b;
	}
}

fn main() {
    println!("Kicking off rayon-join fibo");
    let start = Instant::now();
    let x = fib_recursive(FIBOARG);
    let duration = start.elapsed();
    println!("Recursive fibonacci answer: {}, time taken: {:?}", x, duration);

    println!("Now run with Rayon's join.");

    let start = Instant::now();
    let x = fibonacci_join(FIBOARG);
    let duration = start.elapsed();
    println!("Rayon fibonacci answer: {}, time taken: {:?}", x, duration);
}
