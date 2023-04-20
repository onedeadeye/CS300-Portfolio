//============================================================================
// Name        : HashTable.cpp
// Author      : Liam Daley
// Version     : 1.0
// Copyright   : Copyright © 2023
// Description : Hash Table Courses
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// Define a structure to hold Course information
struct Course {
    string number; // unique identifier
    string name;
    vector<string> prerequisites = {};
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(string key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseNum);
    Course Search(string courseNum);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing with tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;
    // resize nodes size
    nodes.resize(size);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // Logic to free storage when class is destroyed
    nodes.erase(nodes.begin());
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(string key) {
    // Get only the last 4 elements of the course name
    string subs = key.substr(4);
    int courseID = atoi(subs.c_str());
    // Increment courseID by ASCII value of first character in number
    // to further differentiate course numbers
    courseID += key[0];

    return courseID % tableSize;

}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // create the key for the given course]
    unsigned int key = hash(course.number);
    // retrieve node using key
    Node* oldNode = &(nodes.at(key));
    // if no entry found for the key
    if (oldNode == nullptr) {
        // assign this node to the key position
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));
    }
    // else if node is not used
    else if (oldNode->key == UINT_MAX) {
        // assing old node key to UNIT_MAX, set to key, set old node to course and old node next to null pointer
        oldNode->key = key;
        oldNode->course = course;
        oldNode->next = nullptr;
    }
    // else find the next open node
    else {
        //traverse linked list to the end
        while (oldNode->next != nullptr) {
            
            oldNode = oldNode->next;
        }
        // add new Node to end
        oldNode->next = new Node(course, key);
    }
}

/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    // output course number, title, amount and fund
    string prereqs = "";
    for (string s : course.prerequisites) {
        prereqs += s + " ";
    }
    cout << "Number: " << course.number << " Name: " << course.name;
    if (prereqs != "") {
        cout << " Prerequisites: " << prereqs;
    }
    cout << endl;
    return;
}

/**
 * Partition the vector of courses into two parts, low and high
 *
 * @param courses Address of the vector<Bid> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int partition(vector<Course>& courses, int begin, int end) {
    //set low and high equal to begin and end
    int low = begin;
    int high = end;

    // pick the middle element as pivot point
    int midpoint = low + (high - low) / 2;
    string pivot = courses[midpoint].number;

    bool done = false;
    while (!done) {
        // keep incrementing low index while courses[low] < courses[pivot]
        while (courses[low].number < pivot) {
            ++low;
        }

        // keep decrementing high index while courses[pivot] < courses[high]
        while (pivot < courses[high].number) {
            --high;
        }

        /* If there are zero or one elements remaining,
            all courses are partitioned. Return high */
        if (low >= high) {
            done = true;
        }
        else {
            // else swap the low and high courses
            swap(courses[low], courses[high]);

            // move low and high closer ++low, --high
            ++low;
            --high;
        }
    }

    return high;
}

/**
 * Perform a quick sort on bid title
 * Average performance: O(n log(n))
 * Worst case performance O(n^2))
 *
 * @param courses address of the vector<Bid> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void quickSort(vector<Course>& courses, int begin, int end) {
    //set mid equal to 0
    int mid = 0;

    /* Base case: If there are 1 or zero courses to sort,
     partition is already sorted otherwise if begin is greater
     than or equal to end then return*/
    if (begin >= end) {
        return;
    }

    /* Partition courses into low and high such that
     midpoint is location of last element in low */
    mid = partition(courses, begin, end);

    // recursively sort low partition (begin to mid)
    quickSort(courses, begin, mid);

    // recursively sort high partition (mid+1 to end)
    quickSort(courses, mid + 1, end);
}

/**
 * Print all courses
 */
void HashTable::PrintAll() {
    std::vector<Course> courses;
    for (Node head : this->nodes) {
        if (head.key != UINT_MAX) {
            courses.push_back(head.course);
            Node* target = head.next;
            while (target != nullptr) {
                courses.push_back(target->course);
                target = target->next;
            }
        }
        
    }

    quickSort(courses, 0, courses.size() - 1);

    for (Course c : courses) {
        displayCourse(c);
    }

    return;
}

