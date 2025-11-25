#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Employee Struct to hold employee details including manager relationship
struct Employee {
    int id;
    char name[50];
    char designation[30];
    float salary;
    char password[20];
    int managerId;
    struct Employee *next;
};

// UpdateRequest Struct to hold requests for updating employee data
struct UpdateRequest {
    int employeeId;
    char field[30];
    char newValue[50];
    char status[20];
    int managerId;
    struct UpdateRequest *next;
};

// Compares two strings case-insensitively. Returns 0 if they are equal.
int caseInsensitiveCmp(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower(*a) != tolower(*b))
            return (unsigned char)tolower(*a) - (unsigned char)tolower(*b);
        a++; b++;
    }
    return (unsigned char)tolower(*a) - (unsigned char)tolower(*b);
}

// Checks if password satisfies: at least one letter, one digit, and one symbol (@, $, &)
int isValidPassword(const char *password) {
    int hasLetter = 0, hasDigit = 0, hasSymbol = 0;
    const char *symbols = "@$&";
    while (*password) {
        if (isalpha(*password)) hasLetter = 1;
        else if (isdigit(*password)) hasDigit = 1;
        else if (strchr(symbols, *password)) hasSymbol = 1;
        password++;
    }
    return hasLetter && hasDigit && hasSymbol;
}

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

// Allows any logged-in user to change their password with validation
void changePassword(struct Employee *head, int empId);

// Handles logged-in user session and menu options based on role
void workholder(int userId, char name[], char designation[], struct Employee **head);


// UpdateRequest function declarations
struct UpdateRequest* createRequest(int empId, char *field, char *value, int managerId);
void addRequest(struct UpdateRequest **head, struct UpdateRequest *newReq);
struct UpdateRequest* loadRequests(const char *filename);
void saveRequests(struct UpdateRequest *head, const char *filename);
void submitUpdateRequest(struct UpdateRequest **reqHead, int empId, int managerId);
void viewAndProcessRequests(struct UpdateRequest **reqHead, struct Employee **empHead, int reviewerId, char *designation);

// Global head pointer for update requests
struct UpdateRequest *requestsHead = NULL;

// Helper function to find managerId of an employee by id
int findManagerId(struct Employee *head, int empId) {
    struct Employee *temp = head;
    while (temp != NULL) {
        if (temp->id == empId) return temp->managerId;
        temp = temp->next;
    }
    return -1; // not found
}

// Helper to verify if a given managerId exists and is designated "manager"
int managerExists(struct Employee *head, int managerId) {
    struct Employee *temp = head;
    while (temp != NULL) {
        if (temp->id == managerId && caseInsensitiveCmp(temp->designation, "manager") == 0) {
            return 1; // manager found
        }
        temp = temp->next;
    }
    return 0; // manager not found
}


// ----------- Implementations -------------

void trimNewline(char *str) {
    size_t len = strlen(str);
    if(len > 0 && str[len-1] == '\n')
        str[len-1] = '\0';
}

struct Employee* createEmployee(int id, char *name, char *designation, float salary, char *password, int managerId) {
    struct Employee *newEmp = (struct Employee *)malloc(sizeof(struct Employee));
    if (!newEmp) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    newEmp->id = id;
    strcpy(newEmp->name, name);
    strcpy(newEmp->designation, designation);
    newEmp->salary = salary;
    strcpy(newEmp->password, password);
    newEmp->managerId = managerId;
    newEmp->next = NULL;
    return newEmp;
}

void addEmployee(struct Employee **head, struct Employee *newEmp) {
    if (*head == NULL) {
        *head = newEmp;
    } else {
        struct Employee *temp = *head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newEmp;
    }
}

