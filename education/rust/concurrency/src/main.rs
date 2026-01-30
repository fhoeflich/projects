// use std::thread;
// use std::time::Duration;

// use std::sync::mpsc;
// use std::rc::Rc;
// use std::sync::{Arc, Mutex};

use rayon::prelude::*;
use num::{BigUint, One};
use std::time::Instant;

fn factorial(num: u32) -> BigUint {
    if num == 0 || num == 1 {
        return BigUint::one()
    } else {
        (1..=num).map(BigUint::from).reduce(|acc, x| acc * x).unwrap()
    }
}

fn multi_fact(num: u32) -> BigUint {
    if num == 0 || num == 1 {
        return BigUint::one()
    } else {
        (1..=num).into_par_iter().map(BigUint::from).reduce(|| BigUint::one(), |acc, x| acc * x)
    }
}

fn main() {

	// let lock = Arc::new(Mutex::new(0));
	// let lock2 = Arc::clone(&lock);

	// let _ = std::thread::spawn(move || -> () {
	// 	// let _guard = lock2.lock().unwrap();
	// 	// for practice:  do with pattern matching rather than unwrap()
	// 	// for increased precision
	// 	let mut _guard = match lock2.lock() {
	// 		Ok(_guard) => {
	// 			println!("lock2 guard error: {}", _guard);
	// 			// panic!();  // mutex is now poisoned
	// 			_guard
	// 		}
	// 		Err(poisoned) => {
	// 			// println!("lock2 guard error");
	// 			poisoned.into_inner()
	// 		}
	// 	};

	// 	println!("guard before: {:?}", _guard);
	// 	*_guard += 1;
	// 	println!("guard after:  {:?}", _guard);
	// }).join();

	// Tyler's code to do the same thing:
	// let mut guard = match lock.lock() {
	// 	Ok(guard) => guard,
	// 	Err(poisoned) => poisoned.into_inner(),
	// };

	// println!("factorial:  {}", factorial(6));
	// println!("multi_fact: {}", multi_fact(6));

    let now = Instant::now();
    factorial(40000);
    println!("factorial: {:.2?}", now.elapsed());

    let now = Instant::now();
    multi_fact(40000);
    println!("multi_fact: {:.2?}", now.elapsed());

    // let handle = std::thread::spawn(move || {
    // 	println!("Hello from a thread!");
    // });

	// thread::sleep(Duration::from_secs(1));
    // handle.join().unwrap();

    // println!("Hello from main!");

    // let v = vec![1, 2, 3];
    // let handle = std::thread::spawn(move || {
    // 	println!("{:?}", v);
    // });

    // let mut thread_handles = Vec::new();
    // for e in v {
    // 	thread_handles.push(thread::spawn(move || println!("Thread {}", e)));
    // }

    // println!("Main thread!");

    // for handle in thread_handles {
    // 	handle.join().unwrap();
    // }

    // let (transmitter, receiver) = mpsc::sync_channel(1000);
    // let tx = transmitter.clone();

    // let val = String::from("Transmitting!");
    // std::thread::spawn(move || {
    // 	transmitter.send(val).unwrap();
    // });

    // let msg = receiver.recv().unwrap();
    // println!("{}", val);
    // println!("{}", msg);

    // std::thread::spawn(move || {
    // 	let vec = vec![
    // 		String::from("Transmitting"),
    // 		String::from("From"),
    // 		String::from("Original"),
    // 	];

    // 	for val in vec {
    // 		transmitter.send(val).unwrap();
    // 	}
    // });

    // std::thread::spawn(move || {
    //    let vec = vec![
    //       String::from("Clone"),
    //       String::from("Is"),
    //       String::from("Transmitting"),
    //    ];

    //    for val in vec {
    //       tx.send(val).unwrap();
    //    }
    // });

    // for rec in receiver {
    // 	println!("{}", rec);
    // }

    // let rc1 = Rc::new(String::from("Test"));
    // let rc1 = Arc::new(String::from("Test"));
    // let rc2 = rc1.clone();

    // std::thread::spawn(move || {
    // 	rc2;
    // });

    // let counter = Arc::new(Mutex::new(0));
    // let mut handles = vec![];

    // for _ in 0..8 {
    //    let counter = Arc::clone(&counter);
    //    let handle = std::thread::spawn(move || {
    //       let mut num = counter.lock().unwrap();
          // let mut num2 = counter.lock().unwrap();	// creates a deadlock, blocks forever
    //       *num += 1;
    //    }); // lock is given up

    //    handles.push(handle);
    // }

    // for handle in handles {
    //    handle.join().unwrap();
    // }

    // println!("{}", counter.lock().unwrap());
}
