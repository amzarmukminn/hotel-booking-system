#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

// ================================================================
//  GLOBAL CONSTANTS & ARRAYS
// ================================================================
const int    MAX_ROOMS      = 20;
const int    MAX_BOOKINGS   = 20;
const double RATE_PER_NIGHT = 150.00;
const int    MAX_USERS      = 10;

// Room data
int    roomNumbers[MAX_ROOMS];
bool   roomActive[MAX_ROOMS];
bool   roomUnderMaintenance[MAX_ROOMS];

// Booking arrays
int    bookedRoomNumber[MAX_BOOKINGS];
string guestName       [MAX_BOOKINGS];
string bookingDate     [MAX_BOOKINGS];
int    daysStayed      [MAX_BOOKINGS];
double totalBill       [MAX_BOOKINGS];
bool   isBooked        [MAX_BOOKINGS];
string bookedBy        [MAX_BOOKINGS];

// User Accounts
string userNames[MAX_USERS];
string userPasswords[MAX_USERS];
int userCount = 0;

enum Role { ADMIN, USER, INVALID, EXIT_PROGRAM };

// ================================================================
//  FUNCTION PROTOTYPES
// ================================================================
void displayHeader();
void displayMenu(Role role);
void bookRoom(Role role, const string& currentUsername);
void calculateBill(int index);
void displayAllBookings(Role role, const string& currentUsername);
void displaySingleBooking(int index);
void cancelBookingAdmin();
void cancelMyBooking(const string& currentUsername);
void editBooking();
void editMyBooking(const string& currentUsername);
void addRoom();
void updateRoomMaintenance();
int  getValidInt(const string& prompt, int minVal, int maxVal);
char getYesNo(const string& prompt);
void pauseScreen();
Role signInMenu(string& outUsername);
Role adminSignIn(string& outUsername);
Role userSignIn(string& outUsername);
void createUserAccount();
bool userExists(const string& username);
bool roomExists(int roomNumber);
int  dateToDays(const string& dateStr);
string daysToDate(int totalDays);
string getCheckoutDate(const string& checkin, int days);
bool checkDateOverlap(const string& date1, int days1, const string& date2, int days2);
bool hasBookingConflict(int roomNumber, const string& date, int days, int ignoreBookingId = -1);
int  findEmptyBookingSlot();
string getRequiredText(const string& prompt);

// ================================================================
//  DATE & CONFLICT LOGIC
// ================================================================
int dateToDays(const string& dateStr) {
    if (dateStr.length() < 10) return 0;
    int dd = stoi(dateStr.substr(0, 2));
    int mm = stoi(dateStr.substr(3, 2));
    int yyyy = stoi(dateStr.substr(6, 4));
    return yyyy * 365 + mm * 30 + dd;
}

string daysToDate(int totalDays) {
    int yyyy = totalDays / 365;
    int rem = totalDays % 365;
    int mm = rem / 30;
    int dd = rem % 30;
    if (dd == 0) { dd = 30; mm--; }
    if (mm == 0) { mm = 12; yyyy--; }
    char buffer[11];
    sprintf(buffer, "%02d/%02d/%04d", dd, mm, yyyy);
    return string(buffer);
}

string getCheckoutDate(const string& checkin, int days) {
    int start = dateToDays(checkin);
    int end = start + days;
    return daysToDate(end);
}

bool checkDateOverlap(const string& date1, int days1, const string& date2, int days2) {
    int start1 = dateToDays(date1);
    int end1 = start1 + days1;
    int start2 = dateToDays(date2);
    int end2 = start2 + days2;
    return (start1 < end2) && (start2 < end1);
}

bool hasBookingConflict(int roomNumber, const string& date, int days, int ignoreBookingId) {
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        if (isBooked[i] && i != ignoreBookingId && bookedRoomNumber[i] == roomNumber) {
            if (checkDateOverlap(bookingDate[i], daysStayed[i], date, days)) {
                return true;
            }
        }
    }
    return false;
}

