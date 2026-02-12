#[allow(dead_code)]
fn merge(arr: &mut [usize], mid: usize) {
	let left = arr[..mid].to_vec();		// left half - beginning to middle
	let right = arr[mid..].to_vec();	// right half - middle to end

	println!("merge: at top, left is {:?}", left);
	// println!("merge: at top, right is {:?}", right);
	if arr.len() == 2 {
		if arr[0] > arr[1] {
			arr.swap(0,1);
		}
		return;
	}
	println!("merge: at top, mid is {}", mid);
	let mut l = 0;
	let mut r = 0;

	for val in &mut *arr {
		if r == right.len() || (l < left.len() && left[l] < right[r]) {
			println!("merge: in arr loop, poking {} into arr", left[l]);
			*val = left[l];
			l += 1;
		} else {
			println!("merge: in arr loop, poking {} into arr", right[r]);
			*val = right[r];
			r += 1;
		}
	}

	// =============================================
	// array is [4, 5], returning immediately
	// =============================================
	// array is [7, 8], returning immediately
	// merge: at bottom, arr is: [4, 5, 8, 7]

	println!("merge: at bottom, arr is: {:?}", arr);
}

fn quick_sort(array: &mut Vec<usize>, _low: usize, _high: usize) -> Vec<usize> {
	let len = array.len();
	let pivot = array[len-1];	// arbitrary choice - last element of array
	let mut i:usize = 0;

	if len == 1 {
		println!("=============================================");
		println!("array is {:?}, returning immediately", array.to_vec());
		return array.to_vec();
	}

	if len == 2 {
		println!("=============================================");
		let slice2 = array;
		if slice2[0] > slice2[1] {
			slice2.swap(0,1);
		}
		println!("array is {:?}, returning immediately", slice2.to_vec());
		return slice2.to_vec();
	}

	let mut j:usize = len-2;

	println!("=============================================");
	println!("pivot is: {}", pivot);
	println!("array is {:?}", array);
	loop {
		println!("---------------------------------------------");
		println!("top of loop: array[{}] is {}, array[{}] is {}",
					i, array[i], j, array[j]);
		println!("top of loop: array is {:?}", array);

		if array[i] > pivot && array[j] < pivot {
			println!("swapping ...");
			array.swap(i,j);
			println!("after swap: array is {:?}", array);
		}

		if array[i] < pivot {
			if j > i+1 || (j == i+1 && array[i+1] > pivot) {
				i += 1;
			}
		}

		if array[j] > pivot {
			if j > i+1 || (j == i+1 && array[j-1] > pivot) {
				j -= 1;
			}
		}

		if i >= j {
			break;
		}
	}

	//
	// Now that i == j, time to swap the pivot with array[i]/array[j].
	//
	array.swap(i, len-1);
	println!("after pivot: array[{}] is {}, array[{}] is {}",
				i, array[i], j, array[j]);
	println!("after pivot: array is {:?}", array);

	//
	// Then recurse on the left and right halves.
	//
	// if len > 2 {
	let mid = len / 2;
	quick_sort(&mut array[..mid].to_vec(), _low, _high);	// left half
	quick_sort(&mut array[mid..].to_vec(), _low, _high);	// right half
	println!("prior to merge, array is {:?}", array.to_vec());
	merge(array.as_mut_slice(), mid);
	// }

	// println!("=============================================");
	println!("at bottom of quick_sort, array is {:?}", array.to_vec());

	array.to_vec()
}

fn main() {
	// Quick sort
	let mut vec = vec![8,5,1,2,7,3,4];
	let low = 3;
	let high = 7;
	quick_sort(&mut vec, low, high);
	println!("=============================================");
	println!("{:?}", vec);
	println!("XXX: Needs work to debug/finish (incorporate from instructor.rs)");
}
