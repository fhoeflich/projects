fn fact(num: i32) -> i32 {
	if num > 1 {
		return num * fact(num - 1)
	} else {
		return 1
	}
}

fn fibo(num: i32) -> i32 {
	if num < 2 {
		return num
	} else {
		return fibo(num - 1) + fibo(num - 2)
	}
}

fn palindrome(array: &Vec<i32>, start: usize, end: usize) -> bool {
	if start >= end {
		return true
	}

	if array[start] == array[end] {
		return palindrome(array, start + 1, end - 1)
	} else {
		return false
	}
}

fn toh(n: i64) -> i64 {
	if n == 0 {
		return 0
	}

	return toh(n-1) + 1 + toh(n-1)
}

fn main() {
	// let n = 15;

	// println!("fact({}) is {}", n, fact(n));
	// println!("fibo({}) is {}", n, fibo(n));

	// let array = vec![1,2,3,4];
	// println!("{:?}", palindrome(&array, 0, array.len() - 1));

	// let array2 = vec![1,2,3,4,3,2,1];
	// println!("{:?}", palindrome(&array2, 0, array2.len() - 1));

	println!("{}", toh(0));
	println!("{}", toh(1));
	println!("{}", toh(2));
	println!("{}", toh(3));
	println!("{}", toh(4));
	println!("{}", toh(64));
}