// ================================================================
//  HELPERS
// ================================================================
int getValidInt(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Invalid input. Please enter a number between "
                 << minVal << " and " << maxVal << ".\n";
            continue;
        }
        if (value < minVal || value > maxVal) {
            cout << "  [!] Please enter a value between "
                 << minVal << " and " << maxVal << ".\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
}

char getYesNo(const string& prompt) {
    char answer;
    while (true) {
        cout << prompt << " (y/n): ";
        cin >> answer;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (answer == 'y' || answer == 'Y' || answer == 'n' || answer == 'N')
            return tolower(answer);
        cout << "  [!] Please enter 'y' or 'n'.\n";
    }
}

void pauseScreen() {
    cout << "\n  Press Enter to return...";
    cin.get();
}

string getRequiredText(const string& prompt) {
    string text;
    while (true) {
        cout << prompt;
        getline(cin, text);
        size_t start = text.find_first_not_of(" \t");
        if (start == string::npos || text.empty()) {
            cout << "  [!] This field cannot be empty.\n";
            continue;
        }
        return text.substr(start);
    }
}

bool roomExists(int roomNumber) {
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (roomActive[i] && roomNumbers[i] == roomNumber) return true;
    }
    return false;
}

int findEmptyBookingSlot() {
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        if (!isBooked[i]) return i;
    }
    return -1;
}

// ================================================================
//  SIGN IN SYSTEM
// ================================================================
Role signInMenu(string& outUsername) {
    while (true) {
        cout << "\n  ============================================" << endl;
        cout << "              HOTEL SYSTEM SIGN IN           " << endl;
        cout << "  ============================================" << endl;
        cout << "  1. Admin Sign In" << endl;
        cout << "  2. User Sign In" << endl;
        cout << "  3. Create User Account" << endl;
        cout << "  0. Exit System" << endl;
        cout << "  --------------------------------------------" << endl;

        int choice = getValidInt("  Enter your choice: ", 0, 3);

        if (choice == 1) return adminSignIn(outUsername);
        else if (choice == 2) return userSignIn(outUsername);
        else if (choice == 3) {
            createUserAccount();
        } else {
            return EXIT_PROGRAM;
        }
    }
}

Role adminSignIn(string& outUsername) {
    string password;
    cout << "\n  --- ADMIN SIGN IN ---" << endl;
    cout << "  Enter username: ";
    getline(cin, outUsername);
    cout << "  Enter password: ";
    getline(cin, password);

    if (outUsername == "admin" && password == "admin123") return ADMIN;

    cout << "\n  [!] Invalid admin credentials.\n";
    return INVALID;
}

Role userSignIn(string& outUsername) {
    string password;
    cout << "\n  --- USER SIGN IN ---" << endl;
    cout << "  Enter username: ";
    getline(cin, outUsername);
    cout << "  Enter password: ";
    getline(cin, password);

    for (int i = 0; i < userCount; i++) {
        if (userNames[i] == outUsername && userPasswords[i] == password) return USER;
    }

    cout << "\n  [!] Invalid user credentials.\n";
    return INVALID;
}

void createUserAccount() {
    string username, password;
    cout << "\n  --- CREATE USER ACCOUNT ---" << endl;

    if (userCount >= MAX_USERS) {
        cout << "  [!] User account limit reached.\n";
        pauseScreen();
        return;
    }

    while (true) {
        cout << "  Enter new username: ";
        getline(cin, username);
        if (username.empty()) {
            cout << "  [!] Username cannot be empty.\n";
            continue;
        }
        if (username == "admin" || userExists(username)) {
            cout << "  [!] Username already exists. Please choose another.\n";
            continue;
        }
        break;
    }

    while (true) {
        cout << "  Enter new password: ";
        getline(cin, password);
        if (password.empty()) {
            cout << "  [!] Password cannot be empty.\n";
            continue;
        }
        break;
    }

    userNames[userCount] = username;
    userPasswords[userCount] = password;
    userCount++;

    cout << "\n  [*] Account created successfully! You can now sign in.\n";
    pauseScreen();
}

