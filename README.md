# C-PROGRAMMING-PROJECT
TEAM NAME:SSD
TEAM MEMBERS:
1)SUHAS TRIPURANA[IE2025031]
2)V.SUSHEEL VARMA[BA2025056]
3)T.VENKATA.SAI.DISEN[BE2025027]
PROJECT NAME:SIMPLE EMPLOYEE MANAGEMENT SYSTEM
FILES:
1)project2.c (PROJECT FILE)
2)employee1.csv
3)requests.csv{2 CSV FILES}
DESCRIPTION:
WE ARE USING 2 CSV FILES AS ONE FILE STORES ALL WORKERS DETAILS AND OTHER FILE STORES REQUESTS OF EMPLOYEES TO CHANGE THEIR NAME OR SALARY TO MANAGERS.DIRECTORS CAN ADD OR DELETE EMPLOYEES OR MANAGERS AND MANAGERS CAN EDIT OR ADD OR DELETE THEIR EMPLOYEES ONLY.EVERY EMPLOYEE HAS MANAGER ID WHICH IMPLIES HE WORKS UNDER THAT MANAGER AND MANAGERS MANAGER ID WILL BE DIRECTOR ID AS DIRECTOR ADDS MANAGERS.


ALL FUNCTIONS NAMES USED IN MY PROJECT ARE:(ALSO MENTIONED IN TOP OF PROJECT)
// Removes newline character from end of string if present
void trimNewline(char *str);

// Creates a new employee node with given data
struct Employee* createEmployee(int id, char *name, char *designation, float salary, char *password, int managerId);

// Adds a new employee node to the end of the linked list
void addEmployee(struct Employee **head, struct Employee *newEmp);

// Deletes an employee or manager from the list based on requester permissions
void deleteEmployee(struct Employee **head, int id, int requesterId, char *requesterRole);

// Displays all employees in the list in tabular form
void displayEmployees(struct Employee *head);

// Displays employees who report to a specific manager
void displayEmployeesUnderManager(struct Employee *head, int managerId);

// Displays details of a specific employee by id
void displayEmployee(struct Employee *head, int id);

// Saves the list of employees to a CSV file
void saveToFile(struct Employee *head, const char *filename);

// Loads employees from a CSV file into a linked list
struct Employee* loadFromFile(const char *filename);

// Handles user login by verifying id, name, password then initiating user session
int Login_handler(int id, char *name, const char *filename, struct Employee **present);

// Interactive function to add new employee based on logged-in user's role
// Checks if the managerId assigned to the new employee exists and is a manager
void addEmployeeInteractive(struct Employee **head, int loggedInUserId, char *loggedInDesignation);

// Handles logged-in user session and menu options based on role
void workholder(int userId, char name[], char designation[], struct Employee **head);


// UpdateRequest function declarations
struct UpdateRequest* createRequest(int empId, char *field, char *value, int managerId);
void addRequest(struct UpdateRequest **head, struct UpdateRequest *newReq);
struct UpdateRequest* loadRequests(const char *filename);
void saveRequests(struct UpdateRequest *head, const char *filename);
void submitUpdateRequest(struct UpdateRequest **reqHead, int empId, int managerId);
void viewAndProcessRequests(struct UpdateRequest **reqHead, struct Employee **empHead, int reviewerId, char *designation);

FOLLOWING HAS BEEN USED:
STRUCTURES
LINKED LISTS
CSV FILES
[NOTE:ITS CLEAR THAT FIRST RECORD OF EMPLOYEE SHOULD BE SAVED ALREADY MANUALLY AS DIRECTOR MUST BE PRESENT IN FILE ALREADY]
THANK YOU