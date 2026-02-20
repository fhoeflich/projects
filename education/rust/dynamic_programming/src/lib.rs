pub fn fib(val: i32) -> i32 {
    let mut a = 0;
	let mut b = 1;
	// let mut c: i32;
	let mut c: Option<i32> = None;

	for _ in 2..=val {
		c = Some(a + b);
		a = b;
		// b = c;
		b = c.expect("None");
	}

	//
	// Just joking around.  b and c ought to be equal at this point; this is
	// a bit of practice writing Rust code to verify that c, which in the
	// original exercise was declared as `let mut c: i32` and the final
	// statement in the loop was simply `b = c`, is only used if c got
	// initialized.  Works fine.
	//
	if let Some(_value) = c {
		println!("Returning c: {}", c.unwrap());
		return c.unwrap();
	} else {
		println!("Returning b: {}", b);
		return b;
	}
}

pub fn longest_common_subsequence(a: &str, b: &str) -> String {
	let a: Vec<char> = a.chars().collect(); // store all chars into the vec
	let b: Vec<char> = b.chars().collect();

	let (len_a, len_b) = (a.len(), b.len());

	// solution[i][j] is the length of the LCS (longest common subsequence)
	// between a[0..i-1] and b[0..j-1]
	let mut solution = vec![vec![0; len_b+1]; len_a+1];

	for (i,mi) in a.iter().enumerate() {
		for (j, mj) in b.iter().enumerate() {
			// if mi == mj, there is a new common character
			// otherwise, take the best of the two solutions
			// at (i-1, j) and (i, j-1).

			solution[i+1][j+1] = if mi == mj {
				solution[i][j] + 1
			} else {
				solution[i][j+1].max(solution[i+1][j])
			}
		}
	}

	// reconstitute the solution from the lengths
	let mut result: Vec<char> = Vec::new();
	let (mut i, mut j) = (len_a, len_b);

	while i > 0 && j > 0 {
		if a[i-1] == b[j-1] {
			result.push(a[i-1]);
			i -= 1;
			j -= 1;
		} else if solution[i-1][j] > solution[i][j-1] {
			i -= 1;
		} else {
			j -= 1;
		}
	}

	result.reverse();
	result.iter().collect()
}

pub fn maximum_subarray(array: &Vec<i32>) -> Vec<i32> {
	let mut result: Vec<i32> = Vec::new();
	let mut sum:i32 = 0;
	let mut max:i32 = 0;

	//
	// Solution result[] is a (sub)array of contiguous items in `array'
	// which add up to the maximum sum we can find.  So in
	//
	// [-2, 1, -3, 4, -1, 2, 1, -5, 4]
	//
	// you'd start at -2 (cross it out), proceed to 1, proceed to -3
	// (cross both 1 and -3 out since your sum == -2 at that point),
	// proceed to 4, to -1, to 2, to 1, to -5 and finally 4.  Your
	// maximum sum along that path was at [4, -1, 2, 1] resulting in
	// a max of 6, so you'd cross out -5 and 4 as well to keep a max of 6.
	//
	for item in array.iter() {
		// Move on to next item, push it onto `result', and recompute
		// `sum'.  If that results in `sum' < 0, clear the `result' array
		// and restart `sum' at 0.
		// 
		result.push(*item);
		sum += *item;
		if sum < 0 {
			result.clear();
			sum = 0;
			continue;
		}

		// If we find a new max, save it.
		if sum > max.try_into().unwrap() {
			max = sum;
		}
	}

	// eprintln!("after initial loop, sum = {} and max = {}", sum, max);

	//
	// After all candidate addends have been pushed onto `result', reverse
	// it and remove items (now from the front) until once again sum == max.
	// You can't modify `result' while iterating through it without violating
	// borrowing rules, so I'm just cloning it into `rev' and using that.
	//
	result.reverse();
	let rev = result.clone();

	for item in &rev {
		if sum == max {
			break;
		}
		sum -= item;
		result.remove(0);
	}
	// eprintln!("after &rev loop: result = {:?}", result);

	//
	// Re-reverse and return the final cooked result.
	// The instructor's original exercise was meant to just return `max',
	// so I'm printing that for free but the better exercise was to
	// return the maximum subarray itself.
	//
	result.reverse();
	eprintln!("after result reversal: result = {:?}, max = {}",
				result, max);
	result
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fib() {
        assert_eq!(fib(3), 2);
        assert_eq!(fib(4), 3);
        assert_eq!(fib(9), 34);
    }

	#[test]
	fn test_lcs() {
		assert_eq!(longest_common_subsequence("", ""), "");
		assert_eq!(longest_common_subsequence("", "abcd"), "");
		assert_eq!(longest_common_subsequence("asdf", ""), "");

		assert_eq!(longest_common_subsequence("abcd", "a"), "a");
		assert_eq!(longest_common_subsequence("adbc", "b"), "b");
		assert_eq!(longest_common_subsequence("qwerty", "rty"), "rty");

		assert_eq!(longest_common_subsequence("abcdgh", "aedfhr"), "adh");
		assert_eq!(longest_common_subsequence("aggtab", "gxtxayb"), "gtab");
	}

	#[test]
	fn test_max_subarray() {
		let mut test_vec: Vec<i32> = Vec::new();
		let mut test_res: Vec<i32> = Vec::new();

		test_vec.push(-2);
		test_vec.push(1);
		test_vec.push(-3);
		test_vec.push(4);
		test_vec.push(-1);
		test_vec.push(2);
		test_vec.push(1);
		test_vec.push(-5);
		test_vec.push(4);

		test_res.push(4);
		test_res.push(-1);
		test_res.push(2);
		test_res.push(1);

		assert_eq!(maximum_subarray(&test_vec), test_res);

		test_vec.clear();
		test_res.clear();

		test_vec.push(1);
		test_vec.push(2);
		test_vec.push(3);

		test_res.push(1);
		test_res.push(2);
		test_res.push(3);

		assert_eq!(maximum_subarray(&test_vec), test_res);
	}
}