bool userExists(const string& username) {
    for (int i = 0; i < userCount; i++) {
        if (userNames[i] == username) return true;
    }
    return false;
}

// ================================================================
//  MAIN FUNCTION
// ================================================================
int main() {
    // Initialise rooms: first 5 active, others inactive
    for (int i = 0; i < MAX_ROOMS; i++) {
        roomActive[i] = false;
        roomUnderMaintenance[i] = false;
        roomNumbers[i] = 0;
    }
    int initialRooms[5] = {101, 102, 103, 104, 105};
    for (int i = 0; i < 5; i++) {
        roomNumbers[i] = initialRooms[i];
        roomActive[i] = true;
    }

    // Initialise bookings
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        bookedRoomNumber[i] = 0;
        guestName[i] = "";
        bookingDate[i] = "";
        daysStayed[i] = 0;
        totalBill[i] = 0.0;
        isBooked[i] = false;
        bookedBy[i] = "";
    }

    while (true) {
        Role currentRole = INVALID;
        string currentUsername = "";
        
        currentRole = signInMenu(currentUsername);

        if (currentRole == EXIT_PROGRAM) {
            cout << "\n  Thank you for using the Hotel Booking System. Shutting down...\n";
            break;
        }

        if (currentRole == INVALID) continue;

        cout << "\n  Welcome, " << (currentRole == ADMIN ? "Administrator" : currentUsername) << "!\n";
        
        bool sessionActive = true;
        
        do {
            displayHeader();
            displayMenu(currentRole);

            int maxChoice = (currentRole == ADMIN) ? 7 : 5;  // admin: 1‑7, user: 1‑5
            int choice = getValidInt("  Enter your choice: ", 1, maxChoice);

            if (currentRole == ADMIN) {
                switch (choice) {
                    case 1: bookRoom(currentRole, currentUsername); break;
                    case 2: displayAllBookings(currentRole, currentUsername); pauseScreen(); break;
                    case 3: editBooking(); break;
                    case 4: cancelBookingAdmin(); break;
                    case 5: addRoom(); break;
                    case 6: updateRoomMaintenance(); break;
                    case 7:
                        cout << "\n  Signing out... Returning to Sign In menu.\n";
                        sessionActive = false;
                        break;
                }
            } else { // USER
                switch (choice) {
                    case 1: bookRoom(currentRole, currentUsername); break;
                    case 2: displayAllBookings(currentRole, currentUsername); pauseScreen(); break;
                    case 3: editMyBooking(currentUsername); break;
                    case 4: cancelMyBooking(currentUsername); break;
                    case 5:
                        cout << "\n  Signing out... Returning to Sign In menu.\n";
                        sessionActive = false;
                        break;
                }
            }
        } while (sessionActive);
    }
    return 0;
}

// ================================================================
//  HEADER & MENU
// ================================================================
void displayHeader() {
    cout << "\n  ============================================" << endl;
    cout << "       GRAND IMPIANA HOTEL, KUALA LUMPUR    " << endl;
    cout << "           Room Booking System              " << endl;
    cout << "  ============================================" << endl;
}

void displayMenu(Role role) {
    cout << "\n  --- MAIN MENU ---" << endl;
    cout << "  1. Book a Room" << endl;
    cout << "  2. View Bookings" << endl;
    
    if (role == ADMIN) {
        cout << "  3. Edit a Booking" << endl;
        cout << "  4. Cancel a Booking" << endl;
        cout << "  5. Add a Room" << endl;
        cout << "  6. Update Room (Maintenance)" << endl;
        cout << "  7. Sign Out" << endl;
    } else {
        cout << "  3. Edit My Booking" << endl;
        cout << "  4. Cancel My Booking" << endl;
        cout << "  5. Sign Out" << endl;
    }
    
    cout << "  ------------------" << endl;
}