void deleteEmployee(struct Employee **head, int id, int requesterId, char *requesterRole) {
    struct Employee *temp = *head, *prev = NULL;
    while (temp != NULL) {
        if (temp->id == id) {
            if (strcmp(requesterRole, "director") == 0 && caseInsensitiveCmp(temp->designation, "manager") == 0) {
                if (prev == NULL) *head = temp->next;
                else prev->next = temp->next;
                free(temp);
                printf("Manager with ID %d deleted successfully.\n", id);
                return;
            } else if (strcmp(requesterRole, "manager") == 0 && caseInsensitiveCmp(temp->designation, "employee") == 0 && temp->managerId == requesterId) {
                if (prev == NULL) *head = temp->next;
                else prev->next = temp->next;
                free(temp);
                printf("Employee with ID %d deleted successfully.\n", id);
                return;
            }
            else {
                printf("You don't have permission to delete this employee or manager.\n");
                return;
            }
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Employee or Manager with ID %d not found.\n");
}

void displayEmployees(struct Employee *head) {
    printf("ID\tName\t\tDesignation\tSalary\tManagerID\n");
    printf("------------------------------------------------\n");
    while (head != NULL) {
        printf("%d\t%-15s%-12s%.2f\t%d\n", head->id, head->name, head->designation, head->salary, head->managerId);
        head = head->next;
    }
}

void displayEmployeesUnderManager(struct Employee *head, int managerId) {
    printf("ID\tName\t\tDesignation\tSalary\n");
    printf("-----------------------------------------\n");
    while (head != NULL) {
        if (head->managerId == managerId)
            printf("%d\t%-15s%-12s%.2f\n", head->id, head->name, head->designation, head->salary);
        head = head->next;
    }
}

void displayEmployee(struct Employee *head, int id) {
    while (head != NULL) {
        if (head->id == id) {
            printf("\nYour Details:\nID: %d\nName: %s\nDesignation: %s\nSalary: %.2f\nManager ID: %d\n",
                   head->id, head->name, head->designation, head->salary, head->managerId);
            return;
        }
        head = head->next;
    }
    printf("Your employee record not found!\n");
}

void saveToFile(struct Employee *head, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Failed to open %s for writing\n", filename);
        return;
    }
    fprintf(file, "ID,Name,Designation,Salary,Password,ManagerID\n");
    while (head != NULL) {
        fprintf(file, "%d,%s,%s,%.2f,%s,%d\n", head->id, head->name, head->designation, head->salary, head->password, head->managerId);
        head = head->next;
    }
    fclose(file);
}

struct Employee* loadFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;
    char header[256];
    fgets(header, sizeof(header), file); // skip header
    struct Employee *head = NULL;
    int id, managerId;
    float salary;
    char name[50], designation[30], password[20];
    while (fscanf(file, "%d,%49[^,],%29[^,],%f,%19[^,],%d\n", &id, name, designation, &salary, password, &managerId) == 6) {
        struct Employee* newEmp = createEmployee(id, name, designation, salary, password, managerId);
        addEmployee(&head, newEmp);
    }
    fclose(file);
    return head;
}

// Updated Login_handler to be case-insensitive for name comparison
int Login_handler(int id, char *name, const char *filename, struct Employee **present) {
    if (*present == NULL) {
        *present = loadFromFile(filename);
        if (!*present) {
            printf("No employees loaded. Check the CSV file.\n");
            return 0;
        }
    }
    struct Employee *temp = *present;
    char choice;
    while (temp != NULL) {
        if (temp->id == id && caseInsensitiveCmp(temp->name, name) == 0) {
            char passw[20];
            do {
                printf("USER FOUND! \nENTER PASSWORD HERE: ");
                scanf("%19s", passw);
                if (strcmp(temp->password, passw) == 0) {
                    printf("WELCOME Mr/Mrs.%s, OUR CURRENT %s\n", temp->name, temp->designation);
                    workholder(temp->id, temp->name, temp->designation, present);
                    saveToFile(*present, filename);
                    saveRequests(requestsHead, "requests.csv");
                    return 1;
                }
                else {
                    printf("INCORRECT PASSWORD.\n");
                    printf("WANNA TRY AGAIN?(y/n): ");
                    scanf(" %c", &choice);
                    if (choice == 'n' || choice == 'N') return 0;
                }
            } while (choice == 'y' || choice == 'Y');
            return 0;
        }
        temp = temp->next;
    }
    printf("User not found or login failed.\n");
    return 0;
}

// Adds a new employee interactively, validating the manager ID for existence and role
void addEmployeeInteractive(struct Employee **head, int loggedInUserId, char *loggedInDesignation) {
    if (caseInsensitiveCmp(loggedInDesignation, "employee") == 0) {
        printf("You don't have permission to add an employee.\n");
        return;
    }
    int id;
    char name[50], designation[30], password[20];
    float salary;
    int managerId;

    printf("Enter new employee ID: ");
    scanf("%d", &id);
    printf("Enter new employee name: ");
    scanf("%s", name);

    if (caseInsensitiveCmp(loggedInDesignation, "director") == 0) {
        printf("Enter designation (manager/employee): ");
        scanf("%s", designation);
        if (caseInsensitiveCmp(designation, "manager") == 0) {
            managerId = loggedInUserId;
        } else {
            printf("Enter manager ID for this employee: ");
            scanf("%d", &managerId);
            // Validate if managerId exists and is a manager
            if (!managerExists(*head, managerId)) {
                printf("Manager ID not found or invalid. Cannot add employee.\n");
                return;
            }
        }
    } else if (caseInsensitiveCmp(loggedInDesignation, "manager") == 0) {
        strcpy(designation, "employee");
        managerId = loggedInUserId;
    }

    printf("Enter salary: ");
    scanf("%f", &salary);

    // Password input and validation loop
    while (1) {
        printf("Enter password (must contain at least one letter, one digit, and one symbol @,$,&): ");
        scanf("%s", password);
        if (isValidPassword(password)) {
            break;
        } else {
            printf("Invalid password, please try again.\n");
        }
    }
    
    struct Employee *newEmp = createEmployee(id, name, designation, salary, password, managerId);
    addEmployee(head, newEmp);
    printf("Employee added successfully.\n");
}

