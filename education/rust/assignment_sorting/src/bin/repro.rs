fn merge(arr: &mut [usize], mid: usize) {
	let left = arr[..mid].to_vec();
	let right = arr[mid..].to_vec();

	println!("merge: left.len is {}", left.len());
	println!("merge: right.len is {}", right.len());
	println!("merge: mid is {}", mid);
	assert!(arr.len() == 2);
	assert!(left.len() == 1);
	assert!(right.len() == 1);
	arr.swap(0,1);
	// let tmp = arr[0];
	// let arr[0] = arr[1];
	// let arr[1] = tmp;
	// assert!(left[0] <= right[0]);
	assert!(arr[0] <= arr[1]);
}

fn quick_sort(array: &mut Vec<usize>, _low: usize, _high: usize) -> Vec<usize> {
	let myarray = &mut Vec::new();
	myarray.push(8);
	myarray.push(7);
	let mid = myarray.len() / 2;
	println!("prior to merge, myarray is {:?}", myarray.to_vec());
	merge(myarray.as_mut_slice(), mid);
	assert!(myarray[0] <= myarray[1]);
	println!("after merge, myarray is {:?}", myarray.to_vec());

	myarray.to_vec()
}

fn main() {
	// Quick sort
	let mut vec = vec![8,5,1,2,7,3,4];
	let low = 3;
	let high = 7;
	quick_sort(&mut vec, low, high);
}