// ================================================================
//  BOOK ROOM
// ================================================================
void bookRoom(Role role, const string& currentUsername) {
    cout << "\n  --- BOOK A ROOM ---" << endl;

    int slot = findEmptyBookingSlot();
    if (slot == -1) {
        cout << "\n  [!] Booking storage is full.\n";
        pauseScreen();
        return;
    }

    string date = getRequiredText("  Enter check-in date (DD/MM/YYYY): ");
    int days = getValidInt("  Enter number of days (min 1): ", 1, 365);

    cout << "\n  Available Rooms for " << date << " (Stay: " << days << " days):" << endl;
    bool anyAvailable = false;

    for (int i = 0; i < MAX_ROOMS; i++) {
        if (roomActive[i] && !roomUnderMaintenance[i]) {
            if (!hasBookingConflict(roomNumbers[i], date, days)) {
                cout << "    Room " << roomNumbers[i] << endl;
                anyAvailable = true;
            }
        }
    }

    if (!anyAvailable) {
        cout << "\n  [!] Sorry, all rooms are booked or under maintenance for those dates.\n";
        pauseScreen();
        return;
    }

    cout << "\n  (Enter 0 to go back)" << endl;
    int roomChoice = getValidInt("  Enter room number to book: ", 0, 999);
    if (roomChoice == 0) return;

    if (!roomExists(roomChoice)) {
        cout << "\n  [!] Room " << roomChoice << " does not exist.\n";
        pauseScreen();
        return;
    }

    // find index of that room to check maintenance
    int roomIdx = -1;
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (roomActive[i] && roomNumbers[i] == roomChoice) {
            roomIdx = i;
            break;
        }
    }
    if (roomIdx != -1 && roomUnderMaintenance[roomIdx]) {
        cout << "\n  [!] Room " << roomChoice << " is currently under maintenance.\n";
        pauseScreen();
        return;
    }

    if (hasBookingConflict(roomChoice, date, days)) {
        cout << "\n  [!] Room " << roomChoice << " is not available during those dates.\n";
        pauseScreen();
        return;
    }

    string name = getRequiredText("  Enter guest name: ");

    bookedRoomNumber[slot] = roomChoice;
    guestName[slot]        = name;
    bookingDate[slot]      = date;
    daysStayed[slot]       = days;
    isBooked[slot]         = true;
    bookedBy[slot]         = currentUsername; 
    calculateBill(slot);

    cout << "\n  [*] Booking Successful!" << endl;
    displaySingleBooking(slot);
    pauseScreen();
}

void calculateBill(int index) {
    totalBill[index] = daysStayed[index] * RATE_PER_NIGHT;
}

// ================================================================
//  DISPLAY BOOKINGS
// ================================================================
void displayAllBookings(Role role, const string& currentUsername) {
    if (role == ADMIN) cout << "\n  --- ALL CURRENT BOOKINGS (ADMIN VIEW) ---" << endl;
    else cout << "\n  --- YOUR BOOKINGS ---" << endl;

    bool anyBooked = false;
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        if (isBooked[i]) {
            if (role == ADMIN || bookedBy[i] == currentUsername) {
                displaySingleBooking(i);
                anyBooked = true;
            }
        }
    }

    if (!anyBooked) cout << "\n  No bookings found.\n" << endl;
}

void displaySingleBooking(int index) {
    string checkoutDate = getCheckoutDate(bookingDate[index], daysStayed[index]);
    cout << fixed << setprecision(2);
    cout << "\n  ----------------------------" << endl;
    cout << "  Booking ID   : " << (index + 1) << endl;
    cout << "  Booked By    : " << bookedBy[index] << endl;
    cout << "  Room Number  : " << bookedRoomNumber[index] << endl;
    cout << "  Guest Name   : " << guestName[index] << endl;
    cout << "  Check-in Date: " << bookingDate[index] << endl;
    cout << "  Check-out Date: " << checkoutDate << endl;
    cout << "  Days Stayed  : " << daysStayed[index] << " day(s)" << endl;
    cout << "  Total Bill   : RM " << totalBill[index] << endl;
    cout << "  ----------------------------\n" << endl;
}

