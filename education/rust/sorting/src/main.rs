#[allow(dead_code)]
fn selection_sort(array: &mut Vec<i8>) -> Vec<i8> {
	for i in 0..array.len() - 1 {
		let mut smallest = i;
		for j in (i+1)..array.len() {
			if array[j] < array[smallest] {
				smallest = j;
			}
		}
		array.swap(smallest, i);
	}
	array.to_vec()
}

#[allow(dead_code)]
fn nosuch(_array: &mut Vec<i8>) {	// intentionally not allowed as dead code
}

#[allow(dead_code)]
// #[allow(unused_variables)]
#[allow(unused_assignments)]
// #[allow(unused)]
fn bubble_sort(array: &mut Vec<i8>) -> Vec<i8> {
	let mut sorted = true;

	for _ in 1..=array.len() - 1 {
		sorted = true;
		for j in 0..=array.len() - 2 {
			if array[j] > array[j + 1] {
				array.swap(j, j+1);
				sorted = false;
			}
		}

		if sorted {		// small optimization to be had here
			break;
		}
	}

	sorted = sorted;
	array.to_vec()
}

#[allow(dead_code)]
fn merge_sort(arr: &mut [i32]) -> Vec<i32> {
	if arr.len() > 1 {
		let mid = arr.len() / 2;
		merge_sort(&mut arr[..mid]);	// left half
		merge_sort(&mut arr[mid..]);	// right half
		// merge(arr, mid);
	}

	arr.to_vec()
}

#[allow(dead_code)]
fn merge(arr: &mut [usize], mid: usize) {
	let left = arr[..mid].to_vec();		// left half - beginning to middle
	let right = arr[mid..].to_vec();	// right half - middle to end

	let mut l = 0;
	let mut r = 0;

	for val in arr {
		if r == right.len() || (l < left.len() && left[l] < right[r]) {
			*val = left[l];
			l += 1;
		} else {
			*val = right[r];
			r += 1;
		}
	}
}

fn quick_sort(array: &mut Vec<usize>, _low: usize, _high: usize) -> Vec<usize> {
	let len = array.len();
	let pivot = array[len-1];		// arbitrary
	let mut i:usize = 0;

	if len == 1 {
		println!("=============================================");
		println!("array is {:?}, returning immediately", array);
		return array.to_vec();
	}

	if len == 2 {
		if array[0] > array[1] {
			array.swap(0,1);
		}
		println!("=============================================");
		println!("array is {:?}, returning immediately", array);
		return array.to_vec();
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
	if len > 2 {
		let mid = len / 2;
		quick_sort(&mut array[..mid].to_vec(), _low, _high);	// left half
		quick_sort(&mut array[mid..].to_vec(), _low, _high);	// right half
		merge(array.as_mut_slice(), mid);
	}

	array.to_vec()
}

#[allow(dead_code)]
#[allow(dead_code)]
// #[allow(unused_variables)]
// #[allow(unused_assignments)]
// #[allow(unused)]
fn partition() {
}

fn main() {
	// Selection sort
    // let mut arr: Vec<i8> = vec![4,3,1,2];

	// println!("Before sorting: {:?}", arr);
	// selection_sort(&mut arr);
	// println!("After sorting: {:?}", arr);

	// let mut arr2: Vec<i8> = vec![5,10,1,4,11];
	// println!("Before sorting: {:?}", arr2);
	// selection_sort(&mut arr2);
	// println!("After sorting: {:?}", arr2);

	// Bubble sort
	// let mut vec = vec![5,4,1,3,2];
	// bubble_sort(&mut vec);
	// println!("{:?}", vec);

	// let mut vec2 = vec![5,1,4,2,8];
	// bubble_sort(&mut vec2);
	// println!("{:?}", vec2);

	// Merge sort
	// let mut vec = vec![4,7,3,5,1,2];
	// merge_sort(&mut vec);
	// println!("{:?}", vec);

	// Quick sort
	let mut vec = vec![8,5,1,2,7,3,4];
	let low = 3;
	let high = 7;
	quick_sort(&mut vec, low, high);
	println!("=============================================");
	println!("{:?}", vec);
}
