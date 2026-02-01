// Create a macro called "op" that takes three arguments.  All three arguments
// will be integers, but the third argument is going to be how we tell the
// macro what operation to perform.
//
// Operations:
//
// 1 is add, 2 is subtract, 3 is multiply, 4 is divide, and 5 is mod.
//

macro_rules! op {
	($a: expr, $b: expr, $c: expr) => {
		{
			let m = $a;
			let n = $b;
			let operator = $c;
			let mut res = 0;

			match operator {
				1 => res = m + n,
				2 => {
					res = m - n
					},
				3 => res = m * n,
				4 => res = m / n,
				5 => res = m % n,
				_ => res = -1,
			}

			res
		}
	};
}

fn main() {
    println!("op!(5,2,1) is {}", op!(5,2,1)); //should print 7
    println!("op!(5,2,2) is {}", op!(5,2,2)); //should print 3
    println!("op!(5,2,3) is {}", op!(5,2,3)); //should print 10
    println!("op!(5,2,4) is {}", op!(5,2,4)); //should print 2
    println!("op!(5,2,5) is {}", op!(5,2,5)); //should print 1
    println!("op!(5,2,6) is {}", op!(5,2,6)); //should print -1
}