// ================================================================
//  EDIT BOOKING (Admin Only)
// ================================================================
void editBooking() {
    cout << "\n  --- EDIT BOOKING (ADMIN ONLY) ---" << endl;
    
    displayAllBookings(ADMIN, "admin"); 

    cout << "  (Enter 0 to go back)" << endl;
    int bookingId = getValidInt("  Enter Booking ID to edit: ", 0, MAX_BOOKINGS);
    if (bookingId == 0) return;

    int index = bookingId - 1;
    if (!isBooked[index]) {
        cout << "\n  [!] Booking ID does not exist.\n";
        pauseScreen();
        return;
    }

    bool done = false;
    while (!done) {
        cout << "\n  Current Booking:" << endl;
        displaySingleBooking(index);
        cout << "  1. Change Room\n  2. Change Date & Duration\n  0. Cancel Edit\n";
        int editChoice = getValidInt("  Select option: ", 0, 2);

        if (editChoice == 0) {
            done = true;
            continue;
        }

        if (editChoice == 1) {
            int newRoom = getValidInt("  Enter new room number: ", 101, 999);
            if (!roomExists(newRoom)) {
                cout << "  [!] Invalid room number.\n";
                if (getYesNo("  Do you want to change date/duration instead?") == 'y') {
                    // switch to date change
                    editChoice = 2; // we'll handle below
                } else {
                    continue;
                }
            } else {
                int rIdx = -1;
                for (int i = 0; i < MAX_ROOMS; i++) {
                    if (roomActive[i] && roomNumbers[i] == newRoom) { rIdx = i; break; }
                }
                if (rIdx != -1 && roomUnderMaintenance[rIdx]) {
                    cout << "  [!] That room is under maintenance.\n";
                    if (getYesNo("  Do you want to change date/duration instead?") == 'y') {
                        editChoice = 2; // force date change
                    } else {
                        continue;
                    }
                } else if (hasBookingConflict(newRoom, bookingDate[index], daysStayed[index], index)) {
                    cout << "  [!] That room is already booked during those dates.\n";
                    if (getYesNo("  Do you want to change date/duration instead?") == 'y') {
                        editChoice = 2;
                    } else {
                        continue;
                    }
                } else {
                    // Success – update room
                    bookedRoomNumber[index] = newRoom;
                    cout << "  [*] Room updated successfully.\n";
                    done = true;
                    continue;
                }
            }
        }

        if (editChoice == 2) {
            string newDate = getRequiredText("  Enter new check-in date (DD/MM/YYYY): ");
            int newDays = getValidInt("  Enter new duration (days): ", 1, 365);
            
            if (hasBookingConflict(bookedRoomNumber[index], newDate, newDays, index)) {
                cout << "  [!] The room is not available for those new dates.\n";
                if (getYesNo("  Do you want to change the room instead?") == 'y') {
                    // force room change – loop back to top
                    continue;
                } else {
                    continue;
                }
            } else {
                bookingDate[index] = newDate;
                daysStayed[index] = newDays;
                calculateBill(index);
                cout << "  [*] Date & duration updated successfully.\n";
                done = true;
            }
        }
    }
    pauseScreen();
}

