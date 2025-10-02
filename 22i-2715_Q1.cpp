#include <iostream>
#include <string>
using namespace std;

// Forward declarations
class CircularLinkedList;
class DoublyLinkedList;
class TicketBooth;

// Fan structure
struct Fans {
    string fan_id;
    string fan_name;
    string fan_type;
    int fan_priority;
    Fans* add;
};

// Seat structure
struct Seat {
    int seat_id;
    Fans* occupant;
    Seat* add;

    Seat() : seat_id(0), occupant(nullptr), add(nullptr) {}
};

// Node structure for rows
struct Node {
    CircularLinkedList* row;
    Node* left;
    Node* right;

    Node() : row(nullptr), left(nullptr), right(nullptr) {}
};

// Circular Linked List for seats in a row
class CircularLinkedList {
private:
    Seat* circular_head;

public:
    CircularLinkedList() : circular_head(nullptr) {}

    ~CircularLinkedList() {
        if (circular_head) {
            Seat* current = circular_head->add;
            while (current != circular_head) {
                Seat* temp = current;
                current = current->add;
                delete temp;
            }
            delete circular_head;
        }
    }

    void addSeats(int id) {
        Seat* newnode = new Seat;
        newnode->seat_id = id;

        if (circular_head == nullptr) {
            circular_head = newnode;
            newnode->add = circular_head;
        }
        else {
            Seat* nodeptr = circular_head;
            while (nodeptr->add != circular_head) {
                nodeptr = nodeptr->add;
            }
            nodeptr->add = newnode;
            newnode->add = circular_head;
        }
    }

    void display() {
        if (circular_head == nullptr) {
            cout << "Empty row.\n";
            return;
        }
        Seat* nodeptr = circular_head;
        do {
            cout << "Seat ID: " << nodeptr->seat_id;
            if (nodeptr->occupant != nullptr) {
                cout << ", Occupied by: " << nodeptr->occupant->fan_name;
            }
            cout << "\n";
            nodeptr = nodeptr->add;
        } while (nodeptr != circular_head);
    }

    Seat* getHead() { return circular_head; }
};

// Doubly Linked List for stadium rows
class DoublyLinkedList {
private:
    Node* doubly_head;

public:
    DoublyLinkedList() : doubly_head(nullptr) {}

    ~DoublyLinkedList() {
        Node* current = doubly_head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->right;
            delete temp->row;
            delete temp;
        }
    }

    void addRow() {
        Node* newnode = new Node;
        newnode->row = new CircularLinkedList();

        if (doubly_head == nullptr) {
            doubly_head = newnode;
        }
        else {
            Node* last = doubly_head;
            while (last->right != nullptr) {
                last = last->right;
            }
            last->right = newnode;
            newnode->left = last;
        }
    }

    void display() {
        if (doubly_head == nullptr) {
            cout << "The stadium has no rows.\n";
            return;
        }
        Node* nodeptr = doubly_head;
        int rowNum = 1;
        while (nodeptr != nullptr) {
            cout << "Row " << rowNum++ << ":\n";
            nodeptr->row->display();
            cout << "--------------------------\n";
            nodeptr = nodeptr->right;
        }
    }

    int totalRows() {
        int count = 0;
        Node* nodeptr = doubly_head;
        while (nodeptr != nullptr) {
            count++;
            nodeptr = nodeptr->right;
        }
        return count;
    }

    void displaySeat(int row_num, int seat_id) {
        if (row_num > totalRows() || row_num < 1) {
            cout << "Invalid row number! Please try again." << endl;
            return;
        }

        Node* row = doubly_head;
        for (int i = 1; i < row_num; ++i) {
            row = row->right;
        }

        Seat* seat = row->row->getHead();
        do {
            if (seat->seat_id == seat_id) {
                cout << "Seat ID: " << seat->seat_id << endl;
                if (seat->occupant != nullptr) {
                    cout << "Occupied by: " << seat->occupant->fan_name
                        << ", ID: " << seat->occupant->fan_id << endl;
                }
                else {
                    cout << "Seat is unoccupied.\n";
                }
                return;
            }
            seat = seat->add;
        } while (seat != row->row->getHead());

        cout << "Seat ID not found in Row " << row_num << ".\n";
    }

    Node* getHead() { return doubly_head; }
};

// Ticket Booth class for managing fan queue
class TicketBooth {
private:
    Fans* front;
    Fans* rear;

public:
    TicketBooth() : front(nullptr), rear(nullptr) {}

    ~TicketBooth() {
        if (front == nullptr) return;

        // Handle single node case
        if (front == front->add) {
            delete front;
            front = rear = nullptr;
            return;
        }

        // Handle multiple nodes
        Fans* current = front;
        rear->add = nullptr;  // Break the circular link
        while (current != nullptr) {
            Fans* temp = current;
            current = current->add;
            delete temp;
        }
        front = rear = nullptr;
    }

