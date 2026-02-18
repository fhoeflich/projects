use std::collections::LinkedList;

pub struct Queue<T> {
	element: LinkedList<T>,
}

impl<T> Queue<T> {
	pub fn new() -> Queue<T> {
		Queue {
			element: LinkedList::new(),
		}
	}

	pub fn enqueue(&mut self, value: T) {
		self.element.push_back(value)
	}

	pub fn dequeue(&mut self) -> Option<T> {
		self.element.pop_front()
	}

	pub fn peek(&mut self) -> Option<&T> {
		self.element.front()
	}

	pub fn length(&self) -> usize {
		self.element.len()
	}

	pub fn is_empty(&self) -> bool {
		self.element.is_empty()
	}
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn basics() {
        let mut queue = Queue::new();

        // Check if empty queue behaves correctly
        assert_eq!(queue.dequeue(), None);
		assert!(queue.is_empty());		// queue should be empty ...

        // Populate queue
        queue.enqueue(1);
		assert!(!queue.is_empty());		// queue should no longer be empty.
        queue.enqueue(2);
        queue.enqueue(3);

		// Check length of queue
		assert_eq!(queue.length(), 3);

        // Check normal removal
        assert_eq!(queue.dequeue(), Some(1));
        assert_eq!(queue.dequeue(), Some(2));

        // Push some more values on
        queue.enqueue(4);
        queue.enqueue(5);

        // Check normal removal
        assert_eq!(queue.dequeue(), Some(3));
        assert_eq!(queue.dequeue(), Some(4));

        // Exhaustive removal check
        assert_eq!(queue.dequeue(), Some(5));
        assert_eq!(queue.dequeue(), None);

		// Check is_empty() functionality
		assert!(queue.is_empty());
    }
}

#[test]
fn peek() {
	let mut queue = Queue::new();

	assert_eq!(queue.peek(), None);

	queue.enqueue(1);
	queue.enqueue(2);
	queue.enqueue(3);
	queue.enqueue(4);
	queue.enqueue(5);

	assert_eq!(queue.peek(), Some(&1));
	queue.dequeue();

	assert_eq!(queue.peek(), Some(&2));
	queue.dequeue();

	assert_eq!(queue.peek(), Some(&3));
	queue.dequeue();

	assert_eq!(queue.peek(), Some(&4));
	queue.dequeue();

	assert_eq!(queue.peek(), Some(&5));
	queue.dequeue();

	assert!(queue.is_empty());
	assert_eq!(queue.peek(), None);
}