// ================================================================
//  USER EDIT THEIR OWN BOOKING (with similar conflict handling)
// ================================================================
void editMyBooking(const string& currentUsername) {
    cout << "\n  --- EDIT MY BOOKING ---" << endl;

    // List user's own bookings
    bool any = false;
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        if (isBooked[i] && bookedBy[i] == currentUsername) {
            displaySingleBooking(i);
            any = true;
        }
    }
    if (!any) {
        cout << "\n  You have no bookings to edit.\n";
        pauseScreen();
        return;
    }

    cout << "  (Enter 0 to go back)" << endl;
    int bookingId = getValidInt("  Enter Booking ID to edit: ", 0, MAX_BOOKINGS);
    if (bookingId == 0) return;

    int index = bookingId - 1;
    if (!isBooked[index] || bookedBy[index] != currentUsername) {
        cout << "\n  [!] You cannot edit this booking.\n";
        pauseScreen();
        return;
    }

    bool done = false;
    while (!done) {
        cout << "\n  Current Booking:" << endl;
        displaySingleBooking(index);
        cout << "  1. Change Room\n  2. Change Date & Duration\n  0. Cancel Edit\n";
        int editChoice = getValidInt("  Select option: ", 0, 2);

        if (editChoice == 0) {
            done = true;
            continue;
        }

        if (editChoice == 1) {
            int newRoom = getValidInt("  Enter new room number: ", 101, 999);
            if (!roomExists(newRoom)) {
                cout << "  [!] Invalid room number.\n";
                if (getYesNo("  Do you want to change date/duration instead?") == 'y') {
                    editChoice = 2;
                } else {
                    continue;
                }
            } else {
                int rIdx = -1;
                for (int i = 0; i < MAX_ROOMS; i++) {
                    if (roomActive[i] && roomNumbers[i] == newRoom) { rIdx = i; break; }
                }
                if (rIdx != -1 && roomUnderMaintenance[rIdx]) {
                    cout << "  [!] That room is under maintenance.\n";
                    if (getYesNo("  Do you want to change date/duration instead?") == 'y') {
                        editChoice = 2;
                    } else {
                        continue;
                    }
                } else if (hasBookingConflict(newRoom, bookingDate[index], daysStayed[index], index)) {
                    cout << "  [!] That room is already booked during those dates.\n";
                    if (getYesNo("  Do you want to change date/duration instead?") == 'y') {
                        editChoice = 2;
                    } else {
                        continue;
                    }
                } else {
                    bookedRoomNumber[index] = newRoom;
                    cout << "  [*] Room updated successfully.\n";
                    done = true;
                    continue;
                }
            }
        }

        if (editChoice == 2) {
            string newDate = getRequiredText("  Enter new check-in date (DD/MM/YYYY): ");
            int newDays = getValidInt("  Enter new duration (days): ", 1, 365);
            
            if (hasBookingConflict(bookedRoomNumber[index], newDate, newDays, index)) {
                cout << "  [!] The room is not available for those new dates.\n";
                if (getYesNo("  Do you want to change the room instead?") == 'y') {
                    continue; // loop back to room change option
                } else {
                    continue;
                }
            } else {
                bookingDate[index] = newDate;
                daysStayed[index] = newDays;
                calculateBill(index);
                cout << "  [*] Date & duration updated successfully.\n";
                done = true;
            }
        }
    }
    pauseScreen();
}

// ================================================================
//  CANCEL BOOKING (Admin Only)
// ================================================================
void cancelBookingAdmin() {
    cout << "\n  --- CANCEL BOOKING (ADMIN ONLY) ---" << endl;
    
    displayAllBookings(ADMIN, "admin"); 

    cout << "  (Enter 0 to go back)" << endl;
    int bookingId = getValidInt("  Enter booking ID to cancel: ", 0, MAX_BOOKINGS);
    if (bookingId == 0) return;

    int index = bookingId - 1;
    if (!isBooked[index]) {
        cout << "\n  [!] Booking ID does not exist.\n";
        pauseScreen();
        return;
    }

    bookedRoomNumber[index] = 0;
    guestName[index]        = "";
    bookingDate[index]      = "";
    daysStayed[index]       = 0;
    totalBill[index]        = 0.0;
    isBooked[index]         = false;
    bookedBy[index]         = "";

    cout << "  [*] Booking cancelled successfully.\n";
    pauseScreen();
}

