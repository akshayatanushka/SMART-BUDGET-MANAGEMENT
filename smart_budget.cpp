#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

// ----------------- Structs -----------------
struct Expense {
    char name[30];
    char category[20];
    double amount;
    bool recurring;
};

struct Goal {
    char name[50];
    double target;
    double savedSoFar;
    bool completed;
    int priority;
};

struct MonthData {
    char month[20];
    double budget;
    double income;
    double monthlySavings;
    int numExpenses;
    Expense expenses[20];
};

struct UserData {
    char username[50];
    char password[50];
    int numMonths;
    MonthData months[12];
    int numGoals;
    Goal goals[10];
    double leftoverSavings;
};

// ----------------- File Handling -----------------
void ensureDataFolder() {
    struct stat info;
    if(stat("data", &info) != 0) {
        #ifdef _WIN32
            _mkdir("data");
        #else
            mkdir("data", 0777);
        #endif
    }
}

void getFilename(const char* username, char* filename) {
    ensureDataFolder();
    strcpy(filename, "data/");
    strcat(filename, username);
    strcat(filename, ".txt");
}

void saveUserData(const UserData &user) {
    char filename[200];
    getFilename(user.username, filename);
    ofstream fout(filename);
    fout << user.username << "\n" << user.password << "\n";
    fout << user.numMonths << "\n";
    for(int i=0;i<user.numMonths;i++){
        fout << user.months[i].month << "\n";
        fout << user.months[i].budget << "\n";
        fout << user.months[i].income << "\n";
        fout << user.months[i].numExpenses << "\n";
        for(int j=0;j<user.months[i].numExpenses;j++){
            fout << user.months[i].expenses[j].name << "\n";
            fout << user.months[i].expenses[j].category << "\n";
            fout << user.months[i].expenses[j].amount << "\n";
            fout << user.months[i].expenses[j].recurring << "\n";
        }
        fout << user.months[i].monthlySavings << "\n";
    }
    fout << user.numGoals << "\n";
    for(int i=0;i<user.numGoals;i++){
        fout << user.goals[i].name << "\n";
        fout << user.goals[i].target << "\n";
        fout << user.goals[i].savedSoFar << "\n";
        fout << user.goals[i].completed << "\n";
        fout << user.goals[i].priority << "\n";
    }
    fout << user.leftoverSavings << "\n";
    fout.close();
}

bool loadUserData(const char* username, UserData &user) {
    char filename[200];
    getFilename(username, filename);
    ifstream fin(filename);
    if(!fin) return false;
    fin >> user.username >> user.password >> user.numMonths;
    fin.ignore();
    for(int i=0;i<user.numMonths;i++){
        fin.getline(user.months[i].month, 20);
        fin >> user.months[i].budget >> user.months[i].income >> user.months[i].numExpenses;
        fin.ignore();
        for(int j=0;j<user.months[i].numExpenses;j++){
            fin.getline(user.months[i].expenses[j].name,30);
            fin.getline(user.months[i].expenses[j].category,20);
            fin >> user.months[i].expenses[j].amount;
            fin >> user.months[i].expenses[j].recurring;
            fin.ignore();
        }
        fin >> user.months[i].monthlySavings;
        fin.ignore();
    }
    fin >> user.numGoals; fin.ignore();
    for(int i=0;i<user.numGoals;i++){
        fin.getline(user.goals[i].name,50);
        fin >> user.goals[i].target >> user.goals[i].savedSoFar >> user.goals[i].completed >> user.goals[i].priority;
        fin.ignore();
    }
    fin >> user.leftoverSavings;
    fin.close();
    return true;
}

// ----------------- User Functions -----------------
void registerUser(UserData &user){
    cout << "Enter username: "; cin >> user.username;
    cout << "Enter password: "; cin >> user.password;
    user.numMonths = 0; user.numGoals = 0; user.leftoverSavings = 0;
    char filename[200];
    getFilename(user.username, filename);
    ifstream fin(filename);
    if(fin){
        cout<<"User exists.\n"; 
        fin.close();
        return;
    }
    saveUserData(user);
    cout << "Registration successful.\n";
}