// Allows any logged-in user to change their password with validation
void changePassword(struct Employee *head, int empId) {
    char newPassword[20];
    struct Employee *emp = head;
    while (emp != NULL) {
        if (emp->id == empId) {
            while (1) {
                printf("Enter new password (must contain at least one letter, one digit, and one symbol @,$,&): ");
                scanf("%19s", newPassword);
                if (isValidPassword(newPassword)) {
                    strcpy(emp->password, newPassword);
                    printf("Password changed successfully.\n");
                    return;
                } else {
                    printf("Password does not meet criteria, please try again.\n");
                }
            }
        }
        emp = emp->next;
    }
    printf("Employee record not found. Password not changed.\n");
}


// --------- UpdateRequest Functions ---------

struct UpdateRequest* createRequest(int empId, char *field, char *value, int managerId) {
    struct UpdateRequest *newReq = (struct UpdateRequest*)malloc(sizeof(struct UpdateRequest));
    if (!newReq) {
        printf("Memory allocation failed for new request.\n");
        return NULL;
    }
    newReq->employeeId = empId;
    strcpy(newReq->field, field);
    strcpy(newReq->newValue, value);
    strcpy(newReq->status, "pending");
    newReq->managerId = managerId;
    newReq->next = NULL;
    return newReq;
}

void addRequest(struct UpdateRequest **head, struct UpdateRequest *newReq) {
    if (*head == NULL) {
        *head = newReq;
    } else {
        struct UpdateRequest *temp = *head;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newReq;
    }
}

struct UpdateRequest* loadRequests(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;
    struct UpdateRequest *head = NULL;
    int empId, managerId;
    char field[30], value[50], status[20];
    while (fscanf(file, "%d,%29[^,],%49[^,],%19[^,],%d\n", &empId, field, value, status, &managerId) == 5) {
        struct UpdateRequest *newReq = createRequest(empId, field, value, managerId);
        if (!newReq) continue;
        strcpy(newReq->status, status);
        addRequest(&head, newReq);
    }
    fclose(file);
    return head;
}

void saveRequests(struct UpdateRequest *head, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return;
    while (head != NULL) {
        fprintf(file, "%d,%s,%s,%s,%d\n", head->employeeId, head->field, head->newValue, head->status, head->managerId);
        head = head->next;
    }
    fclose(file);
}

void submitUpdateRequest(struct UpdateRequest **reqHead, int empId, int managerId) {
    char field[30], value[50];
    printf("Enter the field you want to update (name/salary): ");
    scanf("%s", field);
    printf("Enter the new value: ");
    scanf("%s", value);
    struct UpdateRequest *newReq = createRequest(empId, field, value, managerId);
    if (!newReq) return;
    addRequest(reqHead, newReq);
    printf("Your update request has been submitted for approval.\n");
}

void viewAndProcessRequests(struct UpdateRequest **reqHead, struct Employee **empHead, int reviewerId, char *designation) {
    struct UpdateRequest *temp = *reqHead;
    int found = 0;
    while (temp != NULL) {
        if ((caseInsensitiveCmp(designation, "manager") == 0 && temp->managerId == reviewerId && caseInsensitiveCmp(temp->status, "pending") == 0) ||
            (caseInsensitiveCmp(designation, "director") == 0 && caseInsensitiveCmp(temp->status, "pending") == 0)) {

            printf("\nRequest for Employee ID: %d\nField: %s\nNew Value: %s\nStatus: %s\n",
                   temp->employeeId, temp->field, temp->newValue, temp->status);
            printf("Approve (1), Reject (0), Skip (2): ");
            int choice; scanf("%d", &choice);
            if (choice == 1) {
                struct Employee *emp = *empHead;
                while (emp != NULL) {
                    if (emp->id == temp->employeeId) {
                        if (caseInsensitiveCmp(temp->field, "name") == 0) strcpy(emp->name, temp->newValue);
                        else if (caseInsensitiveCmp(temp->field, "salary") == 0) emp->salary = atof(temp->newValue);
                        strcpy(temp->status, "approved");
                        printf("Request approved and record updated.\n");
                        found = 1;
                        break;
                    }
                    emp = emp->next;
                }
            } else if (choice == 0) {
                strcpy(temp->status, "rejected");
                printf("Request rejected.\n");
                found = 1;
            }
        }
        temp = temp->next;
    }
    if (!found) printf("No pending requests to review.\n");
}