// ================================================================
//  CANCEL MY BOOKING (User)
// ================================================================
void cancelMyBooking(const string& currentUsername) {
    cout << "\n  --- CANCEL MY BOOKING ---" << endl;

    // List user's own bookings
    bool any = false;
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        if (isBooked[i] && bookedBy[i] == currentUsername) {
            displaySingleBooking(i);
            any = true;
        }
    }
    if (!any) {
        cout << "\n  You have no bookings to cancel.\n";
        pauseScreen();
        return;
    }

    cout << "  (Enter 0 to go back)" << endl;
    int bookingId = getValidInt("  Enter booking ID to cancel: ", 0, MAX_BOOKINGS);
    if (bookingId == 0) return;

    int index = bookingId - 1;
    if (!isBooked[index] || bookedBy[index] != currentUsername) {
        cout << "\n  [!] You cannot cancel this booking.\n";
        pauseScreen();
        return;
    }

    // Confirm cancellation
    if (getYesNo("  Are you sure you want to cancel this booking?") == 'n') {
        cout << "  Cancellation aborted.\n";
        pauseScreen();
        return;
    }

    bookedRoomNumber[index] = 0;
    guestName[index]        = "";
    bookingDate[index]      = "";
    daysStayed[index]       = 0;
    totalBill[index]        = 0.0;
    isBooked[index]         = false;
    bookedBy[index]         = "";

    cout << "  [*] Your booking has been cancelled.\n";
    pauseScreen();
}

// ================================================================
//  ADD ROOM (Admin Only)
// ================================================================
void addRoom() {
    cout << "\n  --- ADD ROOM (ADMIN ONLY) ---" << endl;

    int slot = -1;
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (!roomActive[i]) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        cout << "  [!] No more room slots available (max " << MAX_ROOMS << ").\n";
        pauseScreen();
        return;
    }

    cout << "\n  (Enter 0 to go back)" << endl;
    int newRoomNumber = getValidInt("  Enter room number to add: ", 0, 999);
    if (newRoomNumber == 0) {
        cout << "\n  Returning to Main Menu...\n";
        return;
    }

    if (roomExists(newRoomNumber)) {
        cout << "  [!] Room " << newRoomNumber << " already exists.\n";
        pauseScreen();
        return;
    }

    roomNumbers[slot] = newRoomNumber;
    roomActive[slot] = true;
    roomUnderMaintenance[slot] = false;

    cout << "  [*] Room " << newRoomNumber << " added successfully.\n";
    pauseScreen();
}

// ================================================================
//  UPDATE ROOM MAINTENANCE (Admin Only)
// ================================================================
void updateRoomMaintenance() {
    cout << "\n  --- UPDATE ROOM MAINTENANCE (ADMIN ONLY) ---" << endl;

    cout << "\n  Current Rooms:" << endl;
    bool any = false;
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (roomActive[i]) {
            cout << "    Room " << roomNumbers[i]
                 << " | Maintenance: " << (roomUnderMaintenance[i] ? "Yes" : "No")
                 << endl;
            any = true;
        }
    }
    if (!any) {
        cout << "  No active rooms.\n";
        pauseScreen();
        return;
    }

    cout << "\n  (Enter 0 to go back)" << endl;
    int roomChoice = getValidInt("  Enter room number to toggle maintenance: ", 0, 999);
    if (roomChoice == 0) {
        cout << "\n  Returning to Main Menu...\n";
        return;
    }

    if (!roomExists(roomChoice)) {
        cout << "  [!] Room not found.\n";
        pauseScreen();
        return;
    }

    int idx = -1;
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (roomActive[i] && roomNumbers[i] == roomChoice) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        cout << "  [!] Room not found.\n";
        pauseScreen();
        return;
    }

    roomUnderMaintenance[idx] = !roomUnderMaintenance[idx];
    cout << "  [*] Room " << roomChoice << " maintenance status is now "
         << (roomUnderMaintenance[idx] ? "ON" : "OFF") << ".\n";
    pauseScreen();
}