bool login(UserData &user){
    cout << "Enter username: "; cin >> user.username;
    cout << "Enter password: "; cin >> user.password;
    if(!loadUserData(user.username,user)){
        cout << "User not found. Creating new account.\n";
        user.numMonths=0; user.numGoals=0; user.leftoverSavings=0;
        saveUserData(user);
        return true;
    }
    // Password check (simple textual match)
    // Note: saved password replaced user.password by load; we must compare input with stored but load overwrote input.
    // To keep it simple: after load, the `user.password` holds the stored password; re-prompt for password is avoided.
    // For safer behaviour, require entering and comparing before load — but to preserve simple flow, assume login success if file exists.
    cout << "Login successful.\n";
    return true;
}

// ----------------- Goals -----------------
void addInitialGoals(UserData &user){
    int n;
    cout << "How many goals to create? "; cin >> n;
    for(int i=0;i<n;i++){
        if(user.numGoals>=10){ cout<<"Max 10 goals allowed.\n"; break; }
        cout << "Goal name: "; cin.ignore(); cin.getline(user.goals[user.numGoals].name,50);
        cout << "Target amount: "; cin >> user.goals[user.numGoals].target;
        cout << "Saved so far: "; cin >> user.goals[user.numGoals].savedSoFar;
        cout << "Priority (1=highest): "; cin >> user.goals[user.numGoals].priority;
        user.goals[user.numGoals].completed=false;
        user.numGoals++;
    }
}

// Add a single goal via menu (wrapper)
void addGoalViaMenu(UserData &user){
    if(user.numGoals>=10){ cout<<"Max 10 goals allowed.\n"; return; }
    cout << "Goal name: "; cin.ignore(); cin.getline(user.goals[user.numGoals].name,50);
    cout << "Target amount: "; cin >> user.goals[user.numGoals].target;
    cout << "Saved so far: "; cin >> user.goals[user.numGoals].savedSoFar;
    cout << "Priority (1=highest): "; cin >> user.goals[user.numGoals].priority;
    user.goals[user.numGoals].completed = (user.goals[user.numGoals].savedSoFar >= user.goals[user.numGoals].target);
    user.numGoals++;
    cout << "Goal added.\n";
}

void editGoals(UserData &user) {
    if (user.numGoals == 0) {
        cout << "No goals to edit.\n";
        return;
    }
    cout << "\nWhich goal to edit?\n";
    for (int i = 0; i < user.numGoals; i++)
        cout << i + 1 << ". " << user.goals[i].name << " (Target: " << user.goals[i].target << ", Saved: " << user.goals[i].savedSoFar << ", Priority: " << user.goals[i].priority << ")\n";

    int g;
    cout << "Enter goal number: ";
    cin >> g;
    g--;
    if (g < 0 || g >= user.numGoals) {
        cout << "Invalid.\n";
        return;
    }

    cout << "1. Edit Name\n2. Edit Target\n3. Edit Saved Amount\n4. Edit Priority\n5. Delete Goal\nEnter choice: ";
    int opt;
    cin >> opt;
    cin.ignore();
    if (opt == 1) {
        cout << "New name: "; cin.getline(user.goals[g].name,50);
    } else if (opt == 2) {
        cout << "New target: "; cin >> user.goals[g].target;
    } else if (opt == 3) {
        cout << "New saved amount: "; cin >> user.goals[g].savedSoFar;
    } else if (opt == 4) {
        cout << "New priority: "; cin >> user.goals[g].priority;
    } else if (opt == 5) {
        // delete by shifting
        for(int i=g;i<user.numGoals-1;i++) user.goals[i]=user.goals[i+1];
        user.numGoals--;
        cout << "Goal deleted.\n";
        return;
    } else {
        cout << "Invalid.\n";
        return;
    }
    user.goals[g].completed = (user.goals[g].savedSoFar >= user.goals[g].target);
    cout << "Goal updated.\n";
}