/**
 * Remove a course
 *
 * @param courseNum The course number to search for
 */
void HashTable::Remove(string courseNum) {
    // set key equal to hash atoi courseID cstring
    unsigned int key = hash(courseNum);
    Node* node = &(nodes.at(key));
    // erase node begin and key
    if (node->key != UINT_MAX) {
        if (node->course.number.compare(courseNum) == 0) {
            if (node->next == nullptr) {
                //set node as unused for later use
                node->key = UINT_MAX;
                return;
            }
            else {
                nodes.at(key) = *(node->next);
            }
        }
        else {
            Node* currentNode = node->next;
            Node* previousNode = node;
            while (currentNode != nullptr) {
                if (currentNode->course.number.compare(courseNum) == 0) {
                    previousNode->next = currentNode->next;
                    delete currentNode;
                    currentNode = nullptr;
                    return;
                }
                previousNode = currentNode;
                currentNode = currentNode->next;
            }
        }
    }
}

/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 */
Course HashTable::Search(string courseNum) {
    Course course;

    // create the key for the given course
    unsigned int key = hash(courseNum);
    Node* node = &(nodes.at(key));
    // if entry found for the key
    if (node != nullptr && node->key != UINT_MAX && node->course.number.compare(courseNum) == 0) {
        //First try!
        //return node course
        return node->course;
    }
    // if no entry found for the key
    if (node == nullptr || node->key == UINT_MAX) {
        // return course
        return course;
    }

    // while node not equal to nullptr
    while (node != nullptr) {
        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.number.compare(courseNum) == 0) {
            return node->course;
        }
        //node is equal to next node
        node = node->next;
    }

    return course;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the courses read
 */
void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    string line;
    ifstream file;
    file.open(csvPath);

    // If reading successful
    if (file.is_open()) {
        // Loop through each line in the input file
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            Course newCourse;

            // Parse number
            if (getline(ss, token, ',')) {
                newCourse.number = token;
            }
            else {
                // No course number found
                continue;
            }

            // Parse name
            if (getline(ss, token, ',')) {
                newCourse.name = token;

                // Parse prerequisites
                while (getline(ss, token, ',')) {
                    string prerequisite = token;
                    bool found = false;

                    // Check if prerequisite exists in input file
                    ifstream input_file("TestData.csv");
                    string input_line;

                    // Loop through input file, looking for number match
                    while (getline(input_file, input_line)) {
                        stringstream input_ss(input_line);
                        string input_token;

                        if (!getline(input_ss, input_token, ',')) {
                            continue;
                        }

                        if (input_token.compare(prerequisite) == 0) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        newCourse.prerequisites.push_back(prerequisite);
                    }
                    else {
                        // Prereq not found
                    }
                }

                hashTable->Insert(newCourse);
            }
            else {
                // Missing course name
            }
        }
        file.close();
    }
    else {
        cout << "Failed to open file " << csvPath << endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {
    cout << "Welcome to the course planner." << endl;

    // process command line arguments
    string csvPath, courseKey;
    csvPath = "TestData2.csv";
    courseKey = "MATH201";

    // Define a hash table to hold all the courses
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();

    int choice = 0;
    while (choice != 9) {
        cout << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << endl;
        cout << "What would you like to do? ";
        cin >> choice;

        switch (choice) {

        case 1:
            // Complete the method call to load the courses
            cout << "Enter file name to load from this directory:" << endl;
            cin >> csvPath;
            loadCourses(csvPath, courseTable);
            break;

        case 2:
            courseTable->PrintAll();
            break;

        case 3:
            cout << "What course do you want to know about? ";
            cin >> courseKey;
            // Force uppercase for course number
            transform(courseKey.begin(), courseKey.end(), courseKey.begin(), toupper);
            course = courseTable->Search(courseKey);

            if (!course.number.empty()) {
                displayCourse(course);
            }
            else {
                cout << "Course Number " << courseKey << " not found." << endl;
            }
            break;

        case 9:
            cout << "Good bye." << endl;

            return 0;
        default:
            cout << choice << " is not a valid option." << endl;
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
