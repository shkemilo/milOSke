#ifndef LIST_H_
#define LIST_H_

template<class T>
class List {
private:
	struct Node {
			T* data;
			Node* previous, * next;

			//full constructor for a Node
			Node(T* data, Node* previous, Node* next) : data(data), previous(previous), next(next) {

			}

			//contructor which links the new node to the nextNode
			Node(T* data, Node* nextNode) : data(data), previous(nextNode->previous), next(nextNode) {
				if (previous) {
					previous->next = this;
				}
				next->previous = this;
			}

			//empty node constructor
			Node() : data(0), previous(0), next(0) {

			}
		};

public:
	class Iterator {
	public:
		Iterator(Node* node = 0) : node(node) { }

		T* getData() const {
			return node->data;
		}

		T& operator*() {
			return *(node->data);
		}

		Iterator& operator++() {
			node = node->next;
			return *this;
		}

		Iterator& operator--() {
			node = node->previous;
			return *this;
		}

		int operator==(const Iterator& other) const {
			return node == other.node;
		}

		int operator!=(const Iterator& other) const {
			return !operator==(other);
		}

	private:

		Node* node;
		friend class List<T>;
	};

	List() : head(new Node()), tail(head), count(0) { }
	~List();

	int empty() const {
		return count == 0;
	}

	int size() const {
		return count;
	}

	Iterator begin() {
		return Iterator(head);
	}

	Iterator end() {
		return Iterator(tail);
	}

	T& back() const {
		return *(tail->previous->data);
	}

	T& front() const {
		return *(head->data);
	}

	void push_front(T* data) {
		Node* n = new Node(data, head);
		head = n;
		++count;
	}

	void push_back(T* data) {
		Node* n = new Node(data, tail);
		if (head == tail)
			head = n;

		tail = n->next;
		++count;
	}

	T* pop_front() {
		Node* n = head;
		head->next->previous = 0;
		head = head->next;

		T* ret = n->data;
		--count;
		delete n;

		return ret;
	}

	T* pop_back() {
		Node* n = tail->previous;
		(n->previous != 0 ? n->previous->next : head) = tail;

		tail->previous = n->previous;

		T* ret = n->data;
		--count;
		delete n;

		return ret;
	}

	Iterator insert(const Iterator& iterator, T* data) {
		Node* n = new Node(data, iterator.node);
		if (iterator == begin())
			head = n;

		++count;
		return Iterator(n);
	}

	Iterator erase(const Iterator& iterator) {
		Node* current = iterator.node;
		if (current == tail) {
			return Iterator();
		}

		(current->previous != 0 ? current->previous->next : head) = current->next;

		current->next->previous = current->previous;

		Node* result = current->next;

		delete current;
		--count;
		return Iterator(result);
	}

private:

	Node* head, * tail;
	int count;
};

template<class T>
List<T>::~List() {
	while (head != 0) {
		Node* old = head;
		head = head->next;
		delete old;
	}

	count = 0;
	head = tail = 0;
}

#endif