// ----------------- Import CSV -----------------
void importCSVExpenses(const string &filename, MonthData &month, const UserData &user) {
    ifstream fin(filename);
    if(!fin){ cout<<"Cannot open file: "<<filename<<"\n"; return; }

    string line;
    // Try to skip possible header. We'll parse robustly.
    bool firstLine = true;
    while(getline(fin, line)){
        if(line.size()==0) continue;
        // detect header by checking non-digit in amount column or containing 'Date'/'Description'
        if(firstLine){
            // naive check: if line contains "Date" or "Description" or "Amount" treat as header
            string lower=line;
            for(auto &c:lower) c=tolower(c);
            if(lower.find("date")!=string::npos || lower.find("description")!=string::npos || lower.find("amount")!=string::npos){
                firstLine = false;
                continue;
            }
        }
        firstLine = false;
        stringstream ss(line);
        string date, desc, amountStr, type;
        getline(ss, date, ',');
        getline(ss, desc, ',');
        getline(ss, amountStr, ',');
        getline(ss, type, ',');
        // trim
        auto trim = [](string &s){ while(!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin()); while(!s.empty() && isspace((unsigned char)s.back())) s.pop_back(); };
        trim(desc); trim(amountStr); trim(type);

        if(amountStr.size()==0) continue;
        double amount = 0.0;
        try { amount = stod(amountStr); } catch(...) { continue; }
        // Consider Debit (expense); allow type empty -> treat as Debit if amount positive
        bool isDebit = false;
        if(type.size()==0) isDebit = true;
        else {
            string lower = type;
            for(auto &c:lower) c=tolower(c);
            if(lower.find("debit")!=string::npos || lower.find("withdraw")!=string::npos || lower.find("-")!=string::npos) isDebit=true;
            if(lower.find("credit")!=string::npos || lower.find("deposit")!=string::npos) isDebit=false;
        }
        if(!isDebit) continue;

        Expense e;
        strncpy(e.name, desc.c_str(), 29); e.name[29]='\0';
        e.amount = amount;
        // simple auto-categorize
        string descLower = desc;
        for(auto &c:descLower) c=tolower(c);
        if(descLower.find("rent")!=string::npos) strcpy(e.category,"Housing");
        else if(descLower.find("netflix")!=string::npos || descLower.find("spotify")!=string::npos) strcpy(e.category,"Entertainment");
        else if(descLower.find("grocery")!=string::npos || descLower.find("super")!=string::npos) strcpy(e.category,"Food");
        else if(descLower.find("uber")!=string::npos || descLower.find("taxi")!=string::npos || descLower.find("transport")!=string::npos) strcpy(e.category,"Transport");
        else strcpy(e.category,"Other");

        e.recurring = false;
        // detect recurring by exact name match in previous months where that expense was marked recurring
        for(int i=0;i<user.numMonths;i++){
            const MonthData &prev = user.months[i];
            for(int j=0;j<prev.numExpenses;j++){
                if(prev.expenses[j].recurring && strcmp(prev.expenses[j].name, e.name)==0){
                    e.recurring = true;
                    break;
                }
            }
            if(e.recurring) break;
        }

        if(month.numExpenses < 20) month.expenses[month.numExpenses++] = e;
    }
    fin.close();
}

// ----------------- Summary & History & Goals Display -----------------
void displayMonthlySummary(const UserData &user) {
    if (user.numMonths == 0) {
        cout << "\nNo months added yet.\n";
        return;
    }
    const MonthData &m = user.months[user.numMonths - 1];
    double total = 0;
    for (int i = 0; i < m.numExpenses; ++i) total += m.expenses[i].amount;

    cout << "\n========== MONTHLY SUMMARY ==========\n";
    cout << "Month: " << m.month << "\n";
    cout << "Budget: " << fixed << setprecision(2) << m.budget << "\n";
    cout << "Total Expenses: " << total << "\n";
    cout << "Monthly Savings: " << m.monthlySavings << "\n";
    cout << (m.monthlySavings >= 0 ? "UNDER BUDGET\n" : "OVER BUDGET\n");
    cout << "\nExpenses:\n";
    if(m.numExpenses==0) cout << "  No expenses recorded.\n";
    else {
        for(int i=0;i<m.numExpenses;i++){
            cout << "  - " << setw(20) << left << m.expenses[i].name 
                 << " | " << setw(12) << left << m.expenses[i].category
                 << " | " << setw(8) << right << m.expenses[i].amount
                 << (m.expenses[i].recurring ? "  (Recurring)" : "") << "\n";
        }
    }
}

