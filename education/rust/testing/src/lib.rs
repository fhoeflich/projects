#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_ne!(result, 5);
    }

    #[test]
    #[should_panic]		// can be used to make the panic `expected'
	// #[ignore]				// or ignore the test altogether
    fn it_fails() {
        panic!("Test failed!");
    }

    #[test]
    fn call_simple_add() {
        assert!(simple_add());
    }

    // #[bench]		// unstable, has special req'ts.  Someday ...
    // fn call_bench() {
	// 	println!("call_bench");
	// 	true
    // }
}

fn simple_add() -> bool {
    if 2+2 == 4 {
        true
    } else {
        false
    }
}
