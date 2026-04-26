# SMART-BUDGET-MANAGEMENT
SMART BUDGET MANAGER is a lightweight offline C++ finance tracker that saves all user data locally without extra dependencies. It helps users record monthly budgets, track recurring expenses, manage prioritized financial goals, view savings progress across months, and maintain a clear personal financial history through a simple console interface.
The application operates through a command-line menu system and saves all user data to a local file (data/<username>.txt).

🔑 User Management
Registration: The first time a user logs in, or if they explicitly choose to register, a new account is created. The program uses the entered username to create a data file in the data/ folder.

Login: Users log in with a username and password. If a user is not found, the program offers to create a new account for that username.

Data Persistence: All user data (months, expenses, goals) is loaded on login and saved upon choosing Logout (Option 7) or after adding/editing data in the main menu (Options 1, 4, 5).

📅 Expense Tracking (Option 1)
New Month Setup: Prompts for the month name (e.g., "March 2025"), monthly budget, and optional monthly income.

Recurring Expense Carryover: Automatically carries over recurring expenses from the previous month, allowing the user to edit, delete, or keep them for the current month.

Manual Entry: Requires manual entry for a count of one-time expenses and new recurring expenses.

Calculation: Computes the total expenses and the Monthly Savings (Budget - Total Expenses).

🎯 Goal Management (Options 4, 5, 6)
Add Goals (Option 4): Allows adding new savings goals with a Name, Target Amount, Saved So Far, and a Priority (1 being highest).

Edit Goals (Option 5): Provides a menu to modify the name, target, saved amount, or priority of an existing goal, or to delete a goal entirely.

Goal Allocation: When new month data is entered (Option 1), any positive Monthly Savings are automatically allocated to goals based on Priority (1st priority goals are filled first) until the goal is complete or the savings run out. Remaining savings are added to leftoverSavings.

View Progress (Option 6): Displays the name, target, saved amount, percentage completion, priority, and completion status for all goals.

📊 Reporting (Options 2, 3)
Monthly Summary (Option 2): Displays a detailed breakdown of the latest month's data, including the total budget, total expenses, net monthly savings, and a list of all expenses with their category and recurring status. It indicates if the user is UNDER BUDGET or OVER BUDGET.

Full History (Option 3): Iterates through all recorded months and displays a summary of the budget, total expenses, and savings for each, along with a list of that month's expenses.

⚙️ Constraints and Data Structure
The application uses C-style arrays (char name[30], etc.) and fixed-size arrays for data storage (up to 12 months, 20 expenses per month, 10 goals).

Data is stored using simple text files, with fields separated by newlines, requiring careful use of cin.ignore() and getline() during the file loading process (loadUserData).

A data folder is created automatically if it doesn't exist to store user files.