// Handles user commands and menu options based on their role during session
void workholder(int userId, char name[], char designation[], struct Employee **head) {
    int choice;
    printf("\nWelcome %s! You are logged in as %s.\n", name, designation);
    while (1) {
        printf("\nSelect your operation:\n");
        if (caseInsensitiveCmp(designation, "director") == 0) {
            printf("1. View All Employees and Managers\n");
            printf("2. Add Manager Or Employee\n");
            printf("3. Delete Manager\n");
            printf("4. Review Update Requests\n");
            printf("5. Change Password\n");
        }
        else if (caseInsensitiveCmp(designation, "manager") == 0) {
            printf("1. View Your Employees\n");
            printf("2. Add Employee\n");
            printf("3. Delete Employee\n");
            printf("4. Review Update Requests\n");
            printf("5. Change Password\n");
        }
        else if (caseInsensitiveCmp(designation, "employee") == 0) {
            printf("1. View Your Details\n");
            printf("2. Submit Update Request\n");
            printf("3. Change Password\n");
        }
        else {
            printf("Invalid designation.\n");
            return;
        }
        printf("0. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            if (caseInsensitiveCmp(designation, "director") == 0)
                displayEmployees(*head);
            else if (caseInsensitiveCmp(designation, "manager") == 0)
                displayEmployeesUnderManager(*head, userId);
            else if (caseInsensitiveCmp(designation, "employee") == 0)
                displayEmployee(*head, userId);
            break;
        case 2:
            if (caseInsensitiveCmp(designation, "manager") == 0 || caseInsensitiveCmp(designation, "director") == 0)
                addEmployeeInteractive(head, userId, designation);
            else if (caseInsensitiveCmp(designation, "employee") == 0) {
                int mgrId = findManagerId(*head, userId);
                submitUpdateRequest(&requestsHead, userId, mgrId);
            }
            else
                printf("You do not have permission to add employees.\n");
            break;
        case 3:
            if (caseInsensitiveCmp(designation, "director") == 0) {
                printf("Enter Manager or Employee ID to delete: ");
                int delId;
                scanf("%d", &delId);
                deleteEmployee(head, delId, userId, designation);
            }
            else if (caseInsensitiveCmp(designation, "manager") == 0) {
                printf("Enter Employee ID to delete: ");
                int delId;
                scanf("%d", &delId);
                deleteEmployee(head, delId, userId, designation);
            }
            else {
                printf("You do not have permission to delete employees.\n");
            }
            break;
        case 4:
            if (caseInsensitiveCmp(designation, "director") == 0 || caseInsensitiveCmp(designation, "manager") == 0)
                viewAndProcessRequests(&requestsHead, head, userId, designation);
            else
                printf("Invalid choice.\n");
            break;
        case 5:
            // Change password for any logged-in user
            changePassword(*head, userId);
            break;
        case 0:
            printf("Saving data and logging out...\n");
            saveToFile(*head, "employee1.csv");
            saveRequests(requestsHead, "requests.csv");
            return;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }
}

// ------------- main function -------------
// Starts the program, handles login loop until exit is requested
int main() {
    const char *filename = "employee1.csv";
    const char *requestFile = "requests.csv";

    // Load employees and requests from files
    struct Employee *employees = loadFromFile(filename);
    requestsHead = loadRequests(requestFile);
    printf("***************WELCOME***************\n");
    printf("******ENTER 0 IN ENTER ID FORM TO EXIT FROM LOOP************\n");
    while (1) {
        int id;
        char name[50];
        printf("Enter your ID: ");
        scanf("%d", &id);
        if (id==0){
            break;
        }
        getchar(); // consume leftover newline
        printf("Enter your name: ");
        fgets(name, sizeof(name), stdin);
        trimNewline(name);

        if (Login_handler(id, name, filename, &employees) == 0) {
            printf("Login failed. Try again.\n");
        }
    }
    printf("******THANK YOU*********");
    return 0;
}