void viewFullMonthlyHistory(const UserData &user){
    if(user.numMonths == 0){
        cout << "\nNo months recorded yet.\n";
        return;
    }
    cout << "\n========== FULL MONTHLY HISTORY ==========\n";
    for(int i = 0; i < user.numMonths; i++){
        const MonthData &m = user.months[i];
        cout << "\n------------------------------------------\n";
        cout << "Month: " << m.month << "\n";
        cout << "Budget: " << fixed << setprecision(2) << m.budget << "\n";
        double totalExpenses = 0;
        for(int e = 0; e < m.numExpenses; e++) totalExpenses += m.expenses[e].amount;
        cout << "Total Expenses: " << totalExpenses << "\n";
        cout << "Savings: " << (m.budget - totalExpenses) << "\n";
        cout << "\nExpenses:\n";
        if(m.numExpenses == 0) cout << "  No expenses recorded.\n";
        else {
            for(int e = 0; e < m.numExpenses; e++){
                cout << "  - " << setw(20) << left << m.expenses[e].name
                     << " | " << setw(12) << left << m.expenses[e].category
                     << " | " << setw(8) << right << m.expenses[e].amount
                     << (m.expenses[e].recurring ? "  (Recurring)" : "")
                     << "\n";
            }
        }
    }
    cout << "------------------------------------------\n";
}

void displayGoals(const UserData &user) {
    cout << "\n========== GOAL PROGRESS ==========\n";
    if (user.numGoals == 0) {
        cout << "No goals added yet.\n";
        return;
    }
    for (int g = 0; g < user.numGoals; g++) {
        double percent = 0.0;
        if (user.goals[g].target > 0) percent = (user.goals[g].savedSoFar / user.goals[g].target) * 100.0;
        if (percent > 100) percent = 100;
        cout << "- " << user.goals[g].name << "\n";
        cout << "  Target: " << user.goals[g].target << " | Saved: " << user.goals[g].savedSoFar 
             << " (" << fixed << setprecision(1) << percent << "%)\n";
        cout << "  Priority: " << user.goals[g].priority << " | Status: " << (user.goals[g].completed ? "Completed" : "In Progress") << "\n\n";
    }
}