    void enqueue(string id, string name, string type, int p) {
        Fans* newnode = new Fans;
        newnode->fan_id = id;
        newnode->fan_name = name;
        newnode->fan_type = type;
        newnode->fan_priority = p;
        newnode->add = nullptr;

        if (front == nullptr) {
            front = rear = newnode;
            rear->add = front;  // Create circular link
            return;
        }

        // Handle priority insertion
        if (p < front->fan_priority) {
            newnode->add = front;
            front = newnode;
            rear->add = front;  // Update circular link
            return;
        }

        Fans* current = front;
        while (current->add != front && current->add->fan_priority <= p) {
            current = current->add;
        }

        newnode->add = current->add;
        current->add = newnode;

        // Update rear if necessary
        if (current == rear) {
            rear = newnode;
            rear->add = front;  // Maintain circular link
        }
    }

    void dequeue(DoublyLinkedList& stadium) {
        if (front == nullptr) {
            cout << "The queue is empty.\n";
            return;
        }

        Node* row = stadium.getHead();
        bool seatFound = false;

        while (row != nullptr && !seatFound) {
            Seat* seat = row->row->getHead();
            if (seat != nullptr) {
                Seat* currentSeat = seat;
                do {
                    if (currentSeat->occupant == nullptr) {
                        // Create a deep copy of the fan
                        Fans* newFan = new Fans;
                        newFan->fan_id = front->fan_id;
                        newFan->fan_name = front->fan_name;
                        newFan->fan_type = front->fan_type;
                        newFan->fan_priority = front->fan_priority;
                        newFan->add = nullptr;

                        currentSeat->occupant = newFan;

                        cout << "Name: " << front->fan_name
                            << ", ID: " << front->fan_id
                            << ", Priority: " << front->fan_priority
                            << " received ticket and was assigned to Seat " << currentSeat->seat_id << ".\n";

                        // Remove fan from queue
                        Fans* temp = front;
                        if (front == rear) {  // Only one node
                            front = rear = nullptr;
                        }
                        else {
                            front = front->add;
                            rear->add = front;  // Maintain circular link
                        }
                        delete temp;

                        seatFound = true;
                        break;
                    }
                    currentSeat = currentSeat->add;
                } while (currentSeat != seat && !seatFound);
            }
            if (!seatFound) {
                row = row->right;
            }
        }

        if (!seatFound) {
            cout << "No available seats for the fan.\n";
        }
    }

    void display() {
        if (front == nullptr) {
            cout << "The queue is empty.\n";
            return;
        }

        Fans* current = front;
        do {
            cout << "***************************************\n";
            cout << "ID: " << current->fan_id << "\n";
            cout << "Name: " << current->fan_name << "\n";
            cout << "Type: " << current->fan_type << "\n";
            cout << "Priority: " << current->fan_priority << "\n";
            current = current->add;
        } while (current != front && current != nullptr);
        cout << "***************************************\n";
    }
};

int main() {
    TicketBooth booth;
    DoublyLinkedList stadium;
    int choice;

    while (true) {
        cout << "\nStadium Ticket System\n";
        cout << "1. Add a fan\n";
        cout << "2. Process tickets\n";
        cout << "3. Print queue\n";
        cout << "4. Add row\n";
        cout << "5. Print rows\n";
        cout << "6. Add seat to row\n";
        cout << "7. Display row\n";
        cout << "8. Display seat\n";
        cout << "9. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string id, name, type;
            int priority;
            cout << "Enter fan ID: "; cin >> id;
            cout << "Enter fan name: "; cin >> name;
            cout << "Enter fan type: "; cin >> type;
            do {
                cout << "Enter priority (1-5): ";
                cin >> priority;
            } while (priority < 1 || priority > 5);
            booth.enqueue(id, name, type, priority);
            break;
        }
        case 2:
            booth.dequeue(stadium);
            break;
        case 3:
            booth.display();
            break;
        case 4:
            stadium.addRow();
            cout << "Row added successfully.\n";
            break;
        case 5:
            stadium.display();
            break;
        case 6: {
            int row_num, seat_id;
            cout << "Enter row number: "; cin >> row_num;
            cout << "Enter seat ID: "; cin >> seat_id;

            if (row_num <= stadium.totalRows() && row_num > 0) {
                Node* row = stadium.getHead();
                for (int i = 1; i < row_num; ++i) {
                    row = row->right;
                }
                row->row->addSeats(seat_id);
                cout << "Seat added successfully.\n";
            }
            else {
                cout << "Invalid row number!\n";
            }
            break;
        }
        case 7: {
            int row_num;
            cout << "Enter row number to display: ";
            cin >> row_num;

            if (row_num <= stadium.totalRows() && row_num > 0) {
                Node* row = stadium.getHead();
                for (int i = 1; i < row_num; ++i) {
                    row = row->right;
                }
                row->row->display();
            }
            else {
                cout << "Invalid row number!\n";
            }
            break;
        }
        case 8: {
            int row_num, seat_id;
            cout << "Enter row number: "; cin >> row_num;
            cout << "Enter seat ID: "; cin >> seat_id;
            stadium.displaySeat(row_num, seat_id);
            break;
        }
        case 9:
            return 0;
        default:
            cout << "Invalid choice!\n";
        }
    }
}