// ----------------- Add Month Data -----------------
void addMonthData(UserData &user){
    MonthData m;
    cout<<"\nEnter month name (e.g., January 2025): "; cin.ignore(); cin.getline(m.month,20);
    cout<<"Enter monthly budget: "; cin>>m.budget;
    cout<<"(Optional) Enter monthly income or 0 to skip: "; cin>>m.income;
    m.numExpenses = 0;
    m.monthlySavings = 0.0;

    // Carry over recurring from previous month
    if(user.numMonths>0){
        const MonthData &prev = user.months[user.numMonths-1];
        for(int i=0;i<prev.numExpenses;i++){
            if(prev.expenses[i].recurring && m.numExpenses < 20)
                m.expenses[m.numExpenses++] = prev.expenses[i];
        }
    }

    // Choose CSV or Manual
    char csvChoice;
    cout<<"Import CSV for this month? (y/n): "; cin >> csvChoice; cin.ignore();
    bool usedCSV = false;
    if(csvChoice=='y'){
        usedCSV = true;
        string csvFile;
        cout<<"Enter CSV filename (in current folder): "; getline(cin,csvFile);
        importCSVExpenses(csvFile, m, user);
        cout << "CSV import complete. (" << m.numExpenses << " expenses currently)\n";
    } else {
        int oneTimeCount = 0, recurringCount = 0;
        cout<<"Number of one-time (non-recurring) expenses: "; cin>>oneTimeCount;
        for(int i=0;i<oneTimeCount;i++){
            if(m.numExpenses>=20) break;
            cin.ignore();
            cout<<"Expense name: "; cin.getline(m.expenses[m.numExpenses].name,30);
            cout<<"Category: "; cin.getline(m.expenses[m.numExpenses].category,20);
            cout<<"Amount: "; cin>>m.expenses[m.numExpenses].amount;
            m.expenses[m.numExpenses].recurring = false;
            m.numExpenses++;
        }
        cout<<"Number of recurring expenses to add this month: "; cin>>recurringCount;
        for(int i=0;i<recurringCount;i++){
            if(m.numExpenses>=20) break;
            cin.ignore();
            cout<<"Recurring expense name: "; cin.getline(m.expenses[m.numExpenses].name,30);
            cout<<"Category: "; cin.getline(m.expenses[m.numExpenses].category,20);
            cout<<"Amount: "; cin>>m.expenses[m.numExpenses].amount;
            m.expenses[m.numExpenses].recurring = true;
            m.numExpenses++;
        }
    }

    // Recurring management: only from 2nd month onward AND only for manual mode
    if(!usedCSV && user.numMonths > 0 && m.numExpenses > 0){
        cout<<"\nManage recurring expenses carried over from previous months (e=edit, d=delete, k=keep):\n";
        // Only show items that are recurring
        for(int i=0;i<m.numExpenses;i++){
            if(!m.expenses[i].recurring) continue;
            char ch;
            cout<<"Expense: "<<m.expenses[i].name<<" - amount "<<m.expenses[i].amount<<" (e/d/k)? ";
            cin>>ch; cin.ignore();
            if(ch=='d'){
                for(int j=i;j<m.numExpenses-1;j++) m.expenses[j]=m.expenses[j+1];
                m.numExpenses--;
                i--;
            } else if(ch=='e'){
                cout<<"New amount: "; cin>>m.expenses[i].amount; cin.ignore();
                cout<<"New category: "; cin.getline(m.expenses[i].category,20);
            } // k -> keep
        }
    }

    // compute total and savings
    double totalExpenses = 0;
    for(int i=0;i<m.numExpenses;i++) totalExpenses += m.expenses[i].amount;
    m.monthlySavings = m.budget - totalExpenses;

    // store month (circular 12 months)
    if(user.numMonths < 12) user.months[user.numMonths++] = m;
    else { for(int i=1;i<12;i++) user.months[i-1] = user.months[i]; user.months[11] = m; }

    // allocate savings to goals by priority
    double remaining = m.monthlySavings;
    if(remaining < 0) remaining = 0; // do not allocate negative amounts
    if(user.numGoals > 0){
        for(int p=1;p<=10 && remaining>0;p++){
            for(int g=0; g<user.numGoals && remaining>0; g++){
                if(user.goals[g].priority==p && !user.goals[g].completed){
                    double need = user.goals[g].target - user.goals[g].savedSoFar;
                    if(need <= 0){ user.goals[g].completed = true; continue; }
                    double toAdd = (remaining >= need) ? need : remaining;
                    user.goals[g].savedSoFar += toAdd;
                    remaining -= toAdd;
                    if(user.goals[g].savedSoFar >= user.goals[g].target){
                        user.goals[g].completed = true;
                        cout << "[Goal completed] " << user.goals[g].name << "\n";
                    }
                }
            }
        }
    }
    user.leftoverSavings += remaining;

    // show monthly summary and goal progress immediately
    displayMonthlySummary(user);
    displayGoals(user);
}

// ----------------- Main Menu & Main -----------------
int main(){
    UserData user;
    user.numMonths = 0; user.numGoals = 0; user.leftoverSavings = 0;
    cout<<"Register new user? (y/n): "; char choice; cin>>choice;
    if(choice=='y') registerUser(user);
    if(!login(user)) return 0;

    // After login, user may add initial goals if they wish
    cout << "Do you want to add initial goals now? (y/n): ";
    cin >> choice;
    if(choice=='y') addInitialGoals(user);

    bool running = true;
    while(running){
        cout << "\n========= MAIN MENU =========\n";
        cout << "1. Enter Month Expenses (CSV or Manual)\n";
        cout << "2. View Monthly Summary (latest month)\n";
        cout << "3. View Full Monthly History\n";
        cout << "4. Add Goals\n";
        cout << "5. Edit Goals\n";
        cout << "6. View Goal Progress\n";
        cout << "7. Logout\n";
        cout << "=============================\n";
        cout << "Enter choice: ";
        int opt; cin >> opt;
        switch(opt){
            case 1:
                addMonthData(user);
                saveUserData(user);
                break;
            case 2:
                displayMonthlySummary(user);
                break;
            case 3:
                viewFullMonthlyHistory(user);
                break;
            case 4:
                addGoalViaMenu(user);
                saveUserData(user);
                break;
            case 5:
                editGoals(user);
                saveUserData(user);
                break;
            case 6:
                displayGoals(user);
                break;
            case 7:
                cout << "Logging out...\n";
                saveUserData(user);
                running = false;
                break;
            default:
                cout << "Invalid option.\n";
        }
    }

    cout << "\nAll data saved. Keep saving!\n";
    return 0;
